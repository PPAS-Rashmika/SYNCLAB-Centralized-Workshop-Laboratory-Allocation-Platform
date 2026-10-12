// Form එකේ දත්ත නිවැරදිදැයි බලයි. අවශ්‍ය inputs සහ දෝෂ පණිවිඩ මෙහි වෙනස් කරන්න.
// #include වලින් මේ file එකට අවශ්‍ය Qt සහ project declarations ලබාගනී.
#include "allocationrules.h"

#include <QDate>
#include <QDateTime>
#include <QRandomGenerator>
#include <QRegularExpression>

// Form එකේ පළමු වැරදි field එක ලබාදෙයි; සියල්ල හරි නම් Field::None දෙයි.
AllocationRules::Field AllocationRules::validate(const StudentRecord &record, bool departmentSelected)
{
    // Field පරීක්ෂාව form එකේ අනුපිළිවෙළින් කරයි; පළමු වැරැද්ද පමණක් ලබාදෙයි.
    if (record.getName().trimmed().isEmpty()) {
        return Field::Name;
    }
    if (record.getIndexNo().trimmed().isEmpty()) {
        return Field::Index;
    }
    if (!departmentSelected) {
        return Field::Department;
    }

    // Email validation රටාවයි; වෙනස් කළොත් database email පරීක්ෂාවත් ගළපන්න.
    const QRegularExpression emailPattern("^[A-Za-z0-9._%+\\-]+@[A-Za-z0-9.\\-]+\\.[A-Za-z]{2,}$");
    if (!emailPattern.match(record.getEmail().trimmed()).hasMatch()) {
        return Field::Email;
    }

    if (record.getSupervisorName().trimmed().isEmpty()) {
        return Field::Supervisor;
    }
    if (record.getProjectName().trimmed().isEmpty()) {
        return Field::Project;
    }

    // අද හෝ ඉදිරි දිනයක්, workshop එකක් සහ slot එකක් අවශ්‍යයි.
    const QDate date = QDate::fromString(record.getRecordDate(), Qt::ISODate);
    if (record.getCategory().isEmpty() || date < QDate::currentDate() ||
        record.getTimeSlots().isEmpty()) {
        return Field::Schedule;
    }
    return Field::None;
}

// වැරදි field එකට පෙන්වන පණිවිඩය තෝරයි. Messages වෙනස් කිරීමට return text වෙනස් කරන්න.
QString AllocationRules::validationMessage(Field field)
{
    switch (field) {
    case Field::Name:
        return "Enter your name.";
    case Field::Index:
        return "Enter your student index.";
    case Field::Department:
        return "Select your department.";
    case Field::Email:
        return "Enter a valid email address.";
    case Field::Supervisor:
        return "Enter your supervisor's name.";
    case Field::Project:
        return "Enter your project name.";
    case Field::Schedule:
        return "Choose a current or future date and at least one available time slot.";
    case Field::None:
        return QString();
    }
    return QString();
}

// නව booking එකට ඉලක්කම් හතරේ key, created_at සහ Pending status සකසයි.
void AllocationRules::prepareNewAllocation(StudentRecord &record)
{
    // 0-9999 random අගයයි; rightJustified(4) අඩු ඉලක්කම් ඉදිරියට 0 දමා පුරවයි.
    const int key = QRandomGenerator::global()->bounded(10000);
    record.setPasscode(QString::number(key).rightJustified(4, QChar('0')));
    record.setCreatedAt(QDateTime::currentDateTime().toString(Qt::ISODate));
    record.setStatus("Pending");
}

// Student කළ edit එකකට නැවත Pending යොදයි; admin edit එකේ status එක තබයි.
void AllocationRules::prepareUpdatedAllocation(StudentRecord &record, bool studentEdit)
{
    // Student edit එක නැවත admin review කිරීමට Pending කරයි.
    if (studentEdit) {
        record.setStatus("Pending");
    }
}
