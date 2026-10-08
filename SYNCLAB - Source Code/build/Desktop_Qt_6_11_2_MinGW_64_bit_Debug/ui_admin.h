/********************************************************************************
** Form generated from reading UI file 'admin.ui'
**
** Created by: Qt User Interface Compiler version 6.11.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_ADMIN_H
#define UI_ADMIN_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QFrame>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QWidget>
#include "customcalendarwidget.h"

QT_BEGIN_NAMESPACE

class Ui_adminWidget
{
public:
    QPushButton *adminBackBtn;
    QLabel *adminImgLabel;
    QLabel *adminWsTitleLbl;
    QLabel *adminWsDescLbl;
    QPushButton *adminSettingsBtn;
    QPushButton *adminShowAllBtn;
    QFrame *adminCalFrame;
    CustomCalendarWidget *adminCalendar;
    QPushButton *adminEditBtn;
    QPushButton *adminDelBtn;
    QPushButton *pendingEmailBtn;
    QPushButton *rejectEmailBtn;
    QTableWidget *mainTable;

    void setupUi(QWidget *adminWidget)
    {
        if (adminWidget->objectName().isEmpty())
            adminWidget->setObjectName("adminWidget");
        adminWidget->resize(1180, 780);
        QFont font;
        font.setFamilies({QString::fromUtf8("Segoe UI")});
        font.setPointSize(10);
        font.setBold(false);
        adminWidget->setFont(font);
        adminWidget->setStyleSheet(QString::fromUtf8("QWidget { color:#17243B;   }QMainWindow, QStackedWidget, QWidget#welcomeWidget, QWidget#selectionWidget, QWidget#calanderWidget, QWidget#formWidget, QWidget#adminWidget { background:#F4F7FB; }QScrollArea { border:none; background:transparent; } QScrollArea > QWidget > QWidget { background:#F4F7FB; }QScrollArea#cardsScrollArea > QWidget, QWidget#cardsScrollContents { background:transparent; }QLabel { background:transparent; }QLabel#welcomeTitle { color:#52647D;   }QLabel#syncLabTitle {   }QLabel#syncLabSubtitle {   }QLabel#welcomeDesc, QLabel#selectedDescLabel2, QLabel#adminWsDescLbl { color:#52647D;  }QLabel#selectTitleLabel, QLabel#selectedNameLabel2, QLabel#confirmTitleLbl, QLabel#adminWsTitleLbl {   }QLineEdit, QTextEdit, QComboBox { background:#FFFFFF; border:1px solid #D4DEEB; border-radius:7px; padding:8px; selection-background-color:#2563EB; selection-color:white; }QLineEdit:focus, QTextEdit:focus, QComboBox:focus { border:1px solid #2563EB; }QComboBox QAbstractItemView { background:#FFFFFF; color:#1724"
                        "3B; selection-background-color:#2563EB; selection-color:white; }QPushButton { background:#FFFFFF; color:#17243B; border:1px solid #D4DEEB; border-radius:8px; padding:9px 14px;  }QPushButton:hover, QPushButton:focus { background:#EAF2FF; border-color:#2563EB; }QPushButton:pressed { border:2px solid #2563EB; }QPushButton#allocateNowBtn, QPushButton#nextBtn, QPushButton#nextTo3Btn, QPushButton#submitBtn, QPushButton#updateBtn { background:#2563EB; color:white; border:1px solid #2563EB; }QPushButton#allocateNowBtn:hover, QPushButton#nextBtn:hover, QPushButton#nextTo3Btn:hover, QPushButton#submitBtn:hover, QPushButton#updateBtn:hover { background:#1D4ED8; }QPushButton#deleteBtn, QPushButton#adminDelBtn { color:#BE123C; }QPushButton[timeSlot=true] { padding:10px 8px; }QPushButton:checked { background:#2563EB; color:white; border-color:#2563EB; }QPushButton:disabled { background:#F4F7FB; color:#52647D; border:1px solid #D4DEEB; }QPushButton#allocateNowBtn:disabled, QPushButton#nextBtn:disabled, QPushButton#nextTo3Btn"
                        ":disabled, QPushButton#submitBtn:disabled, QPushButton#updateBtn:disabled, QPushButton#deleteBtn:disabled, QPushButton#adminDelBtn:disabled { background:#F4F7FB; color:#52647D; border:1px solid #D4DEEB; }QFrame#summaryBox { background:#EAF2FF; border:1px solid #D4DEEB; border-radius:10px; }QLabel#summaryDateLbl { color:#2563EB;  }QTableWidget { background:#FFFFFF; alternate-background-color:#F4F7FB; border:1px solid #D4DEEB; gridline-color:#D4DEEB; selection-background-color:#EAF2FF; selection-color:#17243B; }QHeaderView::section { font-weight:600; background:#EAF2FF; color:#17243B; border:none; padding:10px;  }QTableWidget QLineEdit, QTableWidget QComboBox { padding:4px; border-radius:4px; }QCalendarWidget { background:#FFFFFF; border:1px solid #D4DEEB; border-radius:8px; }QCalendarWidget QWidget#qt_calendar_navigationbar { background:#2563EB; }QCalendarWidget QToolButton { font-weight:600; color:white; background:transparent; padding:6px;  }QCalendarWidget QMenu, QCalendarWidget QSpinBox { background:#FFFFFF"
                        "; color:#17243B; }QCalendarWidget QAbstractItemView { background:#FFFFFF; color:#17243B; selection-background-color:#2563EB; selection-color:white; outline:0; }QToolTip { background:#FFFFFF; color:#17243B; border:1px solid #D4DEEB; padding:6px; }"));
        adminBackBtn = new QPushButton(adminWidget);
        adminBackBtn->setObjectName("adminBackBtn");
        adminBackBtn->setGeometry(QRect(24, 24, 181, 37));
        QFont font1;
        font1.setFamilies({QString::fromUtf8("Segoe UI")});
        font1.setPointSize(10);
        font1.setBold(true);
        adminBackBtn->setFont(font1);
        adminBackBtn->setCursor(QCursor(Qt::CursorShape::PointingHandCursor));
        adminImgLabel = new QLabel(adminWidget);
        adminImgLabel->setObjectName("adminImgLabel");
        adminImgLabel->setGeometry(QRect(24, 89, 200, 111));
        adminImgLabel->setFont(font);
        adminImgLabel->setTextFormat(Qt::TextFormat::PlainText);
        adminImgLabel->setAlignment(Qt::AlignmentFlag::AlignCenter);
        adminWsTitleLbl = new QLabel(adminWidget);
        adminWsTitleLbl->setObjectName("adminWsTitleLbl");
        adminWsTitleLbl->setGeometry(QRect(24, 210, 554, 66));
        QFont font2;
        font2.setFamilies({QString::fromUtf8("Segoe UI")});
        font2.setPointSize(19);
        font2.setBold(true);
        adminWsTitleLbl->setFont(font2);
        adminWsTitleLbl->setTextFormat(Qt::TextFormat::PlainText);
        adminWsTitleLbl->setAlignment(Qt::AlignmentFlag::AlignLeading|Qt::AlignmentFlag::AlignLeft|Qt::AlignmentFlag::AlignTop);
        adminWsTitleLbl->setWordWrap(true);
        adminWsDescLbl = new QLabel(adminWidget);
        adminWsDescLbl->setObjectName("adminWsDescLbl");
        adminWsDescLbl->setGeometry(QRect(24, 270, 554, 40));
        QFont font3;
        font3.setFamilies({QString::fromUtf8("Segoe UI")});
        font3.setPointSize(11);
        font3.setBold(false);
        adminWsDescLbl->setFont(font3);
        adminWsDescLbl->setTextFormat(Qt::TextFormat::PlainText);
        adminWsDescLbl->setAlignment(Qt::AlignmentFlag::AlignLeading|Qt::AlignmentFlag::AlignLeft|Qt::AlignmentFlag::AlignTop);
        adminWsDescLbl->setWordWrap(true);
        adminSettingsBtn = new QPushButton(adminWidget);
        adminSettingsBtn->setObjectName("adminSettingsBtn");
        adminSettingsBtn->setGeometry(QRect(217, 24, 141, 37));
        adminSettingsBtn->setFont(font1);
        adminSettingsBtn->setCursor(QCursor(Qt::CursorShape::PointingHandCursor));
        adminShowAllBtn = new QPushButton(adminWidget);
        adminShowAllBtn->setObjectName("adminShowAllBtn");
        adminShowAllBtn->setGeometry(QRect(1022, 343, 134, 37));
        adminShowAllBtn->setFont(font1);
        adminShowAllBtn->setCursor(QCursor(Qt::CursorShape::PointingHandCursor));
        adminCalFrame = new QFrame(adminWidget);
        adminCalFrame->setObjectName("adminCalFrame");
        adminCalFrame->setGeometry(QRect(602, 77, 554, 240));
        adminCalFrame->setFont(font);
        adminCalendar = new CustomCalendarWidget(adminCalFrame);
        adminCalendar->setObjectName("adminCalendar");
        adminCalendar->setGeometry(QRect(0, 0, 554, 240));
        adminCalendar->setFont(font);
        adminCalendar->setVerticalHeaderFormat(QCalendarWidget::VerticalHeaderFormat::NoVerticalHeader);
        adminEditBtn = new QPushButton(adminWidget);
        adminEditBtn->setObjectName("adminEditBtn");
        adminEditBtn->setGeometry(QRect(602, 343, 72, 37));
        adminEditBtn->setFont(font1);
        adminEditBtn->setCursor(QCursor(Qt::CursorShape::PointingHandCursor));
        adminDelBtn = new QPushButton(adminWidget);
        adminDelBtn->setObjectName("adminDelBtn");
        adminDelBtn->setGeometry(QRect(686, 343, 84, 37));
        adminDelBtn->setFont(font1);
        adminDelBtn->setCursor(QCursor(Qt::CursorShape::PointingHandCursor));
        pendingEmailBtn = new QPushButton(adminWidget);
        pendingEmailBtn->setObjectName("pendingEmailBtn");
        pendingEmailBtn->setGeometry(QRect(782, 343, 108, 37));
        pendingEmailBtn->setFont(font1);
        pendingEmailBtn->setCursor(QCursor(Qt::CursorShape::PointingHandCursor));
        rejectEmailBtn = new QPushButton(adminWidget);
        rejectEmailBtn->setObjectName("rejectEmailBtn");
        rejectEmailBtn->setGeometry(QRect(902, 343, 108, 37));
        rejectEmailBtn->setFont(font1);
        rejectEmailBtn->setCursor(QCursor(Qt::CursorShape::PointingHandCursor));
        mainTable = new QTableWidget(adminWidget);
        if (mainTable->columnCount() < 11)
            mainTable->setColumnCount(11);
        QTableWidgetItem *__qtablewidgetitem = new QTableWidgetItem();
        mainTable->setHorizontalHeaderItem(0, __qtablewidgetitem);
        QTableWidgetItem *__qtablewidgetitem1 = new QTableWidgetItem();
        mainTable->setHorizontalHeaderItem(1, __qtablewidgetitem1);
        QTableWidgetItem *__qtablewidgetitem2 = new QTableWidgetItem();
        mainTable->setHorizontalHeaderItem(2, __qtablewidgetitem2);
        QTableWidgetItem *__qtablewidgetitem3 = new QTableWidgetItem();
        mainTable->setHorizontalHeaderItem(3, __qtablewidgetitem3);
        QTableWidgetItem *__qtablewidgetitem4 = new QTableWidgetItem();
        mainTable->setHorizontalHeaderItem(4, __qtablewidgetitem4);
        QTableWidgetItem *__qtablewidgetitem5 = new QTableWidgetItem();
        mainTable->setHorizontalHeaderItem(5, __qtablewidgetitem5);
        QTableWidgetItem *__qtablewidgetitem6 = new QTableWidgetItem();
        mainTable->setHorizontalHeaderItem(6, __qtablewidgetitem6);
        QTableWidgetItem *__qtablewidgetitem7 = new QTableWidgetItem();
        mainTable->setHorizontalHeaderItem(7, __qtablewidgetitem7);
        QTableWidgetItem *__qtablewidgetitem8 = new QTableWidgetItem();
        mainTable->setHorizontalHeaderItem(8, __qtablewidgetitem8);
        QTableWidgetItem *__qtablewidgetitem9 = new QTableWidgetItem();
        mainTable->setHorizontalHeaderItem(9, __qtablewidgetitem9);
        QTableWidgetItem *__qtablewidgetitem10 = new QTableWidgetItem();
        mainTable->setHorizontalHeaderItem(10, __qtablewidgetitem10);
        mainTable->setObjectName("mainTable");
        mainTable->setGeometry(QRect(24, 396, 1132, 360));
        mainTable->setFont(font);
        mainTable->setEditTriggers(QAbstractItemView::EditTrigger::NoEditTriggers);
        mainTable->setAlternatingRowColors(true);
        mainTable->setSelectionMode(QAbstractItemView::SelectionMode::SingleSelection);
        mainTable->setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectRows);
        mainTable->setVerticalScrollMode(QAbstractItemView::ScrollMode::ScrollPerPixel);
        mainTable->setHorizontalScrollMode(QAbstractItemView::ScrollMode::ScrollPerPixel);
        mainTable->setWordWrap(false);
        mainTable->setColumnCount(11);

        retranslateUi(adminWidget);

        QMetaObject::connectSlotsByName(adminWidget);
    } // setupUi

    void retranslateUi(QWidget *adminWidget)
    {
        adminBackBtn->setText(QCoreApplication::translate("adminWidget", " Exit Admin Mode", nullptr));
        adminWsTitleLbl->setText(QCoreApplication::translate("adminWidget", "WORKSHOP", nullptr));
        adminSettingsBtn->setText(QCoreApplication::translate("adminWidget", "Email settings", nullptr));
        adminShowAllBtn->setText(QCoreApplication::translate("adminWidget", "Show All Dates", nullptr));
        adminEditBtn->setText(QCoreApplication::translate("adminWidget", "Edit", nullptr));
        adminDelBtn->setText(QCoreApplication::translate("adminWidget", "Delete", nullptr));
        pendingEmailBtn->setText(QCoreApplication::translate("adminWidget", "Pending", nullptr));
        rejectEmailBtn->setText(QCoreApplication::translate("adminWidget", "Rejection", nullptr));
        QTableWidgetItem *___qtablewidgetitem = mainTable->horizontalHeaderItem(0);
        ___qtablewidgetitem->setText(QCoreApplication::translate("adminWidget", "ALLOCATION KEY", nullptr));
        QTableWidgetItem *___qtablewidgetitem1 = mainTable->horizontalHeaderItem(1);
        ___qtablewidgetitem1->setText(QCoreApplication::translate("adminWidget", "CREATED", nullptr));
        QTableWidgetItem *___qtablewidgetitem2 = mainTable->horizontalHeaderItem(2);
        ___qtablewidgetitem2->setText(QCoreApplication::translate("adminWidget", "INDEX", nullptr));
        QTableWidgetItem *___qtablewidgetitem3 = mainTable->horizontalHeaderItem(3);
        ___qtablewidgetitem3->setText(QCoreApplication::translate("adminWidget", "NAME", nullptr));
        QTableWidgetItem *___qtablewidgetitem4 = mainTable->horizontalHeaderItem(4);
        ___qtablewidgetitem4->setText(QCoreApplication::translate("adminWidget", "DEPARTMENT", nullptr));
        QTableWidgetItem *___qtablewidgetitem5 = mainTable->horizontalHeaderItem(5);
        ___qtablewidgetitem5->setText(QCoreApplication::translate("adminWidget", "EMAIL", nullptr));
        QTableWidgetItem *___qtablewidgetitem6 = mainTable->horizontalHeaderItem(6);
        ___qtablewidgetitem6->setText(QCoreApplication::translate("adminWidget", "ALLOCATION DATE", nullptr));
        QTableWidgetItem *___qtablewidgetitem7 = mainTable->horizontalHeaderItem(7);
        ___qtablewidgetitem7->setText(QCoreApplication::translate("adminWidget", "TIME SLOTS", nullptr));
        QTableWidgetItem *___qtablewidgetitem8 = mainTable->horizontalHeaderItem(8);
        ___qtablewidgetitem8->setText(QCoreApplication::translate("adminWidget", "DESCRIPTION", nullptr));
        QTableWidgetItem *___qtablewidgetitem9 = mainTable->horizontalHeaderItem(9);
        ___qtablewidgetitem9->setText(QCoreApplication::translate("adminWidget", "REMARKS", nullptr));
        QTableWidgetItem *___qtablewidgetitem10 = mainTable->horizontalHeaderItem(10);
        ___qtablewidgetitem10->setText(QCoreApplication::translate("adminWidget", "STATUS", nullptr));
        (void)adminWidget;
    } // retranslateUi

};

namespace Ui {
    class adminWidget: public Ui_adminWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ADMIN_H
