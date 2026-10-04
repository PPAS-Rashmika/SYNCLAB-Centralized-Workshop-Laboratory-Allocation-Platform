#ifndef DESIGNERTHEME_H
#define DESIGNERTHEME_H

class QWidget;

// Keep the .ui stylesheet as the source of appearance. Theme changes translate
// only the supplied light palette's colours; fonts and geometry stay authored.
void applyDesignerTheme(QWidget *root, bool darkMode);

#endif
