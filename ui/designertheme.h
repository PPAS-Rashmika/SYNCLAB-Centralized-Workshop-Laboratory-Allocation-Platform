// පිටු සහ dialogs වල light/dark පාට වෙනස් කරන function එක මෙහි ප්‍රකාශ කරයි.
// එකම header එක කිහිප වරක් include වුවත් definitions එකවරක් පමණක් යෙදීමට මේ guard එක ඇත.
#ifndef DESIGNERTHEME_H
#define DESIGNERTHEME_H

class QWidget;

// Controls වල මුල් style මතක තබා theme මාරු කරන විට අදාළ පාට යොදයි.
void applyDesignerTheme(QWidget *root, bool darkMode);

#endif
