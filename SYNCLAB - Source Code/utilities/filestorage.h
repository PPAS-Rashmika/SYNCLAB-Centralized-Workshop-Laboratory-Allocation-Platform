// Database file එකේ සම්පූර්ණ path එක ලබාදෙන function එක මෙහි ප්‍රකාශ කරයි.
// එකම header එක කිහිප වරක් include වුවත් definitions එකවරක් පමණක් යෙදීමට මේ guard එක ඇත.
#ifndef FILESTORAGE_H
#define FILESTORAGE_H

// #include වලින් මේ file එකට අවශ්‍ය Qt සහ project declarations ලබාගනී.
#include <QString>


// මුලින් තිබෙන database file එක භාවිත කරයි; නැතිනම් අලුත් data folder එකේ path එක දෙයි.
QString defaultDatabasePath();

#endif
