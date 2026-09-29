#ifndef DIALOGTHEME_H
#define DIALOGTHEME_H

#include <QDialog>
#include <QString>

// Apply the same saved light or dark theme to each dialog.
void applyDialogTheme(QDialog *dialog, const QString &accent = "#2563EB");

#endif // DIALOGTHEME_H
