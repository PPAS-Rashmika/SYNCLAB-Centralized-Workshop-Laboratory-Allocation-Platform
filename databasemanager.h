#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QSqlDatabase>
#include <QList>
#include <QDate>
#include <QString>
#include <QStringList>
#include "studentrecord.h"

class IDatabaseManager 
{
public:
    virtual ~IDatabaseManager() {} 
    
    virtual bool createTable() = 0;
    virtual bool addRecord(const StudentRecord &record) = 0;
    virtual bool updateRecord(const StudentRecord &record) = 0;
    virtual bool deleteRecord(int id) = 0;
    virtual bool updateRecordStatus(int id, const QString &status) = 0; 
    virtual bool updateAdminRemarks(int id, const QString &remarks) = 0; 
    
    virtual QList<StudentRecord> getRecordsByDateAndCategory(const QString &date, const QString &category) = 0;
    virtual QList<QDate> getBookedDates(const QString &category) = 0;
    virtual bool isTimeSlotAvailable(const QString &date, const QString &category, const QStringList &requestedSlots, int excludeRecordId = -1) = 0;
    virtual QStringList getBookedTimeSlotsStringList(const QString &date, const QString &category, int excludeRecordId = -1) = 0;
    virtual QList<StudentRecord> getAllRecordsForCategory(const QString &category) = 0;
    virtual QString getLastError() const = 0;
};

class SqliteDatabaseManager : public IDatabaseManager
{
private:
    QSqlDatabase db;
    QString connectionName;
    QString lastError;

    bool ensureOpen();
    bool beginWrite();
    bool commitWrite();
    void rollbackWrite();
    bool validateRecord(const StudentRecord &record);

    Q_DISABLE_COPY(SqliteDatabaseManager)

public:
    SqliteDatabaseManager(const QString &dbName);
    ~SqliteDatabaseManager() override;

    bool createTable() override;
    bool addRecord(const StudentRecord &record) override;
    bool updateRecord(const StudentRecord &record) override;
    bool deleteRecord(int id) override;
    bool updateRecordStatus(int id, const QString &status) override;
    bool updateAdminRemarks(int id, const QString &remarks) override;
    
    QList<StudentRecord> getRecordsByDateAndCategory(const QString &date, const QString &category) override;
    QList<QDate> getBookedDates(const QString &category) override;
    bool isTimeSlotAvailable(const QString &date, const QString &category, const QStringList &requestedSlots, int excludeRecordId = -1) override;
    QStringList getBookedTimeSlotsStringList(const QString &date, const QString &category, int excludeRecordId = -1) override;
    QList<StudentRecord> getAllRecordsForCategory(const QString &category) override;
    QString getLastError() const override;
};

#endif // DATABASEMANAGER_H
