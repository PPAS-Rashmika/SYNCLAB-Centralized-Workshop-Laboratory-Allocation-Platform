#ifndef BOOKINGRULES_H
#define BOOKINGRULES_H

#include "studentrecord.h"

// The booking rules work with data only. They do not read or change any widgets.
class BookingRules
{
  public:
    enum class Field { None, Name, Index, Department, Email, Schedule };

    static Field validate(const StudentRecord &record, bool departmentSelected);
    static QString validationMessage(Field field);
    static void prepareNewBooking(StudentRecord &record);
    static void prepareUpdatedBooking(StudentRecord &record, bool studentEdit);
};

#endif
