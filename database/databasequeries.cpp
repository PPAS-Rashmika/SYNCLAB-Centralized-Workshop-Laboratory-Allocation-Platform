#include "databasemanager.h"
#include "databasehelpers.h"

#include <QSqlError>
#include <QSqlQuery>
#include <QVariant>

using namespace DatabaseHelpers;

namespace
{
// Column order matches readRecord() below.
const QString recordColumns = QStringLiteral(
    "id, workshop_id, name, index_no, department, email, pdf_path, "
    "record_date, time_slot, description, passcode, created_at, status, admin_remarks, category");

// Convert one SQL row into the C++ booking model.
StudentRecord readRecord(const QSqlQuery &query)
{
    StudentRecord record;
    record.setId(query.value(0).toInt());
    record.setWorkshopId(query.value(1).toString());
    record.setName(query.value(2).toString());
    record.setIndexNo(query.value(3).toString());
    record.setDepartment(query.value(4).toString());
    record.setEmail(query.value(5).toString());
    record.setPdfPath(query.value(6).toString());
    record.setRecordDate(query.value(7).toString());
    record.setTimeSlots(query.value(8).toString());
    record.setDescription(query.value(9).toString());
    record.setPasscode(query.value(10).toString());
    record.setCreatedAt(query.value(11).toString());
    record.setStatus(query.value(12).toString());
    record.setAdminRemarks(query.value(13).toString());
    record.setCategory(query.value(14).toString());
    return record;
}
} // namespace

// Read bookings and check which time slots are still available.
QList<StudentRecord> SqliteDatabaseManager::getRecordsByDateAndCategory(const QString &date,
                                                                        const QString &category)
{
    QList<StudentRecord> records;
    if (!ensureOpen())
        return records;
    QSqlQuery query(db);
    query.prepare(
        "SELECT " + recordColumns +
        " FROM records WHERE record_date = :date AND category = :category ORDER BY id DESC");
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

QList<QDate> SqliteDatabaseManager::getBookedDates(const QString &category)
{
    QList<QDate> dates;
    if (!ensureOpen())
        return dates;
    QSqlQuery query(db);
    query.prepare(
        QStringLiteral("SELECT DISTINCT record_date FROM records WHERE category = :category "
                       "AND lower(trim(COALESCE(status, 'Pending'))) NOT IN ('reject', 'rejected') "
                       "ORDER BY record_date"));
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

bool SqliteDatabaseManager::isTimeSlotAvailable(const QString &date, const QString &category,
                                                const QStringList &requestedSlots,
                                                int excludeRecordId)
{
    if (!ensureOpen())
        return false;
    const QStringList slotList = splitSlots(requestedSlots.join(','));
    if (!validDate(date) || category.trimmed().isEmpty() || slotList.isEmpty()) {
        lastError =
            QStringLiteral("Choose a workshop, a valid booking date, and at least one time slot.");
        return false;
    }
    const QStringList bookedSlots = getBookedTimeSlotsStringList(date, category, excludeRecordId);
    if (!lastError.isEmpty())
        return false;
    for (const QString &slot : slotList) {
        if (bookedSlots.contains(slot)) {
            lastError = QStringLiteral(
                "The selected time slot is already booked. Choose another time slot.");
            return false;
        }
    }
    return true;
}

QStringList SqliteDatabaseManager::getBookedTimeSlotsStringList(const QString &date,
                                                                const QString &category,
                                                                int excludeRecordId)
{
    QStringList slotList;
    if (!ensureOpen())
        return slotList;
    QSqlQuery query(db);
    query.prepare(QStringLiteral(
        "SELECT time_slot FROM records WHERE record_date = :date AND category = :category AND id "
        "!= :id "
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
            if (!slotList.contains(slot))
                slotList.append(slot);
        }
    }
    return slotList;
}

QString SqliteDatabaseManager::getLastError() const
{
    return lastError;
}
