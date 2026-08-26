#include "databasemanager.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>

SqliteDatabaseManager::SqliteDatabaseManager(const QString &dbName)
{
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(dbName);
    db.open();
}

SqliteDatabaseManager::~SqliteDatabaseManager()
{
    if (db.isOpen()) {
        db.close();
    }
}

bool SqliteDatabaseManager::createTable()
{
    QSqlQuery query;
    return query.exec("CREATE TABLE IF NOT EXISTS records ("
                      "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                      "category TEXT, name TEXT, index_no TEXT, "
                      "department TEXT, record_date TEXT, "
                      "time_slot TEXT, description TEXT, passcode TEXT)");
}

bool SqliteDatabaseManager::addRecord(const StudentRecord &record)
{
    QSqlQuery query;
    query.prepare("INSERT INTO records (category, name, index_no, department, record_date, time_slot, description, passcode) "
                  "VALUES (:category, :name, :index, :dept, :date, :time, :desc, :passcode)");
    query.bindValue(":category", record.getCategory());
    query.bindValue(":name", record.getName());
    query.bindValue(":index", record.getIndexNo());
    query.bindValue(":dept", record.getDepartment());
    query.bindValue(":date", record.getRecordDate());
    query.bindValue(":time", record.getTimeSlots());
    query.bindValue(":desc", record.getDescription());
    query.bindValue(":passcode", record.getPasscode());
    return query.exec();
}

bool SqliteDatabaseManager::updateRecord(const StudentRecord &record)
{
    QSqlQuery query;
    query.prepare("UPDATE records SET name=:name, index_no=:index, department=:dept, record_date=:date, time_slot=:time, description=:desc WHERE id=:id");
    query.bindValue(":name", record.getName());
    query.bindValue(":index", record.getIndexNo());
    query.bindValue(":dept", record.getDepartment());
    query.bindValue(":date", record.getRecordDate());
    query.bindValue(":time", record.getTimeSlots());
    query.bindValue(":desc", record.getDescription());
    query.bindValue(":id", record.getId());
    return query.exec();
}

bool SqliteDatabaseManager::deleteRecord(int id)
{
    QSqlQuery query;
    query.prepare("DELETE FROM records WHERE id = :id");
    query.bindValue(":id", id);
    return query.exec();
}

QList<StudentRecord> SqliteDatabaseManager::getRecordsByDateAndCategory(const QString &date, const QString &category)
{
    QList<StudentRecord> records;
    QSqlQuery query;
    query.prepare("SELECT id, name, index_no, department, time_slot, description, passcode "
                  "FROM records WHERE record_date = :date AND category = :category");
    query.bindValue(":date", date);
    query.bindValue(":category", category);
    
    if (query.exec()) {
        while (query.next()) {
            StudentRecord record;
            record.setId(query.value(0).toInt());
            record.setName(query.value(1).toString());
            record.setIndexNo(query.value(2).toString());
            record.setDepartment(query.value(3).toString());
            record.setTimeSlots(query.value(4).toString());
            record.setDescription(query.value(5).toString());
            record.setPasscode(query.value(6).toString());
            record.setRecordDate(date);
            record.setCategory(category);
            records.append(record);
        }
    }
    return records;
}

QList<QDate> SqliteDatabaseManager::getBookedDates(const QString &category)
{
    QList<QDate> dates;
    QSqlQuery query;
    query.prepare("SELECT DISTINCT record_date FROM records WHERE category = :category");
    query.bindValue(":category", category);
    
    if (query.exec()) {
        while (query.next()) {
            QDate date = QDate::fromString(query.value(0).toString(), "yyyy-MM-dd");
            if (date.isValid()) dates.append(date);
        }
    }
    return dates;
}

// Logic to check if the requested time slots overlap with existing bookings
bool SqliteDatabaseManager::isTimeSlotAvailable(const QString &date, const QString &category, const QStringList &requestedSlots, int excludeRecordId)
{
    QSqlQuery query;
    // Get all time slots booked on this date for this category (excluding the current record if we are updating)
    query.prepare("SELECT time_slot FROM records WHERE record_date = :date AND category = :category AND id != :id");
    query.bindValue(":date", date);
    query.bindValue(":category", category);
    query.bindValue(":id", excludeRecordId);
    
    if (query.exec()) {
        while (query.next()) {
            QString bookedStr = query.value(0).toString();
            // Check against requested slots
            for (const QString &reqSlot : requestedSlots) {
                if (bookedStr.contains(reqSlot)) {
                    return false; // Slot is already booked
                }
            }
        }
    }
    return true;
}

QString SqliteDatabaseManager::getLastError() const
{
    return db.lastError().text();
}