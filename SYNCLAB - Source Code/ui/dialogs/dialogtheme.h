// Dialogs වල පාට සකසන function එක මෙහි ප්‍රකාශ කරයි.
// එකම header එක කිහිප වරක් include වුවත් definitions එකවරක් පමණක් යෙදීමට මේ guard එක ඇත.
#ifndef DIALOGTHEME_H
#define DIALOGTHEME_H

// #include වලින් මේ file එකට අවශ්‍ය Qt සහ project declarations ලබාගනී.
#include <QDialog>
#include <QString>

// Save කළ theme එක dialog එකට යොදා warning/error accent එකත් සකසයි.
void applyDialogTheme(QDialog *dialog, const QString &alertType = QString());

#endif
