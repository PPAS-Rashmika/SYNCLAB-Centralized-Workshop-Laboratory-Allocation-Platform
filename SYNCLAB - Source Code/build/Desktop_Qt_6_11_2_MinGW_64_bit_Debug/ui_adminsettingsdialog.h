/********************************************************************************
** Form generated from reading UI file 'adminsettingsdialog.ui'
**
** Created by: Qt User Interface Compiler version 6.11.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_ADMINSETTINGSDIALOG_H
#define UI_ADMINSETTINGSDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>

QT_BEGIN_NAMESPACE

class Ui_AdminSettingsDialog
{
public:
    QLabel *titleLbl;
    QLabel *lbl1;
    QLineEdit *deanEmailInput;
    QLabel *lbl2;
    QLineEdit *depHeadEmailInput;
    QPushButton *cancelBtn;
    QPushButton *saveBtn;

    void setupUi(QDialog *AdminSettingsDialog)
    {
        if (AdminSettingsDialog->objectName().isEmpty())
            AdminSettingsDialog->setObjectName("AdminSettingsDialog");
        AdminSettingsDialog->resize(450, 348);
        AdminSettingsDialog->setMinimumSize(QSize(450, 348));
        QFont font;
        font.setFamilies({QString::fromUtf8("Segoe UI")});
        font.setPointSize(10);
        AdminSettingsDialog->setFont(font);
        AdminSettingsDialog->setStyleSheet(QString::fromUtf8("QDialog { background-color: #FFFFFF; color: #0F172A; }\n"
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
        titleLbl = new QLabel(AdminSettingsDialog);
        titleLbl->setObjectName("titleLbl");
        titleLbl->setGeometry(QRect(24, 24, 402, 34));
        QFont font1;
        font1.setPointSize(16);
        font1.setBold(true);
        titleLbl->setFont(font1);
        titleLbl->setTextFormat(Qt::TextFormat::PlainText);
        titleLbl->setWordWrap(true);
        lbl1 = new QLabel(AdminSettingsDialog);
        lbl1->setObjectName("lbl1");
        lbl1->setGeometry(QRect(24, 70, 402, 20));
        lbl1->setTextFormat(Qt::TextFormat::PlainText);
        lbl1->setWordWrap(true);
        deanEmailInput = new QLineEdit(AdminSettingsDialog);
        deanEmailInput->setObjectName("deanEmailInput");
        deanEmailInput->setGeometry(QRect(24, 102, 402, 40));
        deanEmailInput->setMinimumSize(QSize(0, 40));
        QFont font2;
        font2.setPointSize(10);
        deanEmailInput->setFont(font2);
        lbl2 = new QLabel(AdminSettingsDialog);
        lbl2->setObjectName("lbl2");
        lbl2->setGeometry(QRect(24, 154, 402, 20));
        lbl2->setTextFormat(Qt::TextFormat::PlainText);
        lbl2->setWordWrap(true);
        depHeadEmailInput = new QLineEdit(AdminSettingsDialog);
        depHeadEmailInput->setObjectName("depHeadEmailInput");
        depHeadEmailInput->setGeometry(QRect(24, 186, 402, 40));
        depHeadEmailInput->setMinimumSize(QSize(0, 40));
        depHeadEmailInput->setFont(font2);
        cancelBtn = new QPushButton(AdminSettingsDialog);
        cancelBtn->setObjectName("cancelBtn");
        cancelBtn->setGeometry(QRect(24, 278, 195, 46));
        cancelBtn->setMinimumSize(QSize(120, 40));
        QFont font3;
        font3.setPointSize(10);
        font3.setBold(true);
        cancelBtn->setFont(font3);
        cancelBtn->setCursor(QCursor(Qt::CursorShape::PointingHandCursor));
        saveBtn = new QPushButton(AdminSettingsDialog);
        saveBtn->setObjectName("saveBtn");
        saveBtn->setGeometry(QRect(231, 278, 195, 46));
        saveBtn->setMinimumSize(QSize(120, 40));
        saveBtn->setFont(font3);
        saveBtn->setCursor(QCursor(Qt::CursorShape::PointingHandCursor));
        QWidget::setTabOrder(deanEmailInput, depHeadEmailInput);
        QWidget::setTabOrder(depHeadEmailInput, saveBtn);
        QWidget::setTabOrder(saveBtn, cancelBtn);

        retranslateUi(AdminSettingsDialog);

        QMetaObject::connectSlotsByName(AdminSettingsDialog);
    } // setupUi

    void retranslateUi(QDialog *AdminSettingsDialog)
    {
        AdminSettingsDialog->setWindowTitle(QCoreApplication::translate("AdminSettingsDialog", "Administration email settings", nullptr));
        titleLbl->setText(QCoreApplication::translate("AdminSettingsDialog", "Admin Settings", nullptr));
        lbl1->setText(QCoreApplication::translate("AdminSettingsDialog", "Lab / Workshop In-charge Email", nullptr));
        deanEmailInput->setPlaceholderText(QCoreApplication::translate("AdminSettingsDialog", "Required in-charge email address", nullptr));
        lbl2->setText(QCoreApplication::translate("AdminSettingsDialog", "Department or head email", nullptr));
        depHeadEmailInput->setPlaceholderText(QCoreApplication::translate("AdminSettingsDialog", "Required department or head email address", nullptr));
        cancelBtn->setText(QCoreApplication::translate("AdminSettingsDialog", "Cancel", nullptr));
        saveBtn->setText(QCoreApplication::translate("AdminSettingsDialog", "Save Settings", nullptr));
    } // retranslateUi

};

namespace Ui {
    class AdminSettingsDialog: public Ui_AdminSettingsDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ADMINSETTINGSDIALOG_H
