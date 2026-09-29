#include "dialogtheme.h"

#include <QLabel>
#include <QPushButton>
#include <QSettings>

void applyDialogTheme(QDialog *dialog, const QString &accent)
{
    const bool dark = QSettings(QSettings::defaultFormat(), QSettings::UserScope,
                                "StudentWorkshopApp", "ThemeSettings")
                          .value("darkMode", false)
                          .toBool();
    const QString background = dark ? "#172033" : "#FFFFFF";
    const QString foreground = dark ? "#F1F5F9" : "#0F172A";
    const QString input = dark ? "#0F172A" : "#F8FAFC";
    const QString border = dark ? "#475569" : "#CBD5E1";

    dialog->setWindowFlags(Qt::Dialog | Qt::WindowTitleHint | Qt::WindowSystemMenuHint |
                           Qt::WindowCloseButtonHint);
    dialog->setMinimumWidth(420);
    dialog->setStyleSheet(
        QString("QDialog { background-color: %1; color: %2; }"
                "QLabel { color: %2; background: transparent; }"
                "QLineEdit, QComboBox { background-color: %3; color: %2; border: 1px solid %4;"
                " border-radius: 7px; padding: 8px; font-size: 14px; min-height: 22px; }"
                "QLineEdit:focus, QComboBox:focus { border: 1px solid %5; }"
                "QComboBox QAbstractItemView { background-color: %3; color: %2;"
                " selection-background-color: %5; selection-color: white; border: 1px solid %4; }"
                "QPushButton { background-color: %5; color: white; border: 2px solid transparent;"
                " border-radius: 8px; font-size: 14px; font-weight: bold; min-height: 38px; "
                "padding: 2px 14px; }"
                "QPushButton:focus, QPushButton:default { border-color: %2; }"
                "QPushButton:hover { border-color: %4; }"
                "QPushButton:disabled { background-color: %4; color: %2; }"
                "QPushButton#cancelBtn, QPushButton#closeBtn { background-color: %3; color: %2; "
                "border-color: %4; }"
                "QGroupBox { color: %2; font-weight: bold; border: 1px solid %4;"
                " border-radius: 8px; margin-top: 16px; padding-top: 10px; }"
                "QGroupBox::title { subcontrol-origin: margin; left: 12px; padding: 0 4px; }")
            .arg(background, foreground, input, border, accent));

    for (auto *button : dialog->findChildren<QPushButton *>())
        button->setCursor(Qt::PointingHandCursor);
    for (auto *label : dialog->findChildren<QLabel *>())
        label->setTextFormat(Qt::PlainText);
}
