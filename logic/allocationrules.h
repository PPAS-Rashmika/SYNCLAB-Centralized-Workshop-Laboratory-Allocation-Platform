#ifndef ALLOCATIONRULES_H
#define ALLOCATIONRULES_H

#include "studentrecord.h"

// AllocationRules holds the business rules of the program.
//
// Every function is 'static', which means it is called on the class itself
// (AllocationRules::validate(...)) and no object of this class is ever created.
// It is used as a tidy place to keep the rules, so that MainWindow only deals
// with widgets and the database only deals with SQL.
//
// Nothing in this class touches the user interface, so the rules can be read
// and checked on their own.
class AllocationRules
{
  public:
    // Which part of the form is wrong. None means the allocation is valid.
    enum class Field { None, Name, Index, Department, Email, Supervisor, Project, Schedule };

    // Checks an allocation and returns the first field that is wrong.
    static Field validate(const StudentRecord &record, bool departmentSelected);
    // Turns that answer into the sentence shown to the user.
    static QString validationMessage(Field field);
    // Fills in the values only a brand new allocation needs.
    static void prepareNewAllocation(StudentRecord &record);
    // Fills in the values that change when an existing allocation is edited.
    static void prepareUpdatedAllocation(StudentRecord &record, bool studentEdit);
};

#endif
