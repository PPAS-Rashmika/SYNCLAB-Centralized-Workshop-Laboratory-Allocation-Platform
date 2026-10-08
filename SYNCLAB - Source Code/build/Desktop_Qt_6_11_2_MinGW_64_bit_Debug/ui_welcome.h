/********************************************************************************
** Form generated from reading UI file 'welcome.ui'
**
** Created by: Qt User Interface Compiler version 6.11.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_WELCOME_H
#define UI_WELCOME_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_welcomeWidget
{
public:
    QLabel *welcomeImg;
    QLabel *welcomeTitle;
    QLabel *syncLabTitle;
    QLabel *syncLabSubtitle;
    QLabel *welcomeDesc;
    QPushButton *allocateNowBtn;
    QPushButton *editAllocationBtn;
    QPushButton *settingsBtn0;

    void setupUi(QWidget *welcomeWidget)
    {
        if (welcomeWidget->objectName().isEmpty())
            welcomeWidget->setObjectName("welcomeWidget");
        welcomeWidget->resize(1180, 780);
        QFont font;
        font.setFamilies({QString::fromUtf8("Segoe UI")});
        font.setPointSize(10);
        font.setBold(false);
        welcomeWidget->setFont(font);
        welcomeWidget->setStyleSheet(QString::fromUtf8("QWidget { color:#17243B;   }QMainWindow, QStackedWidget, QWidget#welcomeWidget, QWidget#selectionWidget, QWidget#calanderWidget, QWidget#formWidget, QWidget#adminWidget { background:#F4F7FB; }QScrollArea { border:none; background:transparent; } QScrollArea > QWidget > QWidget { background:#F4F7FB; }QScrollArea#cardsScrollArea > QWidget, QWidget#cardsScrollContents { background:transparent; }QLabel { background:transparent; }QLabel#welcomeTitle { color:#52647D;   }QLabel#syncLabTitle {   }QLabel#syncLabSubtitle {   }QLabel#welcomeDesc, QLabel#selectedDescLabel2, QLabel#adminWsDescLbl { color:#52647D;  }QLabel#selectTitleLabel, QLabel#selectedNameLabel2, QLabel#confirmTitleLbl, QLabel#adminWsTitleLbl {   }QLineEdit, QTextEdit, QComboBox { background:#FFFFFF; border:1px solid #D4DEEB; border-radius:7px; padding:8px; selection-background-color:#2563EB; selection-color:white; }QLineEdit:focus, QTextEdit:focus, QComboBox:focus { border:1px solid #2563EB; }QComboBox QAbstractItemView { background:#FFFFFF; color:#1724"
                        "3B; selection-background-color:#2563EB; selection-color:white; }QPushButton { background:#FFFFFF; color:#17243B; border:1px solid #D4DEEB; border-radius:8px; padding:9px 14px;  }QPushButton:hover, QPushButton:focus { background:#EAF2FF; border-color:#2563EB; }QPushButton:pressed { border:2px solid #2563EB; }QPushButton#allocateNowBtn, QPushButton#nextBtn, QPushButton#nextTo3Btn, QPushButton#submitBtn, QPushButton#updateBtn { background:#2563EB; color:white; border:1px solid #2563EB; }QPushButton#allocateNowBtn:hover, QPushButton#nextBtn:hover, QPushButton#nextTo3Btn:hover, QPushButton#submitBtn:hover, QPushButton#updateBtn:hover { background:#1D4ED8; }QPushButton#deleteBtn, QPushButton#adminDelBtn { color:#BE123C; }QPushButton[timeSlot=true] { padding:10px 8px; }QPushButton:checked { background:#2563EB; color:white; border-color:#2563EB; }QPushButton:disabled { background:#F4F7FB; color:#52647D; border:1px solid #D4DEEB; }QPushButton#allocateNowBtn:disabled, QPushButton#nextBtn:disabled, QPushButton#nextTo3Btn"
                        ":disabled, QPushButton#submitBtn:disabled, QPushButton#updateBtn:disabled, QPushButton#deleteBtn:disabled, QPushButton#adminDelBtn:disabled { background:#F4F7FB; color:#52647D; border:1px solid #D4DEEB; }QFrame#summaryBox { background:#EAF2FF; border:1px solid #D4DEEB; border-radius:10px; }QLabel#summaryDateLbl { color:#2563EB;  }QTableWidget { background:#FFFFFF; alternate-background-color:#F4F7FB; border:1px solid #D4DEEB; gridline-color:#D4DEEB; selection-background-color:#EAF2FF; selection-color:#17243B; }QHeaderView::section { font-weight:600; background:#EAF2FF; color:#17243B; border:none; padding:10px;  }QTableWidget QLineEdit, QTableWidget QComboBox { padding:4px; border-radius:4px; }QCalendarWidget { background:#FFFFFF; border:1px solid #D4DEEB; border-radius:8px; }QCalendarWidget QWidget#qt_calendar_navigationbar { background:#2563EB; }QCalendarWidget QToolButton { font-weight:600; color:white; background:transparent; padding:6px;  }QCalendarWidget QMenu, QCalendarWidget QSpinBox { background:#FFFFFF"
                        "; color:#17243B; }QCalendarWidget QAbstractItemView { background:#FFFFFF; color:#17243B; selection-background-color:#2563EB; selection-color:white; outline:0; }QToolTip { background:#FFFFFF; color:#17243B; border:1px solid #D4DEEB; padding:6px; }"));
        welcomeImg = new QLabel(welcomeWidget);
        welcomeImg->setObjectName("welcomeImg");
        welcomeImg->setGeometry(QRect(518, 200, 144, 143));
        welcomeImg->setFont(font);
        welcomeImg->setTextFormat(Qt::TextFormat::PlainText);
        welcomeImg->setPixmap(QPixmap(QString::fromUtf8(":/branding/logo.png")));
        welcomeImg->setScaledContents(true);
        welcomeImg->setAlignment(Qt::AlignmentFlag::AlignCenter);
        welcomeTitle = new QLabel(welcomeWidget);
        welcomeTitle->setObjectName("welcomeTitle");
        welcomeTitle->setGeometry(QRect(24, 350, 1132, 21));
        QFont font1;
        font1.setFamilies({QString::fromUtf8("Segoe UI")});
        font1.setPointSize(12);
        font1.setBold(true);
        welcomeTitle->setFont(font1);
        welcomeTitle->setTextFormat(Qt::TextFormat::PlainText);
        welcomeTitle->setAlignment(Qt::AlignmentFlag::AlignCenter);
        syncLabTitle = new QLabel(welcomeWidget);
        syncLabTitle->setObjectName("syncLabTitle");
        syncLabTitle->setGeometry(QRect(24, 380, 1132, 69));
        QFont font2;
        font2.setFamilies({QString::fromUtf8("Segoe UI")});
        font2.setPointSize(39);
        font2.setBold(true);
        syncLabTitle->setFont(font2);
        syncLabTitle->setTextFormat(Qt::TextFormat::PlainText);
        syncLabTitle->setAlignment(Qt::AlignmentFlag::AlignCenter);
        syncLabSubtitle = new QLabel(welcomeWidget);
        syncLabSubtitle->setObjectName("syncLabSubtitle");
        syncLabSubtitle->setGeometry(QRect(24, 458, 1132, 27));
        QFont font3;
        font3.setFamilies({QString::fromUtf8("Segoe UI")});
        font3.setPointSize(15);
        font3.setBold(true);
        syncLabSubtitle->setFont(font3);
        syncLabSubtitle->setTextFormat(Qt::TextFormat::PlainText);
        syncLabSubtitle->setAlignment(Qt::AlignmentFlag::AlignCenter);
        syncLabSubtitle->setWordWrap(true);
        welcomeDesc = new QLabel(welcomeWidget);
        welcomeDesc->setObjectName("welcomeDesc");
        welcomeDesc->setGeometry(QRect(24, 501, 1132, 38));
        QFont font4;
        font4.setFamilies({QString::fromUtf8("Segoe UI")});
        font4.setPointSize(11);
        font4.setBold(false);
        welcomeDesc->setFont(font4);
        welcomeDesc->setTextFormat(Qt::TextFormat::PlainText);
        welcomeDesc->setAlignment(Qt::AlignmentFlag::AlignCenter);
        welcomeDesc->setWordWrap(true);
        allocateNowBtn = new QPushButton(welcomeWidget);
        allocateNowBtn->setObjectName("allocateNowBtn");
        allocateNowBtn->setGeometry(QRect(319, 555, 170, 56));
        QFont font5;
        font5.setFamilies({QString::fromUtf8("Segoe UI")});
        font5.setPointSize(10);
        font5.setBold(true);
        allocateNowBtn->setFont(font5);
        allocateNowBtn->setCursor(QCursor(Qt::CursorShape::PointingHandCursor));
        editAllocationBtn = new QPushButton(welcomeWidget);
        editAllocationBtn->setObjectName("editAllocationBtn");
        editAllocationBtn->setGeometry(QRect(505, 555, 170, 56));
        editAllocationBtn->setFont(font5);
        editAllocationBtn->setCursor(QCursor(Qt::CursorShape::PointingHandCursor));
        settingsBtn0 = new QPushButton(welcomeWidget);
        settingsBtn0->setObjectName("settingsBtn0");
        settingsBtn0->setGeometry(QRect(691, 555, 170, 56));
        settingsBtn0->setFont(font5);
        settingsBtn0->setCursor(QCursor(Qt::CursorShape::PointingHandCursor));

        retranslateUi(welcomeWidget);

        QMetaObject::connectSlotsByName(welcomeWidget);
    } // setupUi

    void retranslateUi(QWidget *welcomeWidget)
    {
#if QT_CONFIG(accessibility)
        welcomeImg->setAccessibleName(QCoreApplication::translate("welcomeWidget", "SYNCLAB logo", nullptr));
#endif // QT_CONFIG(accessibility)
        welcomeTitle->setText(QCoreApplication::translate("welcomeWidget", "WELCOME", nullptr));
        syncLabTitle->setText(QCoreApplication::translate("welcomeWidget", "SYNCLAB", nullptr));
        syncLabSubtitle->setText(QCoreApplication::translate("welcomeWidget", "Centralized Workshop & Laboratory Allocation Platform", nullptr));
        welcomeDesc->setText(QCoreApplication::translate("welcomeWidget", "Manage the university workshop and laboratory allocations efficiently.\n"
"Please choose an option below to get started with your session allocations.", nullptr));
        allocateNowBtn->setText(QCoreApplication::translate("welcomeWidget", "New allocation", nullptr));
        editAllocationBtn->setText(QCoreApplication::translate("welcomeWidget", "Edit allocation", nullptr));
        settingsBtn0->setText(QCoreApplication::translate("welcomeWidget", "Settings", nullptr));
        (void)welcomeWidget;
    } // retranslateUi

};

namespace Ui {
    class welcomeWidget: public Ui_welcomeWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_WELCOME_H
