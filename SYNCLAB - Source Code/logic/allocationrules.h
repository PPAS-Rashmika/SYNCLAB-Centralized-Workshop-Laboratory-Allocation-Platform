// Booking පරීක්ෂා කිරීමේ නීති සහ අසාර්ථක වූ field එක හඳුනාගන්නා නම් මෙහි ඇත.
// එකම header එක කිහිප වරක් include වුවත් definitions එකවරක් පමණක් යෙදීමට මේ guard එක ඇත.
#ifndef ALLOCATIONRULES_H
#define ALLOCATIONRULES_H

// #include වලින් මේ file එකට අවශ්‍ය Qt සහ project declarations ලබාගනී.
#include "studentrecord.h"

// Static functions නිසා object එකක් නොහදා AllocationRules::function() ලෙස නීති භාවිත කරයි.
class AllocationRules
{
  // අනෙක් code වලින් භාවිත කළ හැකි functions.
  public:
    // Validation වැරැද්ද ඇති field නමයි; None යනු සියල්ල නිවැරදියි.
    enum class Field { None, Name, Index, Department, Email, Supervisor, Project, Schedule };

    // Form එකේ පළමු වැරදි field එක ලබාදෙයි; සියල්ල හරි නම් Field::None දෙයි.
    static Field validate(const StudentRecord &record, bool departmentSelected);
    // වැරදි field එකට පෙන්වන පණිවිඩය තෝරයි. Messages වෙනස් කිරීමට return text වෙනස් කරන්න.
    static QString validationMessage(Field field);
    // නව booking එකට ඉලක්කම් හතරේ key, created_at සහ Pending status සකසයි.
    static void prepareNewAllocation(StudentRecord &record);
    // Student කළ edit එකකට නැවත Pending යොදයි; admin edit එකේ status එක තබයි.
    static void prepareUpdatedAllocation(StudentRecord &record, bool studentEdit);
};

#endif
