#include "studentrecord.h"

StudentRecord::StudentRecord() : id(-1) {}

// Getters
int StudentRecord::getId() const { return id; }
QString StudentRecord::getCategory() const { return category; }
QString StudentRecord::getName() const { return name; }
QString StudentRecord::getIndexNo() const { return indexNo; }
QString StudentRecord::getDepartment() const { return department; }
QString StudentRecord::getRecordDate() const { return recordDate; }
QString StudentRecord::getTimeSlots() const { return timeSlots; }
QString StudentRecord::getDescription() const { return description; }
QString StudentRecord::getPasscode() const { return passcode; }

// Setters
void StudentRecord::setId(int newId) { id = newId; }
void StudentRecord::setCategory(const QString &newCategory) { category = newCategory; }
void StudentRecord::setName(const QString &newName) { name = newName; }
void StudentRecord::setIndexNo(const QString &newIndexNo) { indexNo = newIndexNo; }
void StudentRecord::setDepartment(const QString &newDepartment) { department = newDepartment; }
void StudentRecord::setRecordDate(const QString &newRecordDate) { recordDate = newRecordDate; }
void StudentRecord::setTimeSlots(const QString &newTimeSlots) { timeSlots = newTimeSlots; }
void StudentRecord::setDescription(const QString &newDescription) { description = newDescription; }
void StudentRecord::setPasscode(const QString &newPasscode) { passcode = newPasscode; }