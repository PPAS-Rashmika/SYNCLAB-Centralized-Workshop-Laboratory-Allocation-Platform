#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QSqlDatabase>
#include "idatabasemanager.h"

// Inheritance: this class inherits the IDatabaseManager interface and provides
// the real SQLite implementation of every function the interface declares.
// Encapsulation: the connection, its name and the last error are all private,
// so no other class can change them by accident.
class SqliteDatabaseManager : public IDatabaseManager
{
  public:
    // Constructor: opens the SQLite database file.
    explicit SqliteDatabaseManager(const QString &dbName);
    // Destructor: closes the connection.
    ~SqliteDatabaseManager() override;

    // The functions below override the pure virtual functions of IDatabaseManager.

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

  private:
    QSqlDatabase db;        // the open SQLite connection
    QString connectionName; // the unique name Qt requires for each connection
    QString lastError;      // the message shown to the user when something fails

    bool ensureOpen();                                 // check the connection is usable
    bool beginWrite();                                 // start a transaction
    bool commitWrite();                                // save the transaction
    bool validateRecord(const StudentRecord &record);  // check one booking before saving
};

#endif // DATABASEMANAGER_H
