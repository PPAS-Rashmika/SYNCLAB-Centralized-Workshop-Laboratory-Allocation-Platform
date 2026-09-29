#include "databasemanager.h"

#include <QRegularExpression>
#include <QSet>
#include <QSqlError>
#include <QSqlQuery>
#include <QUuid>
#include <QVariant>

// ---------------------------------------------------------------------------
// Small helper functions. They are only used inside this file, so they live in
// an unnamed namespace instead of needing their own header file.
// ---------------------------------------------------------------------------
namespace
{
// "8.30 - 9.30, 9.30 - 10.30" becomes a clean list with no blanks and no repeats.
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

// The program only ever stores three statuses. Anything else is refused.
QString canonicalStatus(const QString &value)
{
    const QString status = value.trimmed().toLower();
    if (status == "pending")
        return "Pending";
    if (status == "approve" || status == "approved")
        return "Approve";
    if (status == "reject" || status == "rejected")
        return "Reject";
    return QString();
}

// Booking dates are stored as text in ISO form, for example 2026-09-29.
bool validDate(const QString &value)
{
    const QDate date = QDate::fromString(value, Qt::ISODate);
    return date.isValid() && date.toString(Qt::ISODate) == value;
}

// The columns every SELECT below asks for.
// Important: this order must match the value(0)..value(15) order in readRecord().
const QString recordColumns =
    "id, workshop_id, name, index_no, department, email, "
    "record_date, time_slot, description, passcode, created_at, status, admin_remarks, category, "
    "supervisor_name, project_name";

// Turn one SQL row into a StudentRecord object.
StudentRecord readRecord(const QSqlQuery &query)
{
    StudentRecord record;
    record.setId(query.value(0).toInt());
    record.setWorkshopId(query.value(1).toString());
    record.setName(query.value(2).toString());
    record.setIndexNo(query.value(3).toString());
    record.setDepartment(query.value(4).toString());
    record.setEmail(query.value(5).toString());
    record.setRecordDate(query.value(6).toString());
    record.setTimeSlots(query.value(7).toString());
    record.setDescription(query.value(8).toString());
    record.setPasscode(query.value(9).toString());
    record.setCreatedAt(query.value(10).toString());
    record.setStatus(query.value(11).toString());
    record.setAdminRemarks(query.value(12).toString());
    record.setCategory(query.value(13).toString());
    record.setSupervisorName(query.value(14).toString());
    record.setProjectName(query.value(15).toString());
    return record;
}
} // namespace

// ---------------------------------------------------------------------------
// Opening and closing the database
// ---------------------------------------------------------------------------

// Constructor: open the SQLite database file whose path was given.
SqliteDatabaseManager::SqliteDatabaseManager(const QString &dbName)
    : connectionName("workshop-" + QUuid::createUuid().toString(QUuid::WithoutBraces))
{
    // Ask Qt for the SQLite driver. The unique connection name means this
    // object never clashes with another connection in the same program.
    db = QSqlDatabase::addDatabase("QSQLITE", connectionName);
    db.setConnectOptions("QSQLITE_BUSY_TIMEOUT=5000"); // wait 5 seconds if the file is locked
    db.setDatabaseName(dbName);
    if (!db.open())
        lastError = db.lastError().text();
}

// Destructor: close the connection and hand the name back to Qt.
SqliteDatabaseManager::~SqliteDatabaseManager()
{
    db.close();
    db = QSqlDatabase();
    QSqlDatabase::removeDatabase(connectionName);
}

// Every operation starts here so the user always gets a clear message.
bool SqliteDatabaseManager::ensureOpen()
{
    lastError.clear();
    if (db.isOpen())
        return true;
    lastError = db.lastError().text();
    if (lastError.isEmpty())
        lastError = "The booking database is not open.";
    return false;
}

// Start a transaction. A transaction groups a check and its write together,
// so a booking can never be saved while its time slot is already taken.
bool SqliteDatabaseManager::beginWrite()
{
    if (!ensureOpen())
        return false;
    if (db.transaction())
        return true;
    lastError = db.lastError().text();
    return false;
}

// Save the transaction. If saving fails, undo it so the database is unchanged.
bool SqliteDatabaseManager::commitWrite()
{
    if (db.commit())
        return true;
    lastError = db.lastError().text();
    db.rollback();
    return false;
}

// Check a booking before it is written to the database.
bool SqliteDatabaseManager::validateRecord(const StudentRecord &record)
{
    if (record.getCategory().trimmed().isEmpty() || !validDate(record.getRecordDate()) ||
        splitSlots(record.getTimeSlots()).isEmpty()) {
        lastError = "Choose a workshop, a valid booking date, and at least one time slot.";
        return false;
    }
    if (record.getName().trimmed().isEmpty() || record.getIndexNo().trimmed().isEmpty() ||
        record.getDepartment().trimmed().isEmpty() ||
        record.getSupervisorName().trimmed().isEmpty() ||
        record.getProjectName().trimmed().isEmpty()) {
        lastError = "Name, index number, department, supervisor, and project name are required.";
        return false;
    }

    // A simple pattern that accepts ordinary email addresses.
    static const QRegularExpression emailPattern(
        "^[A-Za-z0-9._%+-]+@[A-Za-z0-9.-]+\\.[A-Za-z]{2,}$");
    if (!emailPattern.match(record.getEmail().trimmed()).hasMatch()) {
        lastError = "Enter a valid email address.";
        return false;
    }
    return true;
}

// ---------------------------------------------------------------------------
// Creating the table
// ---------------------------------------------------------------------------

// Create the records table the first time the program runs.
// An older database file is also upgraded here, and no existing booking is lost.
bool SqliteDatabaseManager::createTable()
{
    if (!beginWrite())
        return false;

    QSqlQuery query(db);

    // CREATE TABLE IF NOT EXISTS only builds the table when it is not there yet.
    if (!query.exec("CREATE TABLE IF NOT EXISTS records ("
                    "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                    "workshop_id TEXT, category TEXT, name TEXT, index_no TEXT, "
                    "department TEXT, email TEXT, supervisor_name TEXT, project_name TEXT, "
                    "record_date TEXT, time_slot TEXT, "
                    "description TEXT, passcode TEXT, created_at TEXT, "
                    "status TEXT DEFAULT 'Pending', admin_remarks TEXT)")) {
        lastError = query.lastError().text();
        db.rollback();
        return false;
    }

    // PRAGMA table_info tells us which columns the table already has.
    QSet<QString> columns;
    if (!query.exec("PRAGMA table_info(records)")) {
        lastError = query.lastError().text();
        db.rollback();
        return false;
    }
    while (query.next())
        columns.insert(query.value(1).toString().toLower());
    query.finish();

    if (!columns.contains("id")) {
        lastError = "The records table has no ID column. Restore a compatible booking database.";
        db.rollback();
        return false;
    }

    // Add every column that an older database file is missing.
    const QStringList textColumns = {"workshop_id", "category", "name", "index_no",
                                     "department", "email", "supervisor_name", "project_name",
                                     "record_date", "time_slot", "description", "passcode",
                                     "created_at", "status", "admin_remarks"};
    for (const QString &column : textColumns) {
        if (columns.contains(column))
            continue;
        const QString defaultValue = (column == "status") ? "'Pending'" : "''";
        const QString sql =
            QString("ALTER TABLE records ADD COLUMN %1 TEXT DEFAULT %2").arg(column, defaultValue);
        if (!query.exec(sql)) {
            lastError = query.lastError().text();
            db.rollback();
            return false;
        }
    }

    // Make sure every saved status uses exactly one of the three allowed words.
    const QStringList migrations = {
        "UPDATE records SET status = 'Pending' WHERE status IS NULL OR trim(status) = '' "
        "OR lower(trim(status)) = 'pending'",
        "UPDATE records SET status = 'Approve' WHERE lower(trim(status)) IN ('approve', 'approved')",
        "UPDATE records SET status = 'Reject' WHERE lower(trim(status)) IN ('reject', 'rejected')",
        // An index makes searching by workshop and date much faster.
        "CREATE INDEX IF NOT EXISTS records_category_date ON records(category, record_date)"};
    for (const QString &statement : migrations) {
        if (!query.exec(statement)) {
            lastError = query.lastError().text();
            db.rollback();
            return false;
        }
    }

    return commitWrite();
}

// ---------------------------------------------------------------------------
// Writing: INSERT, UPDATE and DELETE
// ---------------------------------------------------------------------------

// Insert one new booking.
bool SqliteDatabaseManager::addRecord(const StudentRecord &record)
{
    if (!ensureOpen() || !validateRecord(record))
        return false;

    const QString status = canonicalStatus(record.getStatus());
    if (record.getPasscode().trimmed().isEmpty() || status.isEmpty()) {
        lastError = "An allocation key and a valid booking status are required.";
        return false;
    }
    if (!beginWrite())
        return false;

    QSqlQuery query(db);

    // A prepared query keeps the SQL text and the values separate, which stops
    // a value from being read as SQL code.
    query.prepare("SELECT id FROM records WHERE passcode = :passcode LIMIT 1");
    query.bindValue(":passcode", record.getPasscode());
    if (!query.exec()) {
        lastError = query.lastError().text();
        db.rollback();
        return false;
    }
    const bool duplicateKey = query.next();
    query.finish();
    if (duplicateKey) {
        lastError = "This allocation key already exists. Please submit the booking again.";
        db.rollback();
        return false;
    }

    // A rejected booking does not hold its time slots, so it is not checked.
    if (status != "Reject" &&
        !isTimeSlotAvailable(record.getRecordDate(), record.getCategory(),
                             splitSlots(record.getTimeSlots()))) {
        db.rollback();
        return false;
    }

    // INSERT the new row.
    query.prepare("INSERT INTO records (workshop_id, category, name, index_no, "
                  "department, email, supervisor_name, project_name, "
                  "record_date, time_slot, description, "
                  "passcode, created_at, status, admin_remarks) "
                  "VALUES (:w_id, :category, :name, :index, :dept, :email, "
                  ":supervisor, :project, "
                  ":date, :time, :desc, :passcode, :created_at, :status, :remarks)");
    query.bindValue(":w_id", record.getWorkshopId());
    query.bindValue(":category", record.getCategory());
    query.bindValue(":name", record.getName().trimmed());
    query.bindValue(":index", record.getIndexNo().trimmed());
    query.bindValue(":dept", record.getDepartment().trimmed());
    query.bindValue(":email", record.getEmail().trimmed());
    query.bindValue(":supervisor", record.getSupervisorName().trimmed());
    query.bindValue(":project", record.getProjectName().trimmed());
    query.bindValue(":date", record.getRecordDate());
    query.bindValue(":time", splitSlots(record.getTimeSlots()).join(", "));
    query.bindValue(":desc", record.getDescription());
    query.bindValue(":passcode", record.getPasscode());
    query.bindValue(":created_at", record.getCreatedAt());
    query.bindValue(":status", status);
    query.bindValue(":remarks", record.getAdminRemarks());
    if (!query.exec()) {
        lastError = query.lastError().text();
        db.rollback();
        return false;
    }
    return commitWrite();
}

// Save changes to an existing booking.
// The allocation key, the created date and the admin remarks are never changed here.
bool SqliteDatabaseManager::updateRecord(const StudentRecord &record)
{
    if (!ensureOpen() || !validateRecord(record))
        return false;

    const QString status = canonicalStatus(record.getStatus());
    if (status.isEmpty()) {
        lastError = "Choose Pending, Approve, or Reject as the booking status.";
        return false;
    }
    if (!beginWrite())
        return false;

    QSqlQuery query(db);

    // First make sure the booking is still in the database.
    query.prepare("SELECT id FROM records WHERE id = :id");
    query.bindValue(":id", record.getId());
    if (!query.exec()) {
        lastError = query.lastError().text();
        db.rollback();
        return false;
    }
    const bool found = query.next();
    query.finish();
    if (!found) {
        lastError = "The booking no longer exists. Refresh the booking list.";
        db.rollback();
        return false;
    }

    // The booking being edited must not block its own time slots,
    // so its id is passed in and skipped.
    if (status != "Reject" &&
        !isTimeSlotAvailable(record.getRecordDate(), record.getCategory(),
                             splitSlots(record.getTimeSlots()), record.getId())) {
        db.rollback();
        return false;
    }

    // UPDATE the existing row.
    query.prepare("UPDATE records SET workshop_id=:workshop, category=:category, name=:name, "
                  "index_no=:index, department=:dept, email=:email, "
                  "supervisor_name=:supervisor, project_name=:project, record_date=:date, "
                  "time_slot=:time, description=:desc, status=:status WHERE id=:id");
    query.bindValue(":workshop", record.getWorkshopId());
    query.bindValue(":category", record.getCategory());
    query.bindValue(":name", record.getName().trimmed());
    query.bindValue(":index", record.getIndexNo().trimmed());
    query.bindValue(":dept", record.getDepartment().trimmed());
    query.bindValue(":email", record.getEmail().trimmed());
    query.bindValue(":supervisor", record.getSupervisorName().trimmed());
    query.bindValue(":project", record.getProjectName().trimmed());
    query.bindValue(":date", record.getRecordDate());
    query.bindValue(":time", splitSlots(record.getTimeSlots()).join(", "));
    query.bindValue(":desc", record.getDescription());
    query.bindValue(":status", status);
    query.bindValue(":id", record.getId());
    if (!query.exec()) {
        lastError = query.lastError().text();
        db.rollback();
        return false;
    }
    return commitWrite();
}

// Delete one booking by its id.
bool SqliteDatabaseManager::deleteRecord(int id)
{
    if (!ensureOpen())
        return false;

    QSqlQuery query(db);
    query.prepare("DELETE FROM records WHERE id = :id");
    query.bindValue(":id", id);
    if (!query.exec()) {
        lastError = query.lastError().text();
        return false;
    }
    if (query.numRowsAffected() != 1) {
        lastError = "The booking no longer exists. Refresh the booking list.";
        return false;
    }
    return true;
}

// Change only the status of a booking (Pending, Approve or Reject).
bool SqliteDatabaseManager::updateRecordStatus(int id, const QString &value)
{
    if (!ensureOpen())
        return false;

    const QString status = canonicalStatus(value);
    if (status.isEmpty()) {
        lastError = "Choose Pending, Approve, or Reject as the booking status.";
        return false;
    }
    if (!beginWrite())
        return false;

    QSqlQuery query(db);

    // Read the date, workshop and slots of this booking so they can be re-checked.
    query.prepare("SELECT record_date, category, time_slot FROM records WHERE id = :id");
    query.bindValue(":id", id);
    if (!query.exec()) {
        lastError = query.lastError().text();
        db.rollback();
        return false;
    }
    if (!query.next()) {
        lastError = "The booking no longer exists. Refresh the booking list.";
        query.finish();
        db.rollback();
        return false;
    }
    const QString date = query.value(0).toString();
    const QString category = query.value(1).toString();
    const QStringList slotList = splitSlots(query.value(2).toString());
    query.finish();

    // Approving a booking gives it its slots back, so they must still be free.
    if (status != "Reject" && !isTimeSlotAvailable(date, category, slotList, id)) {
        db.rollback();
        return false;
    }

    query.prepare("UPDATE records SET status = :status WHERE id = :id");
    query.bindValue(":status", status);
    query.bindValue(":id", id);
    if (!query.exec()) {
        lastError = query.lastError().text();
        db.rollback();
        return false;
    }
    return commitWrite();
}

// Change only the administrator's remark for a booking.
bool SqliteDatabaseManager::updateAdminRemarks(int id, const QString &remarks)
{
    if (!ensureOpen())
        return false;

    QSqlQuery query(db);
    query.prepare("UPDATE records SET admin_remarks = :remarks WHERE id = :id");
    query.bindValue(":remarks", remarks);
    query.bindValue(":id", id);
    if (!query.exec()) {
        lastError = query.lastError().text();
        return false;
    }
    if (query.numRowsAffected() != 1) {
        lastError = "The booking no longer exists. Refresh the booking list.";
        return false;
    }
    return true;
}

// ---------------------------------------------------------------------------
// Reading: SELECT
// ---------------------------------------------------------------------------

// All bookings for one workshop on one date, newest first.
QList<StudentRecord> SqliteDatabaseManager::getRecordsByDateAndCategory(const QString &date,
                                                                        const QString &category)
{
    QList<StudentRecord> records;
    if (!ensureOpen())
        return records;

    QSqlQuery query(db);
    query.prepare("SELECT " + recordColumns +
                  " FROM records WHERE record_date = :date AND category = :category "
                  "ORDER BY id DESC");
    query.bindValue(":date", date);
    query.bindValue(":category", category);
    if (!query.exec()) {
        lastError = query.lastError().text();
        return records;
    }
    while (query.next())
        records.append(readRecord(query));
    return records;
}

// The dates on which a workshop already has bookings that are not rejected.
QList<QDate> SqliteDatabaseManager::getBookedDates(const QString &category)
{
    QList<QDate> dates;
    if (!ensureOpen())
        return dates;

    QSqlQuery query(db);
    query.prepare("SELECT DISTINCT record_date FROM records WHERE category = :category "
                  "AND lower(trim(COALESCE(status, 'Pending'))) NOT IN ('reject', 'rejected') "
                  "ORDER BY record_date");
    query.bindValue(":category", category);
    if (!query.exec()) {
        lastError = query.lastError().text();
        return dates;
    }
    while (query.next()) {
        const QDate date = QDate::fromString(query.value(0).toString(), Qt::ISODate);
        if (date.isValid())
            dates.append(date);
    }
    return dates;
}

// True when none of the requested slots is already taken.
bool SqliteDatabaseManager::isTimeSlotAvailable(const QString &date, const QString &category,
                                                const QStringList &requestedSlots,
                                                int excludeRecordId)
{
    if (!ensureOpen())
        return false;

    const QStringList slotList = splitSlots(requestedSlots.join(','));
    if (!validDate(date) || category.trimmed().isEmpty() || slotList.isEmpty()) {
        lastError = "Choose a workshop, a valid booking date, and at least one time slot.";
        return false;
    }

    const QStringList bookedSlots = getBookedTimeSlotsStringList(date, category, excludeRecordId);
    if (!lastError.isEmpty())
        return false;

    for (const QString &slot : slotList) {
        if (bookedSlots.contains(slot)) {
            lastError = "The selected time slot is already booked. Choose another time slot.";
            return false;
        }
    }
    return true;
}

// Every slot already booked for one workshop on one date.
QStringList SqliteDatabaseManager::getBookedTimeSlotsStringList(const QString &date,
                                                                const QString &category,
                                                                int excludeRecordId)
{
    QStringList slotList;
    if (!ensureOpen())
        return slotList;

    QSqlQuery query(db);
    query.prepare("SELECT time_slot FROM records WHERE record_date = :date AND category = :category "
                  "AND id != :id "
                  "AND lower(trim(COALESCE(status, 'Pending'))) NOT IN ('reject', 'rejected')");
    query.bindValue(":date", date);
    query.bindValue(":category", category);
    query.bindValue(":id", excludeRecordId);
    if (!query.exec()) {
        lastError = query.lastError().text();
        return slotList;
    }
    while (query.next()) {
        for (const QString &slot : splitSlots(query.value(0).toString())) {
            if (!slotList.contains(slot))
                slotList.append(slot);
        }
    }
    return slotList;
}

// Every booking of one workshop, most recent date first.
QList<StudentRecord> SqliteDatabaseManager::getAllRecordsForCategory(const QString &category)
{
    QList<StudentRecord> records;
    if (!ensureOpen())
        return records;

    QSqlQuery query(db);
    query.prepare("SELECT " + recordColumns +
                  " FROM records WHERE category = :category ORDER BY record_date DESC, id DESC");
    query.bindValue(":category", category);
    if (!query.exec()) {
        lastError = query.lastError().text();
        return records;
    }
    while (query.next())
        records.append(readRecord(query));
    return records;
}

// The reason the last database operation failed.
QString SqliteDatabaseManager::getLastError() const
{
    return lastError;
}
