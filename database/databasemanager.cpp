#include "databasemanager.h"
#include "databasehelpers.h"

#include <QRegularExpression>
#include <QSet>
#include <QSqlError>
#include <QSqlQuery>
#include <QUuid>
#include <QVariant>

// Shared rules keep reads and writes consistent.
namespace DatabaseHelpers
{
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
    if (status == "pending")
        return QStringLiteral("Pending");
    if (status == "approve" || status == "approved")
        return QStringLiteral("Approve");
    if (status == "reject" || status == "rejected")
        return QStringLiteral("Reject");
    return {};
}

bool validDate(const QString &value)
{
    const QDate date = QDate::fromString(value, Qt::ISODate);
    return date.isValid() && date.toString(Qt::ISODate) == value;
}

} // namespace DatabaseHelpers

using namespace DatabaseHelpers;

// Open/close the connection, create the schema, and validate booking data.
SqliteDatabaseManager::SqliteDatabaseManager(const QString &dbName)
    : connectionName(QStringLiteral("workshop-") +
                     QUuid::createUuid().toString(QUuid::WithoutBraces))
{
    db = QSqlDatabase::addDatabase(QStringLiteral("QSQLITE"), connectionName);
    db.setConnectOptions(QStringLiteral("QSQLITE_BUSY_TIMEOUT=5000"));
    db.setDatabaseName(dbName);
    if (!db.open())
        lastError = db.lastError().text();
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
    if (db.isOpen())
        return true;
    lastError = db.lastError().text();
    if (lastError.isEmpty())
        lastError = QStringLiteral("The booking database is not open.");
    return false;
}

bool SqliteDatabaseManager::beginWrite()
{
    if (!ensureOpen())
        return false;
    // Reserve the SQLite writer before checking availability so another process
    // cannot insert a booking between the check and the write.
    QSqlQuery query(db);
    if (query.exec(QStringLiteral("BEGIN IMMEDIATE")))
        return true;
    lastError = query.lastError().text();
    return false;
}

bool SqliteDatabaseManager::commitWrite()
{
    if (db.commit())
        return true;
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
    if (!beginWrite())
        return false;
    QSqlQuery query(db);
    if (!query.exec(QStringLiteral(
            "CREATE TABLE IF NOT EXISTS records ("
            "id INTEGER PRIMARY KEY AUTOINCREMENT, "
            "booking_id TEXT, workshop_id TEXT, category TEXT, name TEXT, index_no TEXT, "
            "department TEXT, email TEXT, pdf_path TEXT, record_date TEXT, time_slot TEXT, "
            "description TEXT, passcode TEXT, created_at TEXT, status TEXT DEFAULT 'Pending', "
            "admin_remarks TEXT)"))) {
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
    while (query.next())
        columns.insert(query.value(1).toString().toLower());
    query.finish();
    if (!columns.contains(QStringLiteral("id"))) {
        lastError = QStringLiteral(
            "The records table has no ID column. Restore a compatible booking database.");
        rollbackWrite();
        return false;
    }
    const QStringList textColumns = {"booking_id",  "workshop_id", "category",     "name",
                                     "index_no",    "department",  "email",        "pdf_path",
                                     "record_date", "time_slot",   "description",  "passcode",
                                     "created_at",  "status",      "admin_remarks"};
    for (const QString &column : textColumns) {
        if (columns.contains(column))
            continue;
        const QString defaultValue = column == "status" ? "'Pending'" : "''";
        if (!query.exec(QStringLiteral("ALTER TABLE records ADD COLUMN %1 TEXT DEFAULT %2")
                            .arg(column, defaultValue))) {
            lastError = query.lastError().text();
            rollbackWrite();
            return false;
        }
    }
    const QStringList migrations = {
        "UPDATE records SET status = 'Pending' WHERE status IS NULL OR trim(status) = '' OR "
        "lower(trim(status)) = 'pending'",
        "UPDATE records SET status = 'Approve' WHERE lower(trim(status)) IN ('approve', "
        "'approved')",
        "UPDATE records SET status = 'Reject' WHERE lower(trim(status)) IN ('reject', 'rejected')",
        "CREATE INDEX IF NOT EXISTS records_category_date ON records(category, record_date)"};
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
    if (record.getCategory().trimmed().isEmpty() || !validDate(record.getRecordDate()) ||
        splitSlots(record.getTimeSlots()).isEmpty()) {
        lastError =
            QStringLiteral("Choose a workshop, a valid booking date, and at least one time slot.");
        return false;
    }
    if (record.getName().trimmed().isEmpty() || record.getIndexNo().trimmed().isEmpty() ||
        record.getDepartment().trimmed().isEmpty()) {
        lastError = QStringLiteral("Name, index number, and department are required.");
        return false;
    }
    static const QRegularExpression emailPattern(
        QStringLiteral("^[A-Za-z0-9._%+-]+@[A-Za-z0-9.-]+\\.[A-Za-z]{2,}$"));
    if (!emailPattern.match(record.getEmail().trimmed()).hasMatch()) {
        lastError = QStringLiteral("Enter a valid email address.");
        return false;
    }
    return true;
}
