#ifndef IDATABASEMANAGER_H
#define IDATABASEMANAGER_H

#include <QList>
#include <QDate>
#include <QString>
#include <QStringList>
#include "studentrecord.h"

// This is the base class (an interface) for every database the program can use.
// Each function marked '= 0' is a pure virtual function: the base class only
// declares what must be done, and the derived class decides how to do it.
// The UI keeps a pointer of this type, so calling one of these functions runs
// the version in the derived class. That is runtime polymorphism.
class IDatabaseManager
{
  public:
    // A virtual destructor so deleting through a base pointer is safe.
    virtual ~IDatabaseManager() {}

    virtual bool createTable() = 0;
    virtual bool addRecord(const StudentRecord &record) = 0;
    virtual bool updateRecord(const StudentRecord &record) = 0;
    virtual bool deleteRecord(int id) = 0;
    virtual bool updateRecordStatus(int id, const QString &status) = 0;
    virtual bool updateAdminRemarks(int id, const QString &remarks) = 0;

    virtual QList<StudentRecord> getRecordsByDateAndCategory(const QString &date,
                                                             const QString &category) = 0;
    virtual QList<QDate> getAllocatedDates(const QString &category) = 0;
    virtual bool isTimeSlotAvailable(const QString &date, const QString &category,
                                     const QStringList &requestedSlots,
                                     int excludeRecordId = -1) = 0;
    virtual QStringList getAllocatedTimeSlotsStringList(const QString &date, const QString &category,
                                                     int excludeRecordId = -1) = 0;
    virtual QList<StudentRecord> getAllRecordsForCategory(const QString &category) = 0;
    virtual QString getLastError() const = 0;
};

#endif // IDATABASEMANAGER_H
