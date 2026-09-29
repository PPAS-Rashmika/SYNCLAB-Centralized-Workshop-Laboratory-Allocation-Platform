#include "databasemanager.h"
#include "databasehelpers.h"

#include <QSqlError>
#include <QSqlQuery>
#include <QVariant>

using namespace DatabaseHelpers;

// Prepared queries bind values separately from SQL text.
// A transaction makes a conflict check and its write one complete operation.
// On failure, rollback leaves the saved bookings unchanged.
bool SqliteDatabaseManager::addRecord(const StudentRecord &record)
{
    if (!ensureOpen() || !validateRecord(record))
        return false;
    const QString status = canonicalStatus(record.getStatus());
    if (record.getBookingId().trimmed().isEmpty() || status.isEmpty()) {
        lastError = QStringLiteral("A booking ID and a valid booking status are required.");
        return false;
    }
    if (!beginWrite())
        return false;
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
        lastError =
            QStringLiteral("This booking ID already exists. Please submit the booking again.");
        rollbackWrite();
        return false;
    }
    if (status != "Reject" && !isTimeSlotAvailable(record.getRecordDate(), record.getCategory(),
                                                   splitSlots(record.getTimeSlots()))) {
        rollbackWrite();
        return false;
    }
    query.prepare(
        QStringLiteral("INSERT INTO records (booking_id, workshop_id, category, name, index_no, "
                       "department, email, pdf_path, record_date, time_slot, description, "
                       "passcode, created_at, status, admin_remarks) "
                       "VALUES (:b_id, :w_id, :category, :name, :index, :dept, :email, :pdf, "
                       ":date, :time, :desc, :passcode, :created_at, :status, :remarks)"));
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
    if (!ensureOpen() || !validateRecord(record))
        return false;
    const QString status = canonicalStatus(record.getStatus());
    if (status.isEmpty()) {
        lastError = QStringLiteral("Choose Pending, Approve, or Reject as the booking status.");
        return false;
    }
    if (!beginWrite())
        return false;
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
    if (status != "Reject" &&
        !isTimeSlotAvailable(record.getRecordDate(), record.getCategory(),
                             splitSlots(record.getTimeSlots()), record.getId())) {
        rollbackWrite();
        return false;
    }
    query.prepare(QStringLiteral(
        "UPDATE records SET workshop_id=:workshop, category=:category, name=:name, "
        "index_no=:index, department=:dept, email=:email, pdf_path=:pdf, record_date=:date, "
        "time_slot=:time, description=:desc, status=:status WHERE id=:id"));
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
    if (!ensureOpen())
        return false;
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
    if (!ensureOpen())
        return false;
    const QString status = canonicalStatus(value);
    if (status.isEmpty()) {
        lastError = QStringLiteral("Choose Pending, Approve, or Reject as the booking status.");
        return false;
    }
    if (!beginWrite())
        return false;
    QSqlQuery query(db);
    query.prepare(
        QStringLiteral("SELECT record_date, category, time_slot FROM records WHERE id = :id"));
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
    if (!ensureOpen())
        return false;
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
