#ifndef STUDENTRECORD_H
#define STUDENTRECORD_H

#include <QString>

class StudentRecord
{
private:
    int id;
    QString category;
    QString name;
    QString indexNo;
    QString department;
    QString recordDate;
    QString timeSlots;
    QString description;
    QString passcode;

public:
    StudentRecord();

    // Getters
    int getId() const;
    QString getCategory() const;
    QString getName() const;
    QString getIndexNo() const;
    QString getDepartment() const;
    QString getRecordDate() const;
    QString getTimeSlots() const;
    QString getDescription() const;
    QString getPasscode() const;

    // Setters
    void setId(int newId);
    void setCategory(const QString &newCategory);
    void setName(const QString &newName);
    void setIndexNo(const QString &newIndexNo);
    void setDepartment(const QString &newDepartment);
    void setRecordDate(const QString &newRecordDate);
    void setTimeSlots(const QString &newTimeSlots);
    void setDescription(const QString &newDescription);
    void setPasscode(const QString &newPasscode);
};

#endif // STUDENTRECORD_H