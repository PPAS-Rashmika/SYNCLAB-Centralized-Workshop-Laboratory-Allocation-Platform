#ifndef STUDENTRECORD_H
#define STUDENTRECORD_H

#include <QString>

// StudentRecord is the data class for one allocation.
//
// ENCAPSULATION: every piece of data below is declared 'private', so no other
// class can read or change it directly. The only way in is through the small
// public get... and set... functions. That means the data can never be
// modified by accident from somewhere else in the program.
//
// The whole class is written in this one header on purpose: every function body
// is a single line, so keeping the declaration and the body together makes the
// class easy to read without jumping between two files.
class StudentRecord
{
  public:
    // Constructor.
    // A brand new allocation has no database row id yet (-1) and starts as
    // "Pending" until an administrator approves or rejects it.
    StudentRecord() : id(-1), status("Pending") {}

    // ---- Getters: read one piece of data ----
    int getId() const { return id; }
    QString getWorkshopId() const { return workshopId; }
    QString getCategory() const { return category; }
    QString getName() const { return name; }
    QString getIndexNo() const { return indexNo; }
    QString getDepartment() const { return department; }
    QString getEmail() const { return email; }
    QString getSupervisorName() const { return supervisorName; }
    QString getProjectName() const { return projectName; }
    QString getRecordDate() const { return recordDate; }
    QString getTimeSlots() const { return timeSlots; }
    QString getDescription() const { return description; }
    QString getPasscode() const { return passcode; }
    QString getCreatedAt() const { return createdAt; }
    QString getStatus() const { return status; }
    QString getAdminRemarks() const { return adminRemarks; }

    // ---- Setters: change one piece of data ----
    void setId(int newId) { id = newId; }
    void setWorkshopId(const QString &newWorkshopId) { workshopId = newWorkshopId; }
    void setCategory(const QString &newCategory) { category = newCategory; }
    void setName(const QString &newName) { name = newName; }
    void setIndexNo(const QString &newIndexNo) { indexNo = newIndexNo; }
    void setDepartment(const QString &newDepartment) { department = newDepartment; }
    void setEmail(const QString &newEmail) { email = newEmail; }
    void setSupervisorName(const QString &newSupervisorName) { supervisorName = newSupervisorName; }
    void setProjectName(const QString &newProjectName) { projectName = newProjectName; }
    void setRecordDate(const QString &newRecordDate) { recordDate = newRecordDate; }
    void setTimeSlots(const QString &newTimeSlots) { timeSlots = newTimeSlots; }
    void setDescription(const QString &newDescription) { description = newDescription; }
    void setPasscode(const QString &newPasscode) { passcode = newPasscode; }
    void setCreatedAt(const QString &newCreatedAt) { createdAt = newCreatedAt; }
    void setStatus(const QString &newStatus) { status = newStatus; }
    void setAdminRemarks(const QString &newRemarks) { adminRemarks = newRemarks; }

  private:
    int id;                 // row number given by SQLite; -1 means "not saved yet"
    QString workshopId;     // the workshop, kept for older database rows
    QString category;       // the workshop, used by all new rows
    QString name;           // student's full name
    QString indexNo;        // student's index number
    QString department;
    QString email;
    QString supervisorName; // the lecturer supervising the project
    QString projectName;    // the project the allocation is for
    QString recordDate;     // the allocated date, stored as "yyyy-MM-dd"
    QString timeSlots;      // one or more slots joined with ", "
    QString description;    // what the student wants to do in the laboratory
    QString passcode;       // the 4-digit allocation key given to the student
    QString createdAt;      // when the allocation was first saved
    QString status;         // "Pending", "Approve" or "Reject"
    QString adminRemarks;   // a note typed by the administrator
};

#endif // STUDENTRECORD_H
