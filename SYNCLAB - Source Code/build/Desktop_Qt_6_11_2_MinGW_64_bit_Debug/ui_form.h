/********************************************************************************
** Form generated from reading UI file 'form.ui'
**
** Created by: Qt User Interface Compiler version 6.11.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_FORM_H
#define UI_FORM_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QFrame>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_formWidget
{
public:
    QPushButton *backTo2Btn;
    QLabel *confirmTitleLbl;
    QFrame *summaryBox;
    QLabel *summaryDateLbl;
    QLabel *summaryTimeLbl;
    QScrollArea *formScrollArea;
    QWidget *formScrollContents;
    QLabel *label;
    QLineEdit *nameInput;
    QLabel *label_2;
    QLineEdit *indexInput;
    QLabel *label_3;
    QComboBox *deptInput;
    QLabel *label_email;
    QLineEdit *emailInput;
    QLabel *label_supervisor;
    QLineEdit *supervisorInput;
    QLabel *label_project;
    QLineEdit *projectInput;
    QLabel *label_6;
    QTextEdit *descriptionInput;
    QPushButton *deleteBtn;
    QPushButton *updateBtn;
    QPushButton *submitBtn;

    void setupUi(QWidget *formWidget)
    {
        if (formWidget->objectName().isEmpty())
            formWidget->setObjectName("formWidget");
        formWidget->resize(1180, 780);
        QFont font;
        font.setFamilies({QString::fromUtf8("Segoe UI")});
        font.setPointSize(10);
        font.setBold(false);
        formWidget->setFont(font);
        formWidget->setStyleSheet(QString::fromUtf8("QWidget { color:#17243B;   }QMainWindow, QStackedWidget, QWidget#welcomeWidget, QWidget#selectionWidget, QWidget#calanderWidget, QWidget#formWidget, QWidget#adminWidget { background:#F4F7FB; }QScrollArea { border:none; background:transparent; } QScrollArea > QWidget > QWidget { background:#F4F7FB; }QScrollArea#cardsScrollArea > QWidget, QWidget#cardsScrollContents { background:transparent; }QLabel { background:transparent; }QLabel#welcomeTitle { color:#52647D;   }QLabel#syncLabTitle {   }QLabel#syncLabSubtitle {   }QLabel#welcomeDesc, QLabel#selectedDescLabel2, QLabel#adminWsDescLbl { color:#52647D;  }QLabel#selectTitleLabel, QLabel#selectedNameLabel2, QLabel#confirmTitleLbl, QLabel#adminWsTitleLbl {   }QLineEdit, QTextEdit, QComboBox { background:#FFFFFF; border:1px solid #D4DEEB; border-radius:7px; padding:8px; selection-background-color:#2563EB; selection-color:white; }QLineEdit:focus, QTextEdit:focus, QComboBox:focus { border:1px solid #2563EB; }QComboBox QAbstractItemView { background:#FFFFFF; color:#1724"
                        "3B; selection-background-color:#2563EB; selection-color:white; }QPushButton { background:#FFFFFF; color:#17243B; border:1px solid #D4DEEB; border-radius:8px; padding:9px 14px;  }QPushButton:hover, QPushButton:focus { background:#EAF2FF; border-color:#2563EB; }QPushButton:pressed { border:2px solid #2563EB; }QPushButton#allocateNowBtn, QPushButton#nextBtn, QPushButton#nextTo3Btn, QPushButton#submitBtn, QPushButton#updateBtn { background:#2563EB; color:white; border:1px solid #2563EB; }QPushButton#allocateNowBtn:hover, QPushButton#nextBtn:hover, QPushButton#nextTo3Btn:hover, QPushButton#submitBtn:hover, QPushButton#updateBtn:hover { background:#1D4ED8; }QPushButton#deleteBtn, QPushButton#adminDelBtn { color:#BE123C; }QPushButton[timeSlot=true] { padding:10px 8px; }QPushButton:checked { background:#2563EB; color:white; border-color:#2563EB; }QPushButton:disabled { background:#F4F7FB; color:#52647D; border:1px solid #D4DEEB; }QPushButton#allocateNowBtn:disabled, QPushButton#nextBtn:disabled, QPushButton#nextTo3Btn"
                        ":disabled, QPushButton#submitBtn:disabled, QPushButton#updateBtn:disabled, QPushButton#deleteBtn:disabled, QPushButton#adminDelBtn:disabled { background:#F4F7FB; color:#52647D; border:1px solid #D4DEEB; }QFrame#summaryBox { background:#EAF2FF; border:1px solid #D4DEEB; border-radius:10px; }QLabel#summaryDateLbl { color:#2563EB;  }QTableWidget { background:#FFFFFF; alternate-background-color:#F4F7FB; border:1px solid #D4DEEB; gridline-color:#D4DEEB; selection-background-color:#EAF2FF; selection-color:#17243B; }QHeaderView::section { font-weight:600; background:#EAF2FF; color:#17243B; border:none; padding:10px;  }QTableWidget QLineEdit, QTableWidget QComboBox { padding:4px; border-radius:4px; }QCalendarWidget { background:#FFFFFF; border:1px solid #D4DEEB; border-radius:8px; }QCalendarWidget QWidget#qt_calendar_navigationbar { background:#2563EB; }QCalendarWidget QToolButton { font-weight:600; color:white; background:transparent; padding:6px;  }QCalendarWidget QMenu, QCalendarWidget QSpinBox { background:#FFFFFF"
                        "; color:#17243B; }QCalendarWidget QAbstractItemView { background:#FFFFFF; color:#17243B; selection-background-color:#2563EB; selection-color:white; outline:0; }QToolTip { background:#FFFFFF; color:#17243B; border:1px solid #D4DEEB; padding:6px; }"));
        backTo2Btn = new QPushButton(formWidget);
        backTo2Btn->setObjectName("backTo2Btn");
        backTo2Btn->setGeometry(QRect(24, 24, 171, 37));
        QFont font1;
        font1.setFamilies({QString::fromUtf8("Segoe UI")});
        font1.setPointSize(10);
        font1.setBold(true);
        backTo2Btn->setFont(font1);
        backTo2Btn->setCursor(QCursor(Qt::CursorShape::PointingHandCursor));
        confirmTitleLbl = new QLabel(formWidget);
        confirmTitleLbl->setObjectName("confirmTitleLbl");
        confirmTitleLbl->setGeometry(QRect(24, 77, 1132, 33));
        QFont font2;
        font2.setFamilies({QString::fromUtf8("Segoe UI")});
        font2.setPointSize(19);
        font2.setBold(true);
        confirmTitleLbl->setFont(font2);
        confirmTitleLbl->setTextFormat(Qt::TextFormat::PlainText);
        summaryBox = new QFrame(formWidget);
        summaryBox->setObjectName("summaryBox");
        summaryBox->setGeometry(QRect(24, 126, 1132, 72));
        summaryBox->setFont(font);
        summaryDateLbl = new QLabel(summaryBox);
        summaryDateLbl->setObjectName("summaryDateLbl");
        summaryDateLbl->setGeometry(QRect(21, 16, 1090, 17));
        summaryDateLbl->setFont(font1);
        summaryDateLbl->setTextFormat(Qt::TextFormat::PlainText);
        summaryDateLbl->setWordWrap(true);
        summaryTimeLbl = new QLabel(summaryBox);
        summaryTimeLbl->setObjectName("summaryTimeLbl");
        summaryTimeLbl->setGeometry(QRect(21, 39, 1090, 17));
        summaryTimeLbl->setFont(font);
        summaryTimeLbl->setTextFormat(Qt::TextFormat::PlainText);
        summaryTimeLbl->setWordWrap(true);
        formScrollArea = new QScrollArea(formWidget);
        formScrollArea->setObjectName("formScrollArea");
        formScrollArea->setGeometry(QRect(24, 214, 1132, 476));
        formScrollArea->setFont(font);
        formScrollArea->setFrameShape(QFrame::Shape::NoFrame);
        formScrollArea->setWidgetResizable(false);
        formScrollContents = new QWidget();
        formScrollContents->setObjectName("formScrollContents");
        formScrollContents->setGeometry(QRect(0, 0, 1132, 476));
        formScrollContents->setMinimumSize(QSize(1132, 476));
        formScrollContents->setFont(font);
        label = new QLabel(formScrollContents);
        label->setObjectName("label");
        label->setGeometry(QRect(9, 9, 545, 21));
        label->setFont(font);
        label->setTextFormat(Qt::TextFormat::PlainText);
        nameInput = new QLineEdit(formScrollContents);
        nameInput->setObjectName("nameInput");
        nameInput->setGeometry(QRect(9, 40, 545, 37));
        nameInput->setFont(font);
        nameInput->setMaxLength(254);
        nameInput->setClearButtonEnabled(true);
        label_2 = new QLabel(formScrollContents);
        label_2->setObjectName("label_2");
        label_2->setGeometry(QRect(9, 87, 545, 21));
        label_2->setFont(font);
        label_2->setTextFormat(Qt::TextFormat::PlainText);
        indexInput = new QLineEdit(formScrollContents);
        indexInput->setObjectName("indexInput");
        indexInput->setGeometry(QRect(9, 118, 545, 37));
        indexInput->setFont(font);
        indexInput->setMaxLength(254);
        indexInput->setClearButtonEnabled(true);
        label_3 = new QLabel(formScrollContents);
        label_3->setObjectName("label_3");
        label_3->setGeometry(QRect(9, 165, 545, 21));
        label_3->setFont(font);
        label_3->setTextFormat(Qt::TextFormat::PlainText);
        deptInput = new QComboBox(formScrollContents);
        deptInput->addItem(QString());
        deptInput->addItem(QString());
        deptInput->addItem(QString());
        deptInput->addItem(QString());
        deptInput->addItem(QString());
        deptInput->setObjectName("deptInput");
        deptInput->setGeometry(QRect(9, 196, 545, 37));
        deptInput->setFont(font);
        deptInput->setSizeAdjustPolicy(QComboBox::SizeAdjustPolicy::AdjustToMinimumContentsLengthWithIcon);
        deptInput->setMinimumContentsLength(24);
        label_email = new QLabel(formScrollContents);
        label_email->setObjectName("label_email");
        label_email->setGeometry(QRect(9, 243, 545, 21));
        label_email->setFont(font);
        label_email->setTextFormat(Qt::TextFormat::PlainText);
        emailInput = new QLineEdit(formScrollContents);
        emailInput->setObjectName("emailInput");
        emailInput->setGeometry(QRect(9, 274, 545, 37));
        emailInput->setFont(font);
        emailInput->setMaxLength(254);
        emailInput->setClearButtonEnabled(true);
        label_supervisor = new QLabel(formScrollContents);
        label_supervisor->setObjectName("label_supervisor");
        label_supervisor->setGeometry(QRect(9, 321, 545, 21));
        label_supervisor->setFont(font);
        label_supervisor->setTextFormat(Qt::TextFormat::PlainText);
        supervisorInput = new QLineEdit(formScrollContents);
        supervisorInput->setObjectName("supervisorInput");
        supervisorInput->setGeometry(QRect(9, 352, 545, 37));
        supervisorInput->setFont(font);
        supervisorInput->setMaxLength(254);
        supervisorInput->setClearButtonEnabled(true);
        label_project = new QLabel(formScrollContents);
        label_project->setObjectName("label_project");
        label_project->setGeometry(QRect(9, 399, 545, 21));
        label_project->setFont(font);
        label_project->setTextFormat(Qt::TextFormat::PlainText);
        projectInput = new QLineEdit(formScrollContents);
        projectInput->setObjectName("projectInput");
        projectInput->setGeometry(QRect(9, 430, 545, 37));
        projectInput->setFont(font);
        projectInput->setMaxLength(254);
        projectInput->setClearButtonEnabled(true);
        label_6 = new QLabel(formScrollContents);
        label_6->setObjectName("label_6");
        label_6->setGeometry(QRect(578, 9, 545, 21));
        label_6->setFont(font);
        label_6->setTextFormat(Qt::TextFormat::PlainText);
        descriptionInput = new QTextEdit(formScrollContents);
        descriptionInput->setObjectName("descriptionInput");
        descriptionInput->setGeometry(QRect(578, 40, 545, 427));
        descriptionInput->setFont(font);
        descriptionInput->setTabChangesFocus(true);
        descriptionInput->setAcceptRichText(false);
        formScrollArea->setWidget(formScrollContents);
        deleteBtn = new QPushButton(formWidget);
        deleteBtn->setObjectName("deleteBtn");
        deleteBtn->setGeometry(QRect(24, 706, 558, 50));
        deleteBtn->setFont(font1);
        deleteBtn->setCursor(QCursor(Qt::CursorShape::PointingHandCursor));
        deleteBtn->setVisible(false);
        updateBtn = new QPushButton(formWidget);
        updateBtn->setObjectName("updateBtn");
        updateBtn->setGeometry(QRect(597, 706, 559, 50));
        updateBtn->setFont(font1);
        updateBtn->setCursor(QCursor(Qt::CursorShape::PointingHandCursor));
        updateBtn->setVisible(false);
        submitBtn = new QPushButton(formWidget);
        submitBtn->setObjectName("submitBtn");
        submitBtn->setGeometry(QRect(24, 706, 1132, 50));
        submitBtn->setFont(font1);
        submitBtn->setCursor(QCursor(Qt::CursorShape::PointingHandCursor));
#if QT_CONFIG(shortcut)
        label->setBuddy(nameInput);
        label_2->setBuddy(indexInput);
        label_3->setBuddy(deptInput);
        label_email->setBuddy(emailInput);
        label_supervisor->setBuddy(supervisorInput);
        label_project->setBuddy(projectInput);
        label_6->setBuddy(descriptionInput);
#endif // QT_CONFIG(shortcut)
        QWidget::setTabOrder(backTo2Btn, nameInput);
        QWidget::setTabOrder(nameInput, indexInput);
        QWidget::setTabOrder(indexInput, deptInput);
        QWidget::setTabOrder(deptInput, emailInput);
        QWidget::setTabOrder(emailInput, supervisorInput);
        QWidget::setTabOrder(supervisorInput, projectInput);
        QWidget::setTabOrder(projectInput, descriptionInput);
        QWidget::setTabOrder(descriptionInput, submitBtn);
        QWidget::setTabOrder(submitBtn, updateBtn);
        QWidget::setTabOrder(updateBtn, deleteBtn);

        retranslateUi(formWidget);

        deptInput->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(formWidget);
    } // setupUi

    void retranslateUi(QWidget *formWidget)
    {
        backTo2Btn->setText(QCoreApplication::translate("formWidget", "Back to Time Slot", nullptr));
        confirmTitleLbl->setText(QCoreApplication::translate("formWidget", "Finalize your allocation", nullptr));
        summaryDateLbl->setText(QCoreApplication::translate("formWidget", "Date: ", nullptr));
        summaryTimeLbl->setText(QCoreApplication::translate("formWidget", "Time: ", nullptr));
        label->setText(QCoreApplication::translate("formWidget", "Full name *", nullptr));
        nameInput->setPlaceholderText(QCoreApplication::translate("formWidget", "Enter your full name", nullptr));
        label_2->setText(QCoreApplication::translate("formWidget", "Student index *", nullptr));
        indexInput->setPlaceholderText(QCoreApplication::translate("formWidget", "Enter your student index number", nullptr));
        label_3->setText(QCoreApplication::translate("formWidget", "Department *", nullptr));
        deptInput->setItemText(0, QCoreApplication::translate("formWidget", "Select department", nullptr));
        deptInput->setItemText(1, QCoreApplication::translate("formWidget", "Department of Construction Technology", nullptr));
        deptInput->setItemText(2, QCoreApplication::translate("formWidget", "Department of Electrotechnology", nullptr));
        deptInput->setItemText(3, QCoreApplication::translate("formWidget", "Department of Mechanical and Manufacturing Technology", nullptr));
        deptInput->setItemText(4, QCoreApplication::translate("formWidget", "Department of Nano Science Technology", nullptr));

        label_email->setText(QCoreApplication::translate("formWidget", "Email address *", nullptr));
        emailInput->setPlaceholderText(QCoreApplication::translate("formWidget", "name@university.edu", nullptr));
        label_supervisor->setText(QCoreApplication::translate("formWidget", "Supervisor name *", nullptr));
        supervisorInput->setPlaceholderText(QCoreApplication::translate("formWidget", "Enter the name of your supervisor", nullptr));
        label_project->setText(QCoreApplication::translate("formWidget", "Project name *", nullptr));
        projectInput->setPlaceholderText(QCoreApplication::translate("formWidget", "Enter the title of your project", nullptr));
        label_6->setText(QCoreApplication::translate("formWidget", "Description (optional)", nullptr));
        descriptionInput->setPlaceholderText(QCoreApplication::translate("formWidget", "Describe the purpose of your allocation and any equipment you need (optional).", nullptr));
        deleteBtn->setText(QCoreApplication::translate("formWidget", "Delete Allocation", nullptr));
        updateBtn->setText(QCoreApplication::translate("formWidget", "Update Allocation", nullptr));
        submitBtn->setText(QCoreApplication::translate("formWidget", "Confirm Allocation", nullptr));
        (void)formWidget;
    } // retranslateUi

};

namespace Ui {
    class formWidget: public Ui_formWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_FORM_H
