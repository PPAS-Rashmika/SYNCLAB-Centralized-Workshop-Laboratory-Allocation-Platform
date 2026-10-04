// එක් booking එකක දත්ත තබයි. get... දත්ත කියවයි; set... දත්ත වෙනස් කරයි.
// එකම header එක කිහිප වරක් include වුවත් definitions එකවරක් පමණක් යෙදීමට මේ guard එක ඇත.
#ifndef STUDENTRECORD_H
#define STUDENTRECORD_H

// #include වලින් මේ file එකට අවශ්‍ය Qt සහ project declarations ලබාගනී.
#include <QString>

// Booking දත්ත private තබා public getters/setters හරහා භාවිත කිරීමට දෙයි.
class StudentRecord
{
  // අනෙක් code වලින් භාවිත කළ හැකි functions.
  public:
    // අලුත් booking එකට තවම ID එකක් නැති බව -1 මගින් දක්වයි; මුල් status එක Pending වේ.
    StudentRecord() : id(-1), status("Pending") {}

    // database row ID එක ලබාදෙයි.
    int getId() const { return id; }
    // workshop category එකේ අනෙක් නම ලබාදෙයි.
    QString getWorkshopId() const { return workshopId; }
    // workshop category එක ලබාදෙයි.
    QString getCategory() const { return category; }
    // student නම ලබාදෙයි.
    QString getName() const { return name; }
    // student index එක ලබාදෙයි.
    QString getIndexNo() const { return indexNo; }
    // student department එක ලබාදෙයි.
    QString getDepartment() const { return department; }
    // student email එක ලබාදෙයි.
    QString getEmail() const { return email; }
    // supervisor නම ලබාදෙයි.
    QString getSupervisorName() const { return supervisorName; }
    // project නම ලබාදෙයි.
    QString getProjectName() const { return projectName; }
    // booking දිනය ලබාදෙයි.
    QString getRecordDate() const { return recordDate; }
    // තෝරාගත් slots list එක ලබාදෙයි.
    QString getTimeSlots() const { return timeSlots; }
    // booking විස්තරය ලබාදෙයි.
    QString getDescription() const { return description; }
    // allocation key එක ලබාදෙයි.
    QString getPasscode() const { return passcode; }
    // මුලින් save කළ වේලාව ලබාදෙයි.
    QString getCreatedAt() const { return createdAt; }
    // Pending/Approve/Reject තත්ත්වය ලබාදෙයි.
    QString getStatus() const { return status; }
    // admin සටහන ලබාදෙයි.
    QString getAdminRemarks() const { return adminRemarks; }

    // database row ID එක අලුත් අගයට සකසයි.
    void setId(int newId) { id = newId; }
    // workshop category එකේ අනෙක් නම අලුත් අගයට සකසයි.
    void setWorkshopId(const QString &newWorkshopId) { workshopId = newWorkshopId; }
    // workshop category එක අලුත් අගයට සකසයි.
    void setCategory(const QString &newCategory) { category = newCategory; }
    // student නම අලුත් අගයට සකසයි.
    void setName(const QString &newName) { name = newName; }
    // student index එක අලුත් අගයට සකසයි.
    void setIndexNo(const QString &newIndexNo) { indexNo = newIndexNo; }
    // student department එක අලුත් අගයට සකසයි.
    void setDepartment(const QString &newDepartment) { department = newDepartment; }
    // student email එක අලුත් අගයට සකසයි.
    void setEmail(const QString &newEmail) { email = newEmail; }
    // supervisor නම අලුත් අගයට සකසයි.
    void setSupervisorName(const QString &newSupervisorName) { supervisorName = newSupervisorName; }
    // project නම අලුත් අගයට සකසයි.
    void setProjectName(const QString &newProjectName) { projectName = newProjectName; }
    // booking දිනය අලුත් අගයට සකසයි.
    void setRecordDate(const QString &newRecordDate) { recordDate = newRecordDate; }
    // තෝරාගත් slots list එක අලුත් අගයට සකසයි.
    void setTimeSlots(const QString &newTimeSlots) { timeSlots = newTimeSlots; }
    // booking විස්තරය අලුත් අගයට සකසයි.
    void setDescription(const QString &newDescription) { description = newDescription; }
    // allocation key එක අලුත් අගයට සකසයි.
    void setPasscode(const QString &newPasscode) { passcode = newPasscode; }
    // මුලින් save කළ වේලාව අලුත් අගයට සකසයි.
    void setCreatedAt(const QString &newCreatedAt) { createdAt = newCreatedAt; }
    // Pending/Approve/Reject තත්ත්වය අලුත් අගයට සකසයි.
    void setStatus(const QString &newStatus) { status = newStatus; }
    // admin සටහන අලුත් අගයට සකසයි.
    void setAdminRemarks(const QString &newRemarks) { adminRemarks = newRemarks; }

  // මෙම class එක තුළ පමණක් භාවිත කරන functions සහ දත්ත.
  private:
    int id; // database row ID එක.
    QString workshopId; // workshop category එකේ අනෙක් නම.
    QString category; // workshop category එක.
    QString name; // student නම.
    QString indexNo; // student index එක.
    QString department; // student department එක.
    QString email; // student email එක.
    QString supervisorName; // supervisor නම.
    QString projectName; // project නම.
    QString recordDate; // booking දිනය.
    QString timeSlots; // UI සඳහා slots text list එක; database එකේ එක් slot එකකට වෙනම row එකක් ඇත.
    QString description; // booking විස්තරය.
    QString passcode; // allocation key එක.
    QString createdAt; // මුලින් save කළ වේලාව.
    QString status; // Pending/Approve/Reject තත්ත්වය.
    QString adminRemarks; // admin සටහන.
};

#endif
