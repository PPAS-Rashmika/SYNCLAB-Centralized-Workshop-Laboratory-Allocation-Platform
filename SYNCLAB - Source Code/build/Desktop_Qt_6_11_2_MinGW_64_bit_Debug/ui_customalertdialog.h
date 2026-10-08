/********************************************************************************
** Form generated from reading UI file 'customalertdialog.ui'
**
** Created by: Qt User Interface Compiler version 6.11.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CUSTOMALERTDIALOG_H
#define UI_CUSTOMALERTDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>

QT_BEGIN_NAMESPACE

class Ui_CustomAlertDialog
{
public:
    QLabel *titleLbl;
    QLabel *messageLbl;
    QPushButton *cancelBtn;
    QPushButton *okBtn;

    void setupUi(QDialog *CustomAlertDialog)
    {
        if (CustomAlertDialog->objectName().isEmpty())
            CustomAlertDialog->setObjectName("CustomAlertDialog");
        CustomAlertDialog->resize(450, 225);
        CustomAlertDialog->setStyleSheet(QString::fromUtf8("QDialog { background-color: #FFFFFF; color: #0F172A; }\n"
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
        CustomAlertDialog->setFont(font);
        CustomAlertDialog->setMinimumSize(QSize(450, 225));
        titleLbl = new QLabel(CustomAlertDialog);
        titleLbl->setObjectName("titleLbl");
        titleLbl->setGeometry(QRect(24, 24, 402, 34));
        titleLbl->setWordWrap(true);
        titleLbl->setTextFormat(Qt::PlainText);
        QFont font1;
        font1.setPointSize(16);
        font1.setBold(true);
        titleLbl->setFont(font1);
        messageLbl = new QLabel(CustomAlertDialog);
        messageLbl->setObjectName("messageLbl");
        messageLbl->setGeometry(QRect(24, 70, 402, 69));
        messageLbl->setTextFormat(Qt::PlainText);
        QFont font2;
        font2.setPointSize(12);
        messageLbl->setFont(font2);
        messageLbl->setWordWrap(true);
        cancelBtn = new QPushButton(CustomAlertDialog);
        cancelBtn->setObjectName("cancelBtn");
        cancelBtn->setGeometry(QRect(24, 155, 195, 46));
        cancelBtn->setMinimumSize(QSize(120, 40));
        cancelBtn->setCursor(QCursor(Qt::CursorShape::PointingHandCursor));
        QFont font3;
        font3.setPointSize(10);
        font3.setBold(true);
        cancelBtn->setFont(font3);
        okBtn = new QPushButton(CustomAlertDialog);
        okBtn->setObjectName("okBtn");
        okBtn->setGeometry(QRect(231, 155, 195, 46));
        okBtn->setMinimumSize(QSize(120, 40));
        okBtn->setCursor(QCursor(Qt::CursorShape::PointingHandCursor));
        okBtn->setFont(font3);

        retranslateUi(CustomAlertDialog);

        QMetaObject::connectSlotsByName(CustomAlertDialog);
    } // setupUi

    void retranslateUi(QDialog *CustomAlertDialog)
    {
        titleLbl->setText(QCoreApplication::translate("CustomAlertDialog", "Information", nullptr));
        messageLbl->setText(QCoreApplication::translate("CustomAlertDialog", "Your allocation details will appear here.", nullptr));
        cancelBtn->setText(QCoreApplication::translate("CustomAlertDialog", "Cancel", nullptr));
        okBtn->setText(QCoreApplication::translate("CustomAlertDialog", "OK", nullptr));
        (void)CustomAlertDialog;
    } // retranslateUi

};

namespace Ui {
    class CustomAlertDialog: public Ui_CustomAlertDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CUSTOMALERTDIALOG_H
