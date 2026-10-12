// Save කළ light/dark setting එක කියවා dialogs වල පාට සහ buttons සකසයි.
// #include වලින් මේ file එකට අවශ්‍ය Qt සහ project declarations ලබාගනී.
#include "dialogtheme.h"
#include "designertheme.h"

#include <QLabel>
#include <QPushButton>
#include <QSettings>

// Save කළ theme එක dialog එකට යොදා warning/error accent එකත් සකසයි.
void applyDialogTheme(QDialog *dialog, const QString &accent)
{
    // User කලින් save කළ darkMode setting එක කියවයි.
    const bool dark = QSettings(QSettings::defaultFormat(), QSettings::UserScope,
                                "StudentWorkshopApp", "ThemeSettings")
                          .value("darkMode", false)
                          .toBool();

    // Dialog title bar සහ close button පමණක් පෙන්වන flags යොදයි.
    dialog->setWindowFlags(Qt::Dialog | Qt::WindowTitleHint | Qt::WindowSystemMenuHint |
                           Qt::WindowCloseButtonHint);
    // Warning/error සඳහා blue accent එක අදාළ colour එකට මාරු කරයි.
    if (accent != "#2563EB") {
        dialog->setStyleSheet(QString(dialog->styleSheet()).replace("#2563EB", accent,
                                                                   Qt::CaseInsensitive));
    }
    applyDesignerTheme(dialog, dark);

    // Dialog buttons මත mouse එක තිබෙන විට අත ලකුණ පෙන්වයි.
    for (auto *button : dialog->findChildren<QPushButton *>())
        button->setCursor(Qt::PointingHandCursor);
    for (auto *label : dialog->findChildren<QLabel *>())
        label->setTextFormat(Qt::PlainText);
}
