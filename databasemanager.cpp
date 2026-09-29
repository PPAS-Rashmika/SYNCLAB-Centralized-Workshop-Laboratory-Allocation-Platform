#include "databasemanager.h"

#include <QRegularExpression>
#include <QSet>
#include <QSqlError>
#include <QSqlQuery>
#include <QUuid>
#include <QVariant>

namespace {
const QString recordColumns = QStringLiteral(
    "id, booking_id, workshop_id, name, index_no, department, email, pdf_path, "
    "record_date, time_slot, description, passcode, created_at, status, admin_remarks, category");

QStringList splitSlots(const QString &value)
{
    QStringList slotList;
    for (const QString &part : value.split(',')) {
        const QString slot = part.trimmed();
        if (!slot.isEmpty() && !slotList.contains(slot))
            slotList.append(slot);
    }
    return slotList;
}

QString canonicalStatus(const QString &value)
{
    const QString status = value.trimmed().toLower();
    if (status == "pending") return QStringLiteral("Pending");
    if (status == "approve" || status == "approved") return QStringLiteral("Approve");
    if (status == "reject" || status == "rejected") return QStringLiteral("Reject");
    return {};
}

bool validDate(const QString &value)
{
    const QDate date = QDate::fromString(value, Qt::ISODate);
    return date.isValid() && date.toString(Qt::ISODate) == value;
}

StudentRecord readRecord(const QSqlQuery &query)
{
    StudentRecord record;
    record.setId(query.value(0).toInt());
    record.setBookingId(query.value(1).toString());
    record.setWorkshopId(query.value(2).toString());
    record.setName(query.value(3).toString());
    record.setIndexNo(query.value(4).toString());
    record.setDepartment(query.value(5).toString());
    record.setEmail(query.value(6).toString());
    record.setPdfPath(query.value(7).toString());
    record.setRecordDate(query.value(8).toString());
    record.setTimeSlots(query.value(9).toString());
    record.setDescription(query.value(10).toString());
    record.setPasscode(query.value(11).toString());
    record.setCreatedAt(query.value(12).toString());
    record.setStatus(query.value(13).toString());
    record.setAdminRemarks(query.value(14).toString());
    record.setCategory(query.value(15).toString());
    return record;
}
}

SqliteDatabaseManager::SqliteDatabaseManager(const QString &dbName)
    : connectionName(QStringLiteral("workshop-") + QUuid::createUuid().toString(QUuid::WithoutBraces))
{
    db = QSqlDatabase::addDatabase(QStringLiteral("QSQLITE"), connectionName);
    db.setConnectOptions(QStringLiteral("QSQLITE_BUSY_TIMEOUT=5000"));
    db.setDatabaseName(dbName);
    if (!db.open()) lastError = db.lastError().text();
}

SqliteDatabaseManager::~SqliteDatabaseManager()
{
    db.close();
    db = QSqlDatabase();
    QSqlDatabase::removeDatabase(connectionName);
}

bool SqliteDatabaseManager::ensureOpen()
{
    lastError.clear();
    if (db.isOpen()) return true;
    lastError = db.lastError().text();
    if (lastError.isEmpty()) lastError = QStringLiteral("The booking database is not open.");
    return false;
}

bool SqliteDatabaseManager::beginWrite()
{
    if (!ensureOpen()) return false;
    // Reserve the SQLite writer before checking availability so another process
    // cannot insert a booking between the check and the write.
    QSqlQuery query(db);
    if (query.exec(QStringLiteral("BEGIN IMMEDIATE"))) return true;
    lastError = query.lastError().text();
    return false;
}

bool SqliteDatabaseManager::commitWrite()
{
    if (db.commit()) return true;
    lastError = db.lastError().text();
    rollbackWrite();
    return false;
}

void SqliteDatabaseManager::rollbackWrite()
{
    // Keep the original operation's error for the caller.
    db.rollback();
}

bool SqliteDatabaseManager::createTable()
{
    if (!beginWrite()) return false;
    QSqlQuery query(db);
    if (!query.exec(QStringLiteral(
            "CREATE TABLE IF NOT EXISTS records ("
            "id INTEGER PRIMARY KEY AUTOINCREMENT, "
            "booking_id TEXT, workshop_id TEXT, category TEXT, name TEXT, index_no TEXT, "
            "department TEXT, email TEXT, pdf_path TEXT, record_date TEXT, time_slot TEXT, "
            "description TEXT, passcode TEXT, created_at TEXT, status TEXT DEFAULT 'Pending', admin_remarks TEXT)"))) {
        lastError = query.lastError().text();
        rollbackWrite();
        return false;
    }

    // Older installations may have a records table without the newer fields.
    // Add missing columns in place, retaining all existing rows and identifiers.
    QSet<QString> columns;
    if (!query.exec(QStringLiteral("PRAGMA table_info(records)"))) {
        lastError = query.lastError().text();
        rollbackWrite();
        return false;
    }
    while (query.next()) columns.insert(query.value(1).toString().toLower());
    query.finish();
    if (!columns.contains(QStringLiteral("id"))) {
        lastError = QStringLiteral("The records table has no ID column. Restore a compatible booking database.");
        rollbackWrite();
        return false;
    }
    const QStringList textColumns = {"booking_id", "workshop_id", "category", "name", "index_no",
        "department", "email", "pdf_path", "record_date", "time_slot", "description", "passcode",
        "created_at", "status", "admin_remarks"};
    for (const QString &column : textColumns) {
        if (columns.contains(column)) continue;
        const QString defaultValue = column == "status" ? "'Pending'" : "''";
        if (!query.exec(QStringLiteral("ALTER TABLE records ADD COLUMN %1 TEXT DEFAULT %2").arg(column, defaultValue))) {
            lastError = query.lastError().text();
            rollbackWrite();
            return false;
        }
    }
    const QStringList migrations = {
        "UPDATE records SET status = 'Pending' WHERE status IS NULL OR trim(status) = '' OR lower(trim(status)) = 'pending'",
        "UPDATE records SET status = 'Approve' WHERE lower(trim(status)) IN ('approve', 'approved')",
        "UPDATE records SET status = 'Reject' WHERE lower(trim(status)) IN ('reject', 'rejected')",
        "CREATE INDEX IF NOT EXISTS records_category_date ON records(category, record_date)"
    };
    for (const QString &statement : migrations) {
        if (!query.exec(statement)) {
            lastError = query.lastError().text();
            rollbackWrite();
            return false;
        }
    }
    return commitWrite();
}

bool SqliteDatabaseManager::validateRecord(const StudentRecord &record)
{
    if (record.getCategory().trimmed().isEmpty() || !validDate(record.getRecordDate())
            || splitSlots(record.getTimeSlots()).isEmpty()) {
        lastError = QStringLiteral("Choose a workshop, a valid booking date, and at least one time slot.");
        return false;
    }
    if (record.getName().trimmed().isEmpty() || record.getIndexNo().trimmed().isEmpty()
            || record.getDepartment().trimmed().isEmpty()) {
        lastError = QStringLiteral("Name, index number, and department are required.");
        return false;
    }
    static const QRegularExpression emailPattern(QStringLiteral("^[A-Za-z0-9._%+-]+@[A-Za-z0-9.-]+\\.[A-Za-z]{2,}$"));
    if (!emailPattern.match(record.getEmail().trimmed()).hasMatch()) {
        lastError = QStringLiteral("Enter a valid email address.");
        return false;
    }
    return true;
}

bool SqliteDatabaseManager::addRecord(const StudentRecord &record)
{
    if (!ensureOpen() || !validateRecord(record)) return false;
    const QString status = canonicalStatus(record.getStatus());
    if (record.getBookingId().trimmed().isEmpty() || status.isEmpty()) {
        lastError = QStringLiteral("A booking ID and a valid booking status are required.");
        return false;
    }
    if (!beginWrite()) return false;
    QSqlQuery query(db);
    query.prepare(QStringLiteral("SELECT id FROM records WHERE booking_id = :booking_id LIMIT 1"));
    query.bindValue(":booking_id", record.getBookingId());
    if (!query.exec()) {
        lastError = query.lastError().text();
        rollbackWrite();
        return false;
    }
    const bool duplicate = query.next();
    query.finish();
    if (duplicate) {
        lastError = QStringLiteral("This booking ID already exists. Please submit the booking again.");
        rollbackWrite();
        return false;
    }
    if (status != "Reject" && !isTimeSlotAvailable(record.getRecordDate(), record.getCategory(), splitSlots(record.getTimeSlots()))) {
        rollbackWrite();
        return false;
    }
    query.prepare(QStringLiteral(
        "INSERT INTO records (booking_id, workshop_id, category, name, index_no, department, email, pdf_path, record_date, time_slot, description, passcode, created_at, status, admin_remarks) "
        "VALUES (:b_id, :w_id, :category, :name, :index, :dept, :email, :pdf, :date, :time, :desc, :passcode, :created_at, :status, :remarks)"));
    query.bindValue(":b_id", record.getBookingId());
    query.bindValue(":w_id", record.getWorkshopId());
    query.bindValue(":category", record.getCategory());
    query.bindValue(":name", record.getName().trimmed());
    query.bindValue(":index", record.getIndexNo().trimmed());
    query.bindValue(":dept", record.getDepartment().trimmed());
    query.bindValue(":email", record.getEmail().trimmed());
    query.bindValue(":pdf", record.getPdfPath());
    query.bindValue(":date", record.getRecordDate());
    query.bindValue(":time", splitSlots(record.getTimeSlots()).join(", "));
    query.bindValue(":desc", record.getDescription());
    query.bindValue(":passcode", record.getPasscode());
    query.bindValue(":created_at", record.getCreatedAt());
    query.bindValue(":status", status);
    query.bindValue(":remarks", record.getAdminRemarks());
    if (!query.exec()) {
        lastError = query.lastError().text();
        rollbackWrite();
        return false;
    }
    return commitWrite();
}

bool SqliteDatabaseManager::updateRecord(const StudentRecord &record)
{
    if (!ensureOpen() || !validateRecord(record)) return false;
    const QString status = canonicalStatus(record.getStatus());
    if (status.isEmpty()) {
        lastError = QStringLiteral("Choose Pending, Approve, or Reject as the booking status.");
        return false;
    }
    if (!beginWrite()) return false;
    QSqlQuery query(db);
    query.prepare(QStringLiteral("SELECT id FROM records WHERE id = :id"));
    query.bindValue(":id", record.getId());
    if (!query.exec()) {
        lastError = query.lastError().text();
        rollbackWrite();
        return false;
    }
    if (!query.next()) {
        lastError = QStringLiteral("The booking no longer exists. Refresh the booking list.");
        query.finish();
        rollbackWrite();
        return false;
    }
    query.finish();
    if (status != "Reject" && !isTimeSlotAvailable(record.getRecordDate(), record.getCategory(), splitSlots(record.getTimeSlots()), record.getId())) {
        rollbackWrite();
        return false;
    }
    query.prepare(QStringLiteral("UPDATE records SET workshop_id=:workshop, category=:category, name=:name, index_no=:index, department=:dept, email=:email, pdf_path=:pdf, record_date=:date, time_slot=:time, description=:desc, status=:status WHERE id=:id"));
    query.bindValue(":workshop", record.getWorkshopId());
    query.bindValue(":category", record.getCategory());
    query.bindValue(":name", record.getName().trimmed());
    query.bindValue(":index", record.getIndexNo().trimmed());
    query.bindValue(":dept", record.getDepartment().trimmed());
    query.bindValue(":email", record.getEmail().trimmed());
    query.bindValue(":pdf", record.getPdfPath());
    query.bindValue(":date", record.getRecordDate());
    query.bindValue(":time", splitSlots(record.getTimeSlots()).join(", "));
    query.bindValue(":desc", record.getDescription());
    query.bindValue(":status", status);
    query.bindValue(":id", record.getId());
    if (!query.exec()) {
        lastError = query.lastError().text();
        rollbackWrite();
        return false;
    }
    return commitWrite();
}

bool SqliteDatabaseManager::deleteRecord(int id)
{
    if (!ensureOpen()) return false;
    QSqlQuery query(db);
    query.prepare(QStringLiteral("DELETE FROM records WHERE id = :id"));
    query.bindValue(":id", id);
    if (!query.exec()) {
        lastError = query.lastError().text();
        return false;
    }
    if (query.numRowsAffected() != 1) {
        lastError = QStringLiteral("The booking no longer exists. Refresh the booking list.");
        return false;
    }
    return true;
}

bool SqliteDatabaseManager::updateRecordStatus(int id, const QString &value)
{
    if (!ensureOpen()) return false;
    const QString status = canonicalStatus(value);
    if (status.isEmpty()) {
        lastError = QStringLiteral("Choose Pending, Approve, or Reject as the booking status.");
        return false;
    }
    if (!beginWrite()) return false;
    QSqlQuery query(db);
    query.prepare(QStringLiteral("SELECT record_date, category, time_slot FROM records WHERE id = :id"));
    query.bindValue(":id", id);
    if (!query.exec()) {
        lastError = query.lastError().text();
        rollbackWrite();
        return false;
    }
    if (!query.next()) {
        lastError = QStringLiteral("The booking no longer exists. Refresh the booking list.");
        query.finish();
        rollbackWrite();
        return false;
    }
    const QString date = query.value(0).toString();
    const QString category = query.value(1).toString();
    const QStringList slotList = splitSlots(query.value(2).toString());
    query.finish();
    if (status != "Reject" && !isTimeSlotAvailable(date, category, slotList, id)) {
        rollbackWrite();
        return false;
    }
    query.prepare(QStringLiteral("UPDATE records SET status = :status WHERE id = :id"));
    query.bindValue(":status", status);
    query.bindValue(":id", id);
    if (!query.exec()) {
        lastError = query.lastError().text();
        rollbackWrite();
        return false;
    }
    return commitWrite();
}

bool SqliteDatabaseManager::updateAdminRemarks(int id, const QString &remarks)
{
    if (!ensureOpen()) return false;
    QSqlQuery query(db);
    query.prepare(QStringLiteral("UPDATE records SET admin_remarks = :remarks WHERE id = :id"));
    query.bindValue(":remarks", remarks);
    query.bindValue(":id", id);
    if (!query.exec()) {
        lastError = query.lastError().text();
        return false;
    }
    if (query.numRowsAffected() != 1) {
        lastError = QStringLiteral("The booking no longer exists. Refresh the booking list.");
        return false;
    }
    return true;
}

QList<StudentRecord> SqliteDatabaseManager::getRecordsByDateAndCategory(const QString &date, const QString &category)
{
    QList<StudentRecord> records;
    if (!ensureOpen()) return records;
    QSqlQuery query(db);
    query.prepare("SELECT " + recordColumns + " FROM records WHERE record_date = :date AND category = :category ORDER BY id DESC");
    query.bindValue(":date", date);
    query.bindValue(":category", category);
    if (!query.exec()) {
        lastError = query.lastError().text();
        return records;
    }
    while (query.next()) records.append(readRecord(query));
    return records;
}

QList<StudentRecord> SqliteDatabaseManager::getAllRecordsForCategory(const QString &category)
{
    QList<StudentRecord> records;
    if (!ensureOpen()) return records;
    QSqlQuery query(db);
    query.prepare("SELECT " + recordColumns + " FROM records WHERE category = :category ORDER BY record_date DESC, id DESC");
    query.bindValue(":category", category);
    if (!query.exec()) {
        lastError = query.lastError().text();
        return records;
    }
    while (query.next()) records.append(readRecord(query));
    return records;
}

QList<QDate> SqliteDatabaseManager::getBookedDates(const QString &category)
{
    QList<QDate> dates;
    if (!ensureOpen()) return dates;
    QSqlQuery query(db);
    query.prepare(QStringLiteral("SELECT DISTINCT record_date FROM records WHERE category = :category "
                                 "AND lower(trim(COALESCE(status, 'Pending'))) NOT IN ('reject', 'rejected') ORDER BY record_date"));
    query.bindValue(":category", category);
    if (!query.exec()) {
        lastError = query.lastError().text();
        return dates;
    }
    while (query.next()) {
        const QDate date = QDate::fromString(query.value(0).toString(), Qt::ISODate);
        if (date.isValid()) dates.append(date);
    }
    return dates;
}

bool SqliteDatabaseManager::isTimeSlotAvailable(const QString &date, const QString &category, const QStringList &requestedSlots, int excludeRecordId)
{
    if (!ensureOpen()) return false;
    const QStringList slotList = splitSlots(requestedSlots.join(','));
    if (!validDate(date) || category.trimmed().isEmpty() || slotList.isEmpty()) {
        lastError = QStringLiteral("Choose a workshop, a valid booking date, and at least one time slot.");
        return false;
    }
    const QStringList bookedSlots = getBookedTimeSlotsStringList(date, category, excludeRecordId);
    if (!lastError.isEmpty()) return false;
    for (const QString &slot : slotList) {
        if (bookedSlots.contains(slot)) {
            lastError = QStringLiteral("The selected time slot is already booked. Choose another time slot.");
            return false;
        }
    }
    return true;
}

QStringList SqliteDatabaseManager::getBookedTimeSlotsStringList(const QString &date, const QString &category, int excludeRecordId)
{
    QStringList slotList;
    if (!ensureOpen()) return slotList;
    QSqlQuery query(db);
    query.prepare(QStringLiteral("SELECT time_slot FROM records WHERE record_date = :date AND category = :category AND id != :id "
                                 "AND lower(trim(COALESCE(status, 'Pending'))) NOT IN ('reject', 'rejected')"));
    query.bindValue(":date", date);
    query.bindValue(":category", category);
    query.bindValue(":id", excludeRecordId);
    if (!query.exec()) {
        lastError = query.lastError().text();
        return slotList;
    }
    while (query.next()) {
        for (const QString &slot : splitSlots(query.value(0).toString())) {
            if (!slotList.contains(slot)) slotList.append(slot);
        }
    }
    return slotList;
}

QString SqliteDatabaseManager::getLastError() const
{
    return lastError;
}

