#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QSqlDatabase>
#include "idatabasemanager.h"

// Inheritance: this class implements IDatabaseManager using SQLite.
// Encapsulation: only this class can use its private connection and error.
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

    QList<StudentRecord> getRecordsByDateAndCategory(const QString &date,
                                                     const QString &category) override;
    QList<QDate> getBookedDates(const QString &category) override;
    bool isTimeSlotAvailable(const QString &date, const QString &category,
                             const QStringList &requestedSlots, int excludeRecordId = -1) override;
    QStringList getBookedTimeSlotsStringList(const QString &date, const QString &category,
                                             int excludeRecordId = -1) override;
    QList<StudentRecord> getAllRecordsForCategory(const QString &category) override;
    QString getLastError() const override;
};

#endif // DATABASEMANAGER_H
