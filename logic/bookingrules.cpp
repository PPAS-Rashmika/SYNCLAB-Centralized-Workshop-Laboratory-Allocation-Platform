#include "bookingrules.h"

#include <QDate>
#include <QDateTime>
#include <QRandomGenerator>
#include <QRegularExpression>

// Checks the booking from the top of the form downwards and returns the first
// problem it finds. The order matters: it is the same order the user filled in.
BookingRules::Field BookingRules::validate(const StudentRecord &record, bool departmentSelected)
{
    if (record.getName().trimmed().isEmpty()) {
        return Field::Name;
    }
    if (record.getIndexNo().trimmed().isEmpty()) {
        return Field::Index;
    }
    if (!departmentSelected) {
        return Field::Department;
    }

    // A simple pattern for "something @ something . something".
    const QRegularExpression emailPattern("^[A-Za-z0-9._%+\\-]+@[A-Za-z0-9.\\-]+\\.[A-Za-z]{2,}$");
    if (!emailPattern.match(record.getEmail().trimmed()).hasMatch()) {
        return Field::Email;
    }

    // The booking needs a workshop, a date that has not already passed, and at
    // least one time slot.
    const QDate date = QDate::fromString(record.getRecordDate(), Qt::ISODate);
    if (record.getCategory().isEmpty() || date < QDate::currentDate() ||
        record.getTimeSlots().isEmpty()) {
        return Field::Schedule;
    }
    return Field::None;
}

// The sentence that goes with each problem above.
QString BookingRules::validationMessage(Field field)
{
    switch (field) {
    case Field::Name:
        return "Enter your full name.";
    case Field::Index:
        return "Enter your student index.";
    case Field::Department:
        return "Select your department.";
    case Field::Email:
        return "Enter a valid email address.";
    case Field::Schedule:
        return "Choose a current or future date and at least one available time slot.";
    case Field::None:
        return QString();
    }
    return QString();
}

void BookingRules::prepareNewBooking(StudentRecord &record)
{
    // A new booking receives its own 4-digit allocation key.
    // bounded(10000) gives a number from 0 to 9999, and rightJustified pads
    // short numbers with zeros so the key is always four digits, for example 0042.
    const int key = QRandomGenerator::global()->bounded(10000);
    record.setPasscode(QString::number(key).rightJustified(4, QChar('0')));
    record.setCreatedAt(QDateTime::currentDateTime().toString(Qt::ISODate));
    record.setStatus("Pending");
}

void BookingRules::prepareUpdatedBooking(StudentRecord &record, bool studentEdit)
{
    // An administrator must review changes submitted by a student again, so the
    // booking goes back to "Pending". An administrator's own edit keeps the
    // status it already had.
    if (studentEdit) {
        record.setStatus("Pending");
    }
}
