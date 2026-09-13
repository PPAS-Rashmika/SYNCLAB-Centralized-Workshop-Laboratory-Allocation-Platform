#ifndef STUDENTRECORD_H
#define STUDENTRECORD_H

#include <QString>

class StudentRecord
{
private:
    int id;
    QString bookingId;
    QString workshopId;
    QString category;
    QString name;
    QString indexNo;
    QString department;
    QString email;
    QString pdfPath;
    QString recordDate;
    QString timeSlots;
    QString description;
    QString passcode;
    QString createdAt;
    QString status; 
    QString adminRemarks;

public:
    StudentRecord();

    int getId() const;
    QString getBookingId() const;
    QString getWorkshopId() const;
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
    QString getStatus() const;
    QString getAdminRemarks() const;

    void setId(int newId);
    void setBookingId(const QString &newBookingId);
    void setWorkshopId(const QString &newWorkshopId);
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
    void setStatus(const QString &newStatus);
    void setAdminRemarks(const QString &newRemarks);
};

#endif // STUDENTRECORD_H