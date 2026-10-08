#ifndef BOOKINGRULES_H
#define BOOKINGRULES_H

#include "studentrecord.h"

// BookingRules holds the business rules of the program.
//
// Every function is 'static', which means it is called on the class itself
// (BookingRules::validate(...)) and no object of this class is ever created.
// It is used as a tidy place to keep the rules, so that MainWindow only deals
// with widgets and the database only deals with SQL.
//
// Nothing in this class touches the user interface, so the rules can be read
// and checked on their own.
class BookingRules
{
  public:
    // Which part of the form is wrong. None means the booking is valid.
    enum class Field { None, Name, Index, Department, Email, Supervisor, Project, Schedule };

    // Checks a booking and returns the first field that is wrong.
    static Field validate(const StudentRecord &record, bool departmentSelected);
    // Turns that answer into the sentence shown to the user.
    static QString validationMessage(Field field);
    // Fills in the values only a brand new booking needs.
    static void prepareNewBooking(StudentRecord &record);
    // Fills in the values that change when an existing booking is edited.
    static void prepareUpdatedBooking(StudentRecord &record, bool studentEdit);
};

#endif
