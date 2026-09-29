#ifndef IDATABASEMANAGER_H
#define IDATABASEMANAGER_H

#include <QList>
#include <QDate>
#include <QString>
#include <QStringList>
#include "studentrecord.h"

// Abstraction: this interface lists the database operations used by the UI.
// Each '= 0' function must be implemented by a concrete database class.
// Calling an override through this interface is runtime polymorphism.
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

    virtual QList<StudentRecord> getRecordsByDateAndCategory(const QString &date,
                                                             const QString &category) = 0;
    virtual QList<QDate> getBookedDates(const QString &category) = 0;
    virtual bool isTimeSlotAvailable(const QString &date, const QString &category,
                                     const QStringList &requestedSlots,
                                     int excludeRecordId = -1) = 0;
    virtual QStringList getBookedTimeSlotsStringList(const QString &date, const QString &category,
                                                     int excludeRecordId = -1) = 0;
    virtual QList<StudentRecord> getAllRecordsForCategory(const QString &category) = 0;
    virtual QString getLastError() const = 0;
};

#endif // IDATABASEMANAGER_H
