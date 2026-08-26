#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QSqlDatabase>
#include <QList>
#include <QDate>
#include "studentrecord.h"

class DatabaseManager
{
private:
    QSqlDatabase db;

public:
    DatabaseManager(const QString &dbName);
    ~DatabaseManager();

    bool createTable();
    bool addRecord(const StudentRecord &record);
    bool updateRecord(const StudentRecord &record);
    bool deleteRecord(int id);
    
    // Returns a record if found, otherwise returns a record with id = -1
    StudentRecord getRecordByDateAndCategory(const QString &date, const QString &category);
    
    // Returns a list of dates that have bookings for a specific category
    QList<QDate> getBookedDates(const QString &category);
    
    QString getLastError() const;
};

#endif // DATABASEMANAGER_H