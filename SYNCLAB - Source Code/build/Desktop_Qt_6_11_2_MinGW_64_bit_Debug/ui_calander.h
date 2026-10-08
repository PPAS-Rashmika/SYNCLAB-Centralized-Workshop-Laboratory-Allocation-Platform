/********************************************************************************
** Form generated from reading UI file 'calander.ui'
**
** Created by: Qt User Interface Compiler version 6.11.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CALANDER_H
#define UI_CALANDER_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QWidget>
#include "customcalendarwidget.h"

QT_BEGIN_NAMESPACE

class Ui_calanderWidget
{
public:
    QPushButton *backTo1Btn;
    QLabel *selectedImgLabel2;
    QLabel *selectedNameLabel2;
    QLabel *selectedDescLabel2;
    QPushButton *adminBtn2;
    CustomCalendarWidget *calendar;
    QLabel *calendarLegend;
    QLabel *timeSlotsLabel;
    QPushButton *timeBtn1;
    QPushButton *timeBtn2;
    QPushButton *timeBtn3;
    QPushButton *timeBtn4;
    QPushButton *nextTo3Btn;

    void setupUi(QWidget *calanderWidget)
    {
        if (calanderWidget->objectName().isEmpty())
            calanderWidget->setObjectName("calanderWidget");
        calanderWidget->resize(1180, 780);
        QFont font;
        font.setFamilies({QString::fromUtf8("Segoe UI")});
        font.setPointSize(10);
        font.setBold(false);
        calanderWidget->setFont(font);
        calanderWidget->setStyleSheet(QString::fromUtf8("QWidget { color:#17243B;   }QMainWindow, QStackedWidget, QWidget#welcomeWidget, QWidget#selectionWidget, QWidget#calanderWidget, QWidget#formWidget, QWidget#adminWidget { background:#F4F7FB; }QScrollArea { border:none; background:transparent; } QScrollArea > QWidget > QWidget { background:#F4F7FB; }QScrollArea#cardsScrollArea > QWidget, QWidget#cardsScrollContents { background:transparent; }QLabel { background:transparent; }QLabel#welcomeTitle { color:#52647D;   }QLabel#syncLabTitle {   }QLabel#syncLabSubtitle {   }QLabel#welcomeDesc, QLabel#selectedDescLabel2, QLabel#adminWsDescLbl { color:#52647D;  }QLabel#selectTitleLabel, QLabel#selectedNameLabel2, QLabel#confirmTitleLbl, QLabel#adminWsTitleLbl {   }QLineEdit, QTextEdit, QComboBox { background:#FFFFFF; border:1px solid #D4DEEB; border-radius:7px; padding:8px; selection-background-color:#2563EB; selection-color:white; }QLineEdit:focus, QTextEdit:focus, QComboBox:focus { border:1px solid #2563EB; }QComboBox QAbstractItemView { background:#FFFFFF; color:#1724"
                        "3B; selection-background-color:#2563EB; selection-color:white; }QPushButton { background:#FFFFFF; color:#17243B; border:1px solid #D4DEEB; border-radius:8px; padding:9px 14px;  }QPushButton:hover, QPushButton:focus { background:#EAF2FF; border-color:#2563EB; }QPushButton:pressed { border:2px solid #2563EB; }QPushButton#allocateNowBtn, QPushButton#nextBtn, QPushButton#nextTo3Btn, QPushButton#submitBtn, QPushButton#updateBtn { background:#2563EB; color:white; border:1px solid #2563EB; }QPushButton#allocateNowBtn:hover, QPushButton#nextBtn:hover, QPushButton#nextTo3Btn:hover, QPushButton#submitBtn:hover, QPushButton#updateBtn:hover { background:#1D4ED8; }QPushButton#deleteBtn, QPushButton#adminDelBtn { color:#BE123C; }QPushButton[timeSlot=true] { padding:10px 8px; }QPushButton:checked { background:#2563EB; color:white; border-color:#2563EB; }QPushButton:disabled { background:#F4F7FB; color:#52647D; border:1px solid #D4DEEB; }QPushButton#allocateNowBtn:disabled, QPushButton#nextBtn:disabled, QPushButton#nextTo3Btn"
                        ":disabled, QPushButton#submitBtn:disabled, QPushButton#updateBtn:disabled, QPushButton#deleteBtn:disabled, QPushButton#adminDelBtn:disabled { background:#F4F7FB; color:#52647D; border:1px solid #D4DEEB; }QFrame#summaryBox { background:#EAF2FF; border:1px solid #D4DEEB; border-radius:10px; }QLabel#summaryDateLbl { color:#2563EB;  }QTableWidget { background:#FFFFFF; alternate-background-color:#F4F7FB; border:1px solid #D4DEEB; gridline-color:#D4DEEB; selection-background-color:#EAF2FF; selection-color:#17243B; }QHeaderView::section { font-weight:600; background:#EAF2FF; color:#17243B; border:none; padding:10px;  }QTableWidget QLineEdit, QTableWidget QComboBox { padding:4px; border-radius:4px; }QCalendarWidget { background:#FFFFFF; border:1px solid #D4DEEB; border-radius:8px; }QCalendarWidget QWidget#qt_calendar_navigationbar { background:#2563EB; }QCalendarWidget QToolButton { font-weight:600; color:white; background:transparent; padding:6px;  }QCalendarWidget QMenu, QCalendarWidget QSpinBox { background:#FFFFFF"
                        "; color:#17243B; }QCalendarWidget QAbstractItemView { background:#FFFFFF; color:#17243B; selection-background-color:#2563EB; selection-color:white; outline:0; }QToolTip { background:#FFFFFF; color:#17243B; border:1px solid #D4DEEB; padding:6px; }"));
        backTo1Btn = new QPushButton(calanderWidget);
        backTo1Btn->setObjectName("backTo1Btn");
        backTo1Btn->setGeometry(QRect(24, 24, 171, 37));
        QFont font1;
        font1.setFamilies({QString::fromUtf8("Segoe UI")});
        font1.setPointSize(10);
        font1.setBold(true);
        backTo1Btn->setFont(font1);
        backTo1Btn->setCursor(QCursor(Qt::CursorShape::PointingHandCursor));
        selectedImgLabel2 = new QLabel(calanderWidget);
        selectedImgLabel2->setObjectName("selectedImgLabel2");
        selectedImgLabel2->setGeometry(QRect(24, 290, 251, 160));
        selectedImgLabel2->setFont(font);
        selectedImgLabel2->setTextFormat(Qt::TextFormat::PlainText);
        selectedImgLabel2->setAlignment(Qt::AlignmentFlag::AlignCenter);
        selectedNameLabel2 = new QLabel(calanderWidget);
        selectedNameLabel2->setObjectName("selectedNameLabel2");
        selectedNameLabel2->setGeometry(QRect(24, 480, 369, 51));
        QFont font2;
        font2.setFamilies({QString::fromUtf8("Segoe UI")});
        font2.setPointSize(19);
        font2.setBold(true);
        selectedNameLabel2->setFont(font2);
        selectedNameLabel2->setTextFormat(Qt::TextFormat::PlainText);
        selectedNameLabel2->setAlignment(Qt::AlignmentFlag::AlignLeading|Qt::AlignmentFlag::AlignLeft|Qt::AlignmentFlag::AlignTop);
        selectedNameLabel2->setWordWrap(true);
        selectedDescLabel2 = new QLabel(calanderWidget);
        selectedDescLabel2->setObjectName("selectedDescLabel2");
        selectedDescLabel2->setGeometry(QRect(24, 531, 369, 105));
        QFont font3;
        font3.setFamilies({QString::fromUtf8("Segoe UI")});
        font3.setPointSize(11);
        font3.setBold(false);
        selectedDescLabel2->setFont(font3);
        selectedDescLabel2->setTextFormat(Qt::TextFormat::PlainText);
        selectedDescLabel2->setAlignment(Qt::AlignmentFlag::AlignLeading|Qt::AlignmentFlag::AlignLeft|Qt::AlignmentFlag::AlignTop);
        selectedDescLabel2->setWordWrap(true);
        adminBtn2 = new QPushButton(calanderWidget);
        adminBtn2->setObjectName("adminBtn2");
        adminBtn2->setGeometry(QRect(24, 711, 369, 45));
        adminBtn2->setFont(font1);
        adminBtn2->setCursor(QCursor(Qt::CursorShape::PointingHandCursor));
        calendar = new CustomCalendarWidget(calanderWidget);
        calendar->setObjectName("calendar");
        calendar->setGeometry(QRect(417, 24, 739, 487));
        calendar->setFont(font);
        calendar->setVerticalHeaderFormat(QCalendarWidget::VerticalHeaderFormat::NoVerticalHeader);
        calendarLegend = new QLabel(calanderWidget);
        calendarLegend->setObjectName("calendarLegend");
        calendarLegend->setGeometry(QRect(417, 525, 739, 17));
        calendarLegend->setFont(font);
        calendarLegend->setWordWrap(true);
        timeSlotsLabel = new QLabel(calanderWidget);
        timeSlotsLabel->setObjectName("timeSlotsLabel");
        timeSlotsLabel->setGeometry(QRect(417, 556, 739, 17));
        timeSlotsLabel->setFont(font);
        timeSlotsLabel->setWordWrap(true);
        timeBtn1 = new QPushButton(calanderWidget);
        timeBtn1->setObjectName("timeBtn1");
        timeBtn1->setGeometry(QRect(417, 587, 362, 45));
        timeBtn1->setFont(font1);
        timeBtn1->setCursor(QCursor(Qt::CursorShape::PointingHandCursor));
        timeBtn1->setCheckable(true);
        timeBtn1->setProperty("timeSlot", QVariant(true));
        timeBtn2 = new QPushButton(calanderWidget);
        timeBtn2->setObjectName("timeBtn2");
        timeBtn2->setGeometry(QRect(794, 587, 362, 45));
        timeBtn2->setFont(font1);
        timeBtn2->setCursor(QCursor(Qt::CursorShape::PointingHandCursor));
        timeBtn2->setCheckable(true);
        timeBtn2->setProperty("timeSlot", QVariant(true));
        timeBtn3 = new QPushButton(calanderWidget);
        timeBtn3->setObjectName("timeBtn3");
        timeBtn3->setGeometry(QRect(417, 647, 362, 45));
        timeBtn3->setFont(font1);
        timeBtn3->setCursor(QCursor(Qt::CursorShape::PointingHandCursor));
        timeBtn3->setCheckable(true);
        timeBtn3->setProperty("timeSlot", QVariant(true));
        timeBtn4 = new QPushButton(calanderWidget);
        timeBtn4->setObjectName("timeBtn4");
        timeBtn4->setGeometry(QRect(794, 647, 362, 45));
        timeBtn4->setFont(font1);
        timeBtn4->setCursor(QCursor(Qt::CursorShape::PointingHandCursor));
        timeBtn4->setCheckable(true);
        timeBtn4->setProperty("timeSlot", QVariant(true));
        nextTo3Btn = new QPushButton(calanderWidget);
        nextTo3Btn->setObjectName("nextTo3Btn");
        nextTo3Btn->setGeometry(QRect(417, 706, 739, 50));
        nextTo3Btn->setFont(font1);
        nextTo3Btn->setCursor(QCursor(Qt::CursorShape::PointingHandCursor));

        retranslateUi(calanderWidget);

        QMetaObject::connectSlotsByName(calanderWidget);
    } // setupUi

    void retranslateUi(QWidget *calanderWidget)
    {
        backTo1Btn->setText(QCoreApplication::translate("calanderWidget", "Back to Selection", nullptr));
        selectedNameLabel2->setText(QCoreApplication::translate("calanderWidget", "Workshop Name", nullptr));
        selectedDescLabel2->setText(QCoreApplication::translate("calanderWidget", "Select a date, then choose one or more available time slots to continue.", nullptr));
        adminBtn2->setText(QCoreApplication::translate("calanderWidget", "Admin dashboard", nullptr));
        calendarLegend->setText(QCoreApplication::translate("calanderWidget", "Allocation counts: A = approved \302\267 P = pending \302\267 R = rejected", nullptr));
        timeSlotsLabel->setText(QCoreApplication::translate("calanderWidget", "Available time slots \302\267 select one or more", nullptr));
#if QT_CONFIG(tooltip)
        timeBtn1->setToolTip(QCoreApplication::translate("calanderWidget", "Choose this time slot. You may select more than one.", nullptr));
#endif // QT_CONFIG(tooltip)
        timeBtn1->setText(QCoreApplication::translate("calanderWidget", "8.30 AM - 10.30 AM", nullptr));
#if QT_CONFIG(tooltip)
        timeBtn2->setToolTip(QCoreApplication::translate("calanderWidget", "Choose this time slot. You may select more than one.", nullptr));
#endif // QT_CONFIG(tooltip)
        timeBtn2->setText(QCoreApplication::translate("calanderWidget", "10.30 AM - 12.30 PM", nullptr));
#if QT_CONFIG(tooltip)
        timeBtn3->setToolTip(QCoreApplication::translate("calanderWidget", "Choose this time slot. You may select more than one.", nullptr));
#endif // QT_CONFIG(tooltip)
        timeBtn3->setText(QCoreApplication::translate("calanderWidget", "1.30 PM - 3.30 PM", nullptr));
#if QT_CONFIG(tooltip)
        timeBtn4->setToolTip(QCoreApplication::translate("calanderWidget", "Choose this time slot. You may select more than one.", nullptr));
#endif // QT_CONFIG(tooltip)
        timeBtn4->setText(QCoreApplication::translate("calanderWidget", "3.30 PM - 5.30 PM", nullptr));
        nextTo3Btn->setText(QCoreApplication::translate("calanderWidget", "Next Step", nullptr));
        (void)calanderWidget;
    } // retranslateUi

};

namespace Ui {
    class calanderWidget: public Ui_calanderWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CALANDER_H
