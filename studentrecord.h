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
    QString email;       // අලුතින් එක් කරන ලදී
    QString pdfPath;     // අලුතින් එක් කරන ලදී
    QString recordDate;
    QString timeSlots;
    QString description;
    QString passcode;
    QString createdAt;

public:
    StudentRecord();

    int getId() const;
    QString getCategory() const;
    QString getName() const;
    QString getIndexNo() const;
    QString getDepartment() const;
    QString getEmail() const;
    QString getPdfPath() const;
    QString getRecordDate() const;
    QString getTimeSlots() const;
    QString getDescription() const;
    QString getPasscode() const;
    QString getCreatedAt() const;

    void setId(int newId);
    void setCategory(const QString &newCategory);
    void setName(const QString &newName);
    void setIndexNo(const QString &newIndexNo);
    void setDepartment(const QString &newDepartment);
    void setEmail(const QString &newEmail);
    void setPdfPath(const QString &newPdfPath);
    void setRecordDate(const QString &newRecordDate);
    void setTimeSlots(const QString &newTimeSlots);
    void setDescription(const QString &newDescription);
    void setPasscode(const QString &newPasscode);
    void setCreatedAt(const QString &newCreatedAt);
};

#endif // STUDENTRECORD_H