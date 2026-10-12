// Workshop නම, department සහ image ලබාගන්නා functions මෙහි ප්‍රකාශ කරයි.
// එකම header එක කිහිප වරක් include වුවත් definitions එකවරක් පමණක් යෙදීමට මේ guard එක ඇත.
#ifndef WORKSHOPCATALOG_H
#define WORKSHOPCATALOG_H

// #include වලින් මේ file එකට අවශ්‍ය Qt සහ project declarations ලබාගනී.
#include <QPixmap>
#include <QSize>
#include <QString>
#include <QStringList>


// Cards වල workshop අනුපිළිවෙළයි. මෙය වෙනස් කළොත් selection.ui හි category values ද ගළපන්න.
QStringList workshopCategories();

// Workshop එකට පෙන්වන සම්පූර්ණ නම හදයි; අවශ්‍ය නම් Lab යන කොටස වෙනස් කරන්න.
QString workshopTitle(const QString &category);

// Workshop එක අයත් department එක තෝරයි. Student ගේ department එක මෙයින් තීරණය නොකරයි.
QString workshopDepartment(const QString &category);

// Resource image එක label ප්‍රමාණයට ගළපයි; image නැත්නම් අකුරු සහිත badge එකක් අඳියි.
QPixmap workshopImage(const QString &category, const QSize &size);

#endif
