#include "dialogtheme.h"
#include "designertheme.h"

#include <QLabel>
#include <QPushButton>
#include <QSettings>

// Reads the saved theme once and styles a dialog to match the main window.
void applyDialogTheme(QDialog *dialog, const QString &accent)
{
    // QSettings reads the small configuration file the program writes to.
    const bool dark = QSettings(QSettings::defaultFormat(), QSettings::UserScope,
                                "StudentWorkshopApp", "ThemeSettings")
                          .value("darkMode", false)
                          .toBool();

    dialog->setWindowFlags(Qt::Dialog | Qt::WindowTitleHint | Qt::WindowSystemMenuHint |
                           Qt::WindowCloseButtonHint);
    // Layout, fonts and normal button colours come from the editable .ui file.
    // Warnings/errors still receive their contextual accent before the snapshot.
    if (accent != "#2563EB") {
        dialog->setStyleSheet(QString(dialog->styleSheet()).replace("#2563EB", accent,
                                                                   Qt::CaseInsensitive));
    }
    applyDesignerTheme(dialog, dark);

    for (auto *button : dialog->findChildren<QPushButton *>())
        button->setCursor(Qt::PointingHandCursor);
    for (auto *label : dialog->findChildren<QLabel *>())
        label->setTextFormat(Qt::PlainText);
}
