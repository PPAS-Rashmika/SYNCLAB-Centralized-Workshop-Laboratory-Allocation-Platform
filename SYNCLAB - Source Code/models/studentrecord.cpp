#include "studentrecord.h"

// New bookings have no database row ID yet and start as Pending.
StudentRecord::StudentRecord() : id(-1), status("Pending"), adminRemarks("") {}

int StudentRecord::getId() const
{
    return id;
}
QString StudentRecord::getWorkshopId() const
{
    return workshopId;
}
QString StudentRecord::getCategory() const
{
    return category;
}
QString StudentRecord::getName() const
{
    return name;
}
QString StudentRecord::getIndexNo() const
{
    return indexNo;
}
QString StudentRecord::getDepartment() const
{
    return department;
}
QString StudentRecord::getEmail() const
{
    return email;
}
QString StudentRecord::getPdfPath() const
{
    return pdfPath;
}
QString StudentRecord::getRecordDate() const
{
    return recordDate;
}
QString StudentRecord::getTimeSlots() const
{
    return timeSlots;
}
QString StudentRecord::getDescription() const
{
    return description;
}
QString StudentRecord::getPasscode() const
{
    return passcode;
}
QString StudentRecord::getCreatedAt() const
{
    return createdAt;
}
QString StudentRecord::getStatus() const
{
    return status;
}
QString StudentRecord::getAdminRemarks() const
{
    return adminRemarks;
}

void StudentRecord::setId(int newId)
{
    id = newId;
}
void StudentRecord::setWorkshopId(const QString &newWorkshopId)
{
    workshopId = newWorkshopId;
}
void StudentRecord::setCategory(const QString &newCategory)
{
    category = newCategory;
}
void StudentRecord::setName(const QString &newName)
{
    name = newName;
}
void StudentRecord::setIndexNo(const QString &newIndexNo)
{
    indexNo = newIndexNo;
}
void StudentRecord::setDepartment(const QString &newDepartment)
{
    department = newDepartment;
}
void StudentRecord::setEmail(const QString &newEmail)
{
    email = newEmail;
}
void StudentRecord::setPdfPath(const QString &newPdfPath)
{
    pdfPath = newPdfPath;
}
void StudentRecord::setRecordDate(const QString &newRecordDate)
{
    recordDate = newRecordDate;
}
void StudentRecord::setTimeSlots(const QString &newTimeSlots)
{
    timeSlots = newTimeSlots;
}
void StudentRecord::setDescription(const QString &newDescription)
{
    description = newDescription;
}
void StudentRecord::setPasscode(const QString &newPasscode)
{
    passcode = newPasscode;
}
void StudentRecord::setCreatedAt(const QString &newCreatedAt)
{
    createdAt = newCreatedAt;
}
void StudentRecord::setStatus(const QString &newStatus)
{
    status = newStatus;
}
void StudentRecord::setAdminRemarks(const QString &newRemarks)
{
    adminRemarks = newRemarks;
}