/********************************************************************************
** Form generated from reading UI file 'systemsettingsdialog.ui'
**
** Created by: Qt User Interface Compiler version 6.11.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SYSTEMSETTINGSDIALOG_H
#define UI_SYSTEMSETTINGSDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>

QT_BEGIN_NAMESPACE

class Ui_SystemSettingsDialog
{
public:
    QLabel *titleLbl;
    QPushButton *toggleThemeBtn;
    QPushButton *changeWallpaperBtn;
    QGroupBox *groupBox;
    QLabel *lblCat;
    QComboBox *workshopCombo;
    QLabel *lblCode;
    QLineEdit *newPasscodeInput;
    QPushButton *savePasscodeBtn;
    QPushButton *closeBtn;

    void setupUi(QDialog *SystemSettingsDialog)
    {
        if (SystemSettingsDialog->objectName().isEmpty())
            SystemSettingsDialog->setObjectName("SystemSettingsDialog");
        SystemSettingsDialog->resize(560, 540);
        SystemSettingsDialog->setStyleSheet(QString::fromUtf8("QDialog { background-color: #FFFFFF; color: #0F172A; }\n"
"QLabel { color: #0F172A; background: transparent; }\n"
"QLineEdit, QComboBox { background-color: #F8FAFC; color: #0F172A; border: 1px solid #CBD5E1; border-radius: 7px; padding: 8px; }\n"
"QLineEdit:focus, QComboBox:focus { border: 1px solid #2563EB; }\n"
"QComboBox QAbstractItemView { background-color: #F8FAFC; color: #0F172A; selection-background-color: #2563EB; selection-color: white; border: 1px solid #CBD5E1; }\n"
"QPushButton { background-color: #2563EB; color: white; border: 2px solid transparent; border-radius: 8px; padding: 2px 14px; }\n"
"QPushButton:focus, QPushButton:default { border-color: #0F172A; }\n"
"QPushButton:hover { border-color: #CBD5E1; }\n"
"QPushButton:disabled { background-color: #CBD5E1; color: #0F172A; }\n"
"QPushButton#cancelBtn, QPushButton#closeBtn { background-color: #F8FAFC; color: #0F172A; border-color: #CBD5E1; }\n"
"QGroupBox { color: #0F172A; border: 1px solid #CBD5E1; border-radius: 8px; margin-top: 16px; padding-t"
                        "op: 10px; }\n"
"QGroupBox::title { subcontrol-origin: margin; left: 12px; padding: 0 4px; }"));
        QFont font;
        font.setFamilies({QString::fromUtf8("Segoe UI")});
        font.setPointSize(10);
        SystemSettingsDialog->setFont(font);
        SystemSettingsDialog->setMinimumSize(QSize(560, 540));
        titleLbl = new QLabel(SystemSettingsDialog);
        titleLbl->setObjectName("titleLbl");
        titleLbl->setGeometry(QRect(24, 24, 512, 34));
        titleLbl->setWordWrap(true);
        titleLbl->setTextFormat(Qt::PlainText);
        QFont font1;
        font1.setPointSize(16);
        font1.setBold(true);
        titleLbl->setFont(font1);
        titleLbl->setAlignment(Qt::AlignCenter);
        toggleThemeBtn = new QPushButton(SystemSettingsDialog);
        toggleThemeBtn->setObjectName("toggleThemeBtn");
        toggleThemeBtn->setGeometry(QRect(24, 80, 512, 46));
        toggleThemeBtn->setMinimumSize(QSize(0, 45));
        toggleThemeBtn->setCursor(QCursor(Qt::CursorShape::PointingHandCursor));
        QFont font2;
        font2.setPointSize(10);
        font2.setBold(true);
        toggleThemeBtn->setFont(font2);
        changeWallpaperBtn = new QPushButton(SystemSettingsDialog);
        changeWallpaperBtn->setObjectName("changeWallpaperBtn");
        changeWallpaperBtn->setGeometry(QRect(24, 138, 512, 46));
        changeWallpaperBtn->setMinimumSize(QSize(0, 45));
        changeWallpaperBtn->setCursor(QCursor(Qt::CursorShape::PointingHandCursor));
        changeWallpaperBtn->setFont(font2);
        groupBox = new QGroupBox(SystemSettingsDialog);
        groupBox->setObjectName("groupBox");
        groupBox->setGeometry(QRect(24, 196, 512, 208));
        QFont font3;
        font3.setBold(true);
        groupBox->setFont(font3);
        lblCat = new QLabel(groupBox);
        lblCat->setObjectName("lblCat");
        lblCat->setGeometry(QRect(12, 39, 138, 38));
        lblCat->setWordWrap(true);
        lblCat->setTextFormat(Qt::PlainText);
        workshopCombo = new QComboBox(groupBox);
        workshopCombo->setObjectName("workshopCombo");
        workshopCombo->setGeometry(QRect(160, 39, 340, 38));
        workshopCombo->setMinimumSize(QSize(0, 35));
        QFont font4;
        font4.setPointSize(10);
        workshopCombo->setFont(font4);
        lblCode = new QLabel(groupBox);
        lblCode->setObjectName("lblCode");
        lblCode->setGeometry(QRect(12, 89, 138, 38));
        lblCode->setWordWrap(true);
        lblCode->setTextFormat(Qt::PlainText);
        newPasscodeInput = new QLineEdit(groupBox);
        newPasscodeInput->setObjectName("newPasscodeInput");
        newPasscodeInput->setGeometry(QRect(160, 89, 340, 38));
        newPasscodeInput->setMinimumSize(QSize(0, 35));
        newPasscodeInput->setEchoMode(QLineEdit::Password);
        newPasscodeInput->setFont(font4);
        savePasscodeBtn = new QPushButton(groupBox);
        savePasscodeBtn->setObjectName("savePasscodeBtn");
        savePasscodeBtn->setGeometry(QRect(12, 145, 488, 46));
        savePasscodeBtn->setMinimumSize(QSize(0, 40));
        savePasscodeBtn->setCursor(QCursor(Qt::CursorShape::PointingHandCursor));
        savePasscodeBtn->setFont(font2);
        closeBtn = new QPushButton(SystemSettingsDialog);
        closeBtn->setObjectName("closeBtn");
        closeBtn->setGeometry(QRect(24, 470, 512, 46));
        closeBtn->setMinimumSize(QSize(0, 45));
        closeBtn->setCursor(QCursor(Qt::CursorShape::PointingHandCursor));
        closeBtn->setFont(font2);
        QWidget::setTabOrder(toggleThemeBtn, changeWallpaperBtn);
        QWidget::setTabOrder(changeWallpaperBtn, workshopCombo);
        QWidget::setTabOrder(workshopCombo, newPasscodeInput);
        QWidget::setTabOrder(newPasscodeInput, savePasscodeBtn);
        QWidget::setTabOrder(savePasscodeBtn, closeBtn);

        retranslateUi(SystemSettingsDialog);

        QMetaObject::connectSlotsByName(SystemSettingsDialog);
    } // setupUi

    void retranslateUi(QDialog *SystemSettingsDialog)
    {
        SystemSettingsDialog->setWindowTitle(QCoreApplication::translate("SystemSettingsDialog", "System settings", nullptr));
        titleLbl->setText(QCoreApplication::translate("SystemSettingsDialog", "System Settings", nullptr));
        toggleThemeBtn->setText(QCoreApplication::translate("SystemSettingsDialog", "Switch to dark mode", nullptr));
        changeWallpaperBtn->setText(QCoreApplication::translate("SystemSettingsDialog", "Change Background Wallpaper", nullptr));
        groupBox->setTitle(QCoreApplication::translate("SystemSettingsDialog", "Administrator passcodes", nullptr));
        lblCat->setText(QCoreApplication::translate("SystemSettingsDialog", "&Administrator", nullptr));
        lblCode->setText(QCoreApplication::translate("SystemSettingsDialog", "&New passcode", nullptr));
        newPasscodeInput->setPlaceholderText(QCoreApplication::translate("SystemSettingsDialog", "At least 4 characters", nullptr));
        savePasscodeBtn->setText(QCoreApplication::translate("SystemSettingsDialog", "Update Passcode", nullptr));
        closeBtn->setText(QCoreApplication::translate("SystemSettingsDialog", "Close", nullptr));
    } // retranslateUi

};

namespace Ui {
    class SystemSettingsDialog: public Ui_SystemSettingsDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SYSTEMSETTINGSDIALOG_H
