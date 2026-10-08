/********************************************************************************
** Form generated from reading UI file 'selection.ui'
**
** Created by: Qt User Interface Compiler version 6.11.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SELECTION_H
#define UI_SELECTION_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QFrame>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_selectionWidget
{
public:
    QPushButton *backTo0Btn;
    QLabel *selectTitleLabel;
    QLabel *selectionHintLabel;
    QScrollArea *cardsScrollArea;
    QWidget *cardsScrollContents;
    QFrame *cardFrame01;
    QLabel *workshopImage01;
    QLabel *workshopTitle01;
    QLabel *workshopDepartment01;
    QPushButton *workshopCard_Biochemistry;
    QFrame *cardFrame02;
    QLabel *workshopImage02;
    QLabel *workshopTitle02;
    QLabel *workshopDepartment02;
    QPushButton *workshopCard_Electronics;
    QFrame *cardFrame03;
    QLabel *workshopImage03;
    QLabel *workshopTitle03;
    QLabel *workshopDepartment03;
    QPushButton *workshopCard_Computer;
    QFrame *cardFrame05;
    QLabel *workshopImage05;
    QLabel *workshopTitle05;
    QLabel *workshopDepartment05;
    QPushButton *workshopCard_Geotechnical;
    QFrame *cardFrame06;
    QLabel *workshopImage06;
    QLabel *workshopTitle06;
    QLabel *workshopDepartment06;
    QPushButton *workshopCard_Surveying;
    QFrame *cardFrame07;
    QLabel *workshopImage07;
    QLabel *workshopTitle07;
    QLabel *workshopDepartment07;
    QPushButton *workshopCard_Concrete;
    QFrame *cardFrame11;
    QLabel *workshopImage11;
    QLabel *workshopTitle11;
    QLabel *workshopDepartment11;
    QPushButton *workshopCard_Drawing_Office;
    QFrame *cardFrame12;
    QLabel *workshopImage12;
    QLabel *workshopTitle12;
    QLabel *workshopDepartment12;
    QPushButton *workshopCard_Manufacturing;
    QFrame *cardFrame14;
    QLabel *workshopImage14;
    QLabel *workshopTitle14;
    QLabel *workshopDepartment14;
    QPushButton *workshopCard_Electrical_Machines___Communications_Laboratory;
    QFrame *cardFrame15;
    QLabel *workshopImage15;
    QLabel *workshopTitle15;
    QLabel *workshopDepartment15;
    QPushButton *workshopCard_Measurement_Laboratory;
    QPushButton *nextBtn;

    void setupUi(QWidget *selectionWidget)
    {
        if (selectionWidget->objectName().isEmpty())
            selectionWidget->setObjectName("selectionWidget");
        selectionWidget->resize(1180, 750);
        QFont font;
        font.setFamilies({QString::fromUtf8("Segoe UI")});
        font.setPointSize(10);
        selectionWidget->setFont(font);
        backTo0Btn = new QPushButton(selectionWidget);
        backTo0Btn->setObjectName("backTo0Btn");
        backTo0Btn->setGeometry(QRect(24, 24, 111, 37));
        QFont font1;
        font1.setFamilies({QString::fromUtf8("Segoe UI")});
        font1.setPointSize(10);
        font1.setBold(true);
        backTo0Btn->setFont(font1);
        backTo0Btn->setCursor(QCursor(Qt::CursorShape::PointingHandCursor));
        selectTitleLabel = new QLabel(selectionWidget);
        selectTitleLabel->setObjectName("selectTitleLabel");
        selectTitleLabel->setGeometry(QRect(645, 24, 511, 40));
        QFont font2;
        font2.setFamilies({QString::fromUtf8("Segoe UI")});
        font2.setPointSize(19);
        font2.setBold(true);
        selectTitleLabel->setFont(font2);
        selectTitleLabel->setAlignment(Qt::AlignmentFlag::AlignRight|Qt::AlignmentFlag::AlignTrailing|Qt::AlignmentFlag::AlignVCenter);
        selectTitleLabel->setWordWrap(true);
        selectionHintLabel = new QLabel(selectionWidget);
        selectionHintLabel->setObjectName("selectionHintLabel");
        selectionHintLabel->setGeometry(QRect(24, 80, 1132, 17));
        selectionHintLabel->setWordWrap(true);
        cardsScrollArea = new QScrollArea(selectionWidget);
        cardsScrollArea->setObjectName("cardsScrollArea");
        cardsScrollArea->setGeometry(QRect(24, 160, 1132, 443));
        cardsScrollArea->setFrameShape(QFrame::Shape::NoFrame);
        cardsScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAlwaysOff);
        cardsScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAlwaysOff);
        cardsScrollArea->setWidgetResizable(true);
        cardsScrollContents = new QWidget();
        cardsScrollContents->setObjectName("cardsScrollContents");
        cardsScrollContents->setGeometry(QRect(0, 0, 1132, 443));
        cardFrame01 = new QFrame(cardsScrollContents);
        cardFrame01->setObjectName("cardFrame01");
        cardFrame01->setGeometry(QRect(4, 4, 212, 210));
        cardFrame01->setFrameShape(QFrame::Shape::NoFrame);
        workshopImage01 = new QLabel(cardFrame01);
        workshopImage01->setObjectName("workshopImage01");
        workshopImage01->setGeometry(QRect(56, 24, 100, 100));
        workshopImage01->setPixmap(QPixmap(QString::fromUtf8(":/workshops/Biochemistry.png")));
        workshopImage01->setScaledContents(true);
        workshopImage01->setAlignment(Qt::AlignmentFlag::AlignCenter);
        workshopTitle01 = new QLabel(cardFrame01);
        workshopTitle01->setObjectName("workshopTitle01");
        workshopTitle01->setGeometry(QRect(16, 132, 180, 17));
        workshopTitle01->setFont(font1);
        workshopTitle01->setAlignment(Qt::AlignmentFlag::AlignCenter);
        workshopTitle01->setWordWrap(true);
        workshopDepartment01 = new QLabel(cardFrame01);
        workshopDepartment01->setObjectName("workshopDepartment01");
        workshopDepartment01->setGeometry(QRect(16, 153, 180, 53));
        workshopDepartment01->setFont(font);
        workshopDepartment01->setAlignment(Qt::AlignmentFlag::AlignHCenter|Qt::AlignmentFlag::AlignTop);
        workshopDepartment01->setWordWrap(true);
        workshopCard_Biochemistry = new QPushButton(cardFrame01);
        workshopCard_Biochemistry->setObjectName("workshopCard_Biochemistry");
        workshopCard_Biochemistry->setGeometry(QRect(0, 0, 212, 210));
        workshopCard_Biochemistry->setCursor(QCursor(Qt::CursorShape::PointingHandCursor));
        workshopCard_Biochemistry->setFlat(true);
        cardFrame02 = new QFrame(cardsScrollContents);
        cardFrame02->setObjectName("cardFrame02");
        cardFrame02->setGeometry(QRect(232, 4, 212, 210));
        cardFrame02->setFrameShape(QFrame::Shape::NoFrame);
        workshopImage02 = new QLabel(cardFrame02);
        workshopImage02->setObjectName("workshopImage02");
        workshopImage02->setGeometry(QRect(56, 24, 100, 100));
        workshopImage02->setPixmap(QPixmap(QString::fromUtf8(":/workshops/Electronics.png")));
        workshopImage02->setScaledContents(true);
        workshopImage02->setAlignment(Qt::AlignmentFlag::AlignCenter);
        workshopTitle02 = new QLabel(cardFrame02);
        workshopTitle02->setObjectName("workshopTitle02");
        workshopTitle02->setGeometry(QRect(16, 132, 180, 17));
        workshopTitle02->setFont(font1);
        workshopTitle02->setAlignment(Qt::AlignmentFlag::AlignCenter);
        workshopTitle02->setWordWrap(true);
        workshopDepartment02 = new QLabel(cardFrame02);
        workshopDepartment02->setObjectName("workshopDepartment02");
        workshopDepartment02->setGeometry(QRect(16, 153, 180, 53));
        workshopDepartment02->setFont(font);
        workshopDepartment02->setAlignment(Qt::AlignmentFlag::AlignHCenter|Qt::AlignmentFlag::AlignTop);
        workshopDepartment02->setWordWrap(true);
        workshopCard_Electronics = new QPushButton(cardFrame02);
        workshopCard_Electronics->setObjectName("workshopCard_Electronics");
        workshopCard_Electronics->setGeometry(QRect(0, 0, 212, 210));
        workshopCard_Electronics->setCursor(QCursor(Qt::CursorShape::PointingHandCursor));
        workshopCard_Electronics->setFlat(true);
        cardFrame03 = new QFrame(cardsScrollContents);
        cardFrame03->setObjectName("cardFrame03");
        cardFrame03->setGeometry(QRect(460, 4, 212, 210));
        cardFrame03->setFrameShape(QFrame::Shape::NoFrame);
        workshopImage03 = new QLabel(cardFrame03);
        workshopImage03->setObjectName("workshopImage03");
        workshopImage03->setGeometry(QRect(56, 24, 100, 100));
        workshopImage03->setPixmap(QPixmap(QString::fromUtf8(":/workshops/Computer.png")));
        workshopImage03->setScaledContents(true);
        workshopImage03->setAlignment(Qt::AlignmentFlag::AlignCenter);
        workshopTitle03 = new QLabel(cardFrame03);
        workshopTitle03->setObjectName("workshopTitle03");
        workshopTitle03->setGeometry(QRect(16, 132, 180, 17));
        workshopTitle03->setFont(font1);
        workshopTitle03->setAlignment(Qt::AlignmentFlag::AlignCenter);
        workshopTitle03->setWordWrap(true);
        workshopDepartment03 = new QLabel(cardFrame03);
        workshopDepartment03->setObjectName("workshopDepartment03");
        workshopDepartment03->setGeometry(QRect(16, 153, 180, 53));
        workshopDepartment03->setFont(font);
        workshopDepartment03->setAlignment(Qt::AlignmentFlag::AlignHCenter|Qt::AlignmentFlag::AlignTop);
        workshopDepartment03->setWordWrap(true);
        workshopCard_Computer = new QPushButton(cardFrame03);
        workshopCard_Computer->setObjectName("workshopCard_Computer");
        workshopCard_Computer->setGeometry(QRect(0, 0, 212, 210));
        workshopCard_Computer->setCursor(QCursor(Qt::CursorShape::PointingHandCursor));
        workshopCard_Computer->setFlat(true);
        cardFrame05 = new QFrame(cardsScrollContents);
        cardFrame05->setObjectName("cardFrame05");
        cardFrame05->setGeometry(QRect(688, 4, 212, 210));
        cardFrame05->setFrameShape(QFrame::Shape::NoFrame);
        workshopImage05 = new QLabel(cardFrame05);
        workshopImage05->setObjectName("workshopImage05");
        workshopImage05->setGeometry(QRect(56, 24, 100, 100));
        workshopImage05->setPixmap(QPixmap(QString::fromUtf8(":/workshops/Geotechnical.png")));
        workshopImage05->setScaledContents(true);
        workshopImage05->setAlignment(Qt::AlignmentFlag::AlignCenter);
        workshopTitle05 = new QLabel(cardFrame05);
        workshopTitle05->setObjectName("workshopTitle05");
        workshopTitle05->setGeometry(QRect(16, 132, 180, 17));
        workshopTitle05->setFont(font1);
        workshopTitle05->setAlignment(Qt::AlignmentFlag::AlignCenter);
        workshopTitle05->setWordWrap(true);
        workshopDepartment05 = new QLabel(cardFrame05);
        workshopDepartment05->setObjectName("workshopDepartment05");
        workshopDepartment05->setGeometry(QRect(16, 153, 180, 53));
        workshopDepartment05->setFont(font);
        workshopDepartment05->setAlignment(Qt::AlignmentFlag::AlignHCenter|Qt::AlignmentFlag::AlignTop);
        workshopDepartment05->setWordWrap(true);
        workshopCard_Geotechnical = new QPushButton(cardFrame05);
        workshopCard_Geotechnical->setObjectName("workshopCard_Geotechnical");
        workshopCard_Geotechnical->setGeometry(QRect(0, 0, 212, 210));
        workshopCard_Geotechnical->setCursor(QCursor(Qt::CursorShape::PointingHandCursor));
        workshopCard_Geotechnical->setFlat(true);
        cardFrame06 = new QFrame(cardsScrollContents);
        cardFrame06->setObjectName("cardFrame06");
        cardFrame06->setGeometry(QRect(916, 4, 212, 210));
        cardFrame06->setFrameShape(QFrame::Shape::NoFrame);
        workshopImage06 = new QLabel(cardFrame06);
        workshopImage06->setObjectName("workshopImage06");
        workshopImage06->setGeometry(QRect(56, 24, 100, 100));
        workshopImage06->setPixmap(QPixmap(QString::fromUtf8(":/workshops/Surveying.png")));
        workshopImage06->setScaledContents(true);
        workshopImage06->setAlignment(Qt::AlignmentFlag::AlignCenter);
        workshopTitle06 = new QLabel(cardFrame06);
        workshopTitle06->setObjectName("workshopTitle06");
        workshopTitle06->setGeometry(QRect(16, 132, 180, 17));
        workshopTitle06->setFont(font1);
        workshopTitle06->setAlignment(Qt::AlignmentFlag::AlignCenter);
        workshopTitle06->setWordWrap(true);
        workshopDepartment06 = new QLabel(cardFrame06);
        workshopDepartment06->setObjectName("workshopDepartment06");
        workshopDepartment06->setGeometry(QRect(16, 153, 180, 53));
        workshopDepartment06->setFont(font);
        workshopDepartment06->setAlignment(Qt::AlignmentFlag::AlignHCenter|Qt::AlignmentFlag::AlignTop);
        workshopDepartment06->setWordWrap(true);
        workshopCard_Surveying = new QPushButton(cardFrame06);
        workshopCard_Surveying->setObjectName("workshopCard_Surveying");
        workshopCard_Surveying->setGeometry(QRect(0, 0, 212, 210));
        workshopCard_Surveying->setCursor(QCursor(Qt::CursorShape::PointingHandCursor));
        workshopCard_Surveying->setFlat(true);
        cardFrame07 = new QFrame(cardsScrollContents);
        cardFrame07->setObjectName("cardFrame07");
        cardFrame07->setGeometry(QRect(4, 230, 212, 210));
        cardFrame07->setFrameShape(QFrame::Shape::NoFrame);
        workshopImage07 = new QLabel(cardFrame07);
        workshopImage07->setObjectName("workshopImage07");
        workshopImage07->setGeometry(QRect(56, 24, 100, 100));
        workshopImage07->setPixmap(QPixmap(QString::fromUtf8(":/workshops/Concrete.png")));
        workshopImage07->setScaledContents(true);
        workshopImage07->setAlignment(Qt::AlignmentFlag::AlignCenter);
        workshopTitle07 = new QLabel(cardFrame07);
        workshopTitle07->setObjectName("workshopTitle07");
        workshopTitle07->setGeometry(QRect(16, 132, 180, 17));
        workshopTitle07->setFont(font1);
        workshopTitle07->setAlignment(Qt::AlignmentFlag::AlignCenter);
        workshopTitle07->setWordWrap(true);
        workshopDepartment07 = new QLabel(cardFrame07);
        workshopDepartment07->setObjectName("workshopDepartment07");
        workshopDepartment07->setGeometry(QRect(16, 153, 180, 53));
        workshopDepartment07->setFont(font);
        workshopDepartment07->setAlignment(Qt::AlignmentFlag::AlignHCenter|Qt::AlignmentFlag::AlignTop);
        workshopDepartment07->setWordWrap(true);
        workshopCard_Concrete = new QPushButton(cardFrame07);
        workshopCard_Concrete->setObjectName("workshopCard_Concrete");
        workshopCard_Concrete->setGeometry(QRect(0, 0, 212, 210));
        workshopCard_Concrete->setCursor(QCursor(Qt::CursorShape::PointingHandCursor));
        workshopCard_Concrete->setFlat(true);
        cardFrame11 = new QFrame(cardsScrollContents);
        cardFrame11->setObjectName("cardFrame11");
        cardFrame11->setGeometry(QRect(232, 230, 212, 210));
        cardFrame11->setFrameShape(QFrame::Shape::NoFrame);
        workshopImage11 = new QLabel(cardFrame11);
        workshopImage11->setObjectName("workshopImage11");
        workshopImage11->setGeometry(QRect(56, 24, 100, 100));
        workshopImage11->setPixmap(QPixmap(QString::fromUtf8(":/workshops/Drawing Office.png")));
        workshopImage11->setScaledContents(true);
        workshopImage11->setAlignment(Qt::AlignmentFlag::AlignCenter);
        workshopTitle11 = new QLabel(cardFrame11);
        workshopTitle11->setObjectName("workshopTitle11");
        workshopTitle11->setGeometry(QRect(16, 132, 180, 17));
        workshopTitle11->setFont(font1);
        workshopTitle11->setAlignment(Qt::AlignmentFlag::AlignCenter);
        workshopTitle11->setWordWrap(true);
        workshopDepartment11 = new QLabel(cardFrame11);
        workshopDepartment11->setObjectName("workshopDepartment11");
        workshopDepartment11->setGeometry(QRect(16, 153, 180, 53));
        workshopDepartment11->setFont(font);
        workshopDepartment11->setAlignment(Qt::AlignmentFlag::AlignHCenter|Qt::AlignmentFlag::AlignTop);
        workshopDepartment11->setWordWrap(true);
        workshopCard_Drawing_Office = new QPushButton(cardFrame11);
        workshopCard_Drawing_Office->setObjectName("workshopCard_Drawing_Office");
        workshopCard_Drawing_Office->setGeometry(QRect(0, 0, 212, 210));
        workshopCard_Drawing_Office->setCursor(QCursor(Qt::CursorShape::PointingHandCursor));
        workshopCard_Drawing_Office->setFlat(true);
        cardFrame12 = new QFrame(cardsScrollContents);
        cardFrame12->setObjectName("cardFrame12");
        cardFrame12->setGeometry(QRect(460, 230, 212, 210));
        cardFrame12->setFrameShape(QFrame::Shape::NoFrame);
        workshopImage12 = new QLabel(cardFrame12);
        workshopImage12->setObjectName("workshopImage12");
        workshopImage12->setGeometry(QRect(56, 24, 100, 100));
        workshopImage12->setPixmap(QPixmap(QString::fromUtf8(":/workshops/Manufacturing.png")));
        workshopImage12->setScaledContents(true);
        workshopImage12->setAlignment(Qt::AlignmentFlag::AlignCenter);
        workshopTitle12 = new QLabel(cardFrame12);
        workshopTitle12->setObjectName("workshopTitle12");
        workshopTitle12->setGeometry(QRect(16, 132, 180, 17));
        workshopTitle12->setFont(font1);
        workshopTitle12->setAlignment(Qt::AlignmentFlag::AlignCenter);
        workshopTitle12->setWordWrap(true);
        workshopDepartment12 = new QLabel(cardFrame12);
        workshopDepartment12->setObjectName("workshopDepartment12");
        workshopDepartment12->setGeometry(QRect(16, 153, 180, 53));
        workshopDepartment12->setFont(font);
        workshopDepartment12->setAlignment(Qt::AlignmentFlag::AlignHCenter|Qt::AlignmentFlag::AlignTop);
        workshopDepartment12->setWordWrap(true);
        workshopCard_Manufacturing = new QPushButton(cardFrame12);
        workshopCard_Manufacturing->setObjectName("workshopCard_Manufacturing");
        workshopCard_Manufacturing->setGeometry(QRect(0, 0, 212, 210));
        workshopCard_Manufacturing->setCursor(QCursor(Qt::CursorShape::PointingHandCursor));
        workshopCard_Manufacturing->setFlat(true);
        cardFrame14 = new QFrame(cardsScrollContents);
        cardFrame14->setObjectName("cardFrame14");
        cardFrame14->setGeometry(QRect(688, 230, 212, 210));
        cardFrame14->setFrameShape(QFrame::Shape::NoFrame);
        workshopImage14 = new QLabel(cardFrame14);
        workshopImage14->setObjectName("workshopImage14");
        workshopImage14->setGeometry(QRect(56, 24, 100, 100));
        workshopImage14->setPixmap(QPixmap(QString::fromUtf8(":/workshops/Electrical Machines & Communications Laboratory.png")));
        workshopImage14->setScaledContents(true);
        workshopImage14->setAlignment(Qt::AlignmentFlag::AlignCenter);
        workshopTitle14 = new QLabel(cardFrame14);
        workshopTitle14->setObjectName("workshopTitle14");
        workshopTitle14->setGeometry(QRect(16, 132, 180, 34));
        workshopTitle14->setFont(font1);
        workshopTitle14->setAlignment(Qt::AlignmentFlag::AlignCenter);
        workshopTitle14->setWordWrap(true);
        workshopDepartment14 = new QLabel(cardFrame14);
        workshopDepartment14->setObjectName("workshopDepartment14");
        workshopDepartment14->setGeometry(QRect(16, 170, 180, 36));
        workshopDepartment14->setFont(font);
        workshopDepartment14->setAlignment(Qt::AlignmentFlag::AlignHCenter|Qt::AlignmentFlag::AlignTop);
        workshopDepartment14->setWordWrap(true);
        workshopCard_Electrical_Machines___Communications_Laboratory = new QPushButton(cardFrame14);
        workshopCard_Electrical_Machines___Communications_Laboratory->setObjectName("workshopCard_Electrical_Machines___Communications_Laboratory");
        workshopCard_Electrical_Machines___Communications_Laboratory->setGeometry(QRect(0, 0, 212, 210));
        workshopCard_Electrical_Machines___Communications_Laboratory->setCursor(QCursor(Qt::CursorShape::PointingHandCursor));
        workshopCard_Electrical_Machines___Communications_Laboratory->setFlat(true);
        cardFrame15 = new QFrame(cardsScrollContents);
        cardFrame15->setObjectName("cardFrame15");
        cardFrame15->setGeometry(QRect(916, 230, 212, 210));
        cardFrame15->setFrameShape(QFrame::Shape::NoFrame);
        workshopImage15 = new QLabel(cardFrame15);
        workshopImage15->setObjectName("workshopImage15");
        workshopImage15->setGeometry(QRect(56, 24, 100, 100));
        workshopImage15->setPixmap(QPixmap(QString::fromUtf8(":/workshops/Measurement Laboratory.png")));
        workshopImage15->setScaledContents(true);
        workshopImage15->setAlignment(Qt::AlignmentFlag::AlignCenter);
        workshopTitle15 = new QLabel(cardFrame15);
        workshopTitle15->setObjectName("workshopTitle15");
        workshopTitle15->setGeometry(QRect(16, 132, 180, 17));
        workshopTitle15->setFont(font1);
        workshopTitle15->setAlignment(Qt::AlignmentFlag::AlignCenter);
        workshopTitle15->setWordWrap(true);
        workshopDepartment15 = new QLabel(cardFrame15);
        workshopDepartment15->setObjectName("workshopDepartment15");
        workshopDepartment15->setGeometry(QRect(16, 153, 180, 53));
        workshopDepartment15->setFont(font);
        workshopDepartment15->setAlignment(Qt::AlignmentFlag::AlignHCenter|Qt::AlignmentFlag::AlignTop);
        workshopDepartment15->setWordWrap(true);
        workshopCard_Measurement_Laboratory = new QPushButton(cardFrame15);
        workshopCard_Measurement_Laboratory->setObjectName("workshopCard_Measurement_Laboratory");
        workshopCard_Measurement_Laboratory->setGeometry(QRect(0, 0, 212, 210));
        workshopCard_Measurement_Laboratory->setCursor(QCursor(Qt::CursorShape::PointingHandCursor));
        workshopCard_Measurement_Laboratory->setFlat(true);
        cardsScrollArea->setWidget(cardsScrollContents);
        nextBtn = new QPushButton(selectionWidget);
        nextBtn->setObjectName("nextBtn");
        nextBtn->setEnabled(false);
        nextBtn->setGeometry(QRect(24, 680, 1132, 50));
        nextBtn->setFont(font1);
        nextBtn->setCursor(QCursor(Qt::CursorShape::PointingHandCursor));
        QWidget::setTabOrder(backTo0Btn, cardsScrollArea);
        QWidget::setTabOrder(cardsScrollArea, workshopCard_Biochemistry);
        QWidget::setTabOrder(workshopCard_Biochemistry, workshopCard_Electronics);
        QWidget::setTabOrder(workshopCard_Electronics, workshopCard_Computer);
        QWidget::setTabOrder(workshopCard_Computer, workshopCard_Geotechnical);
        QWidget::setTabOrder(workshopCard_Geotechnical, workshopCard_Surveying);
        QWidget::setTabOrder(workshopCard_Surveying, workshopCard_Concrete);
        QWidget::setTabOrder(workshopCard_Concrete, workshopCard_Drawing_Office);
        QWidget::setTabOrder(workshopCard_Drawing_Office, workshopCard_Manufacturing);
        QWidget::setTabOrder(workshopCard_Manufacturing, workshopCard_Electrical_Machines___Communications_Laboratory);
        QWidget::setTabOrder(workshopCard_Electrical_Machines___Communications_Laboratory, workshopCard_Measurement_Laboratory);
        QWidget::setTabOrder(workshopCard_Measurement_Laboratory, nextBtn);

        retranslateUi(selectionWidget);

        QMetaObject::connectSlotsByName(selectionWidget);
    } // setupUi

    void retranslateUi(QWidget *selectionWidget)
    {
        selectionWidget->setStyleSheet(QCoreApplication::translate("selectionWidget", "QWidget#selectionWidget { background:#F4F7FB; color:#17243B; } QLabel { background:transparent; color:#17243B; } QWidget#cardsScrollContents, QScrollArea#cardsScrollArea { background:transparent; border:none; } QPushButton { background:#FFFFFF; color:#17243B; border:1px solid #D4DEEB; border-radius:8px; padding:9px 14px; } QPushButton:hover, QPushButton:focus { background:#EAF2FF; border-color:#2563EB; } QPushButton#nextBtn { background:#2563EB; color:white; border:1px solid #2563EB; } QPushButton#nextBtn:hover { background:#1D4ED8; } QPushButton#nextBtn:disabled { background:#F4F7FB; color:#52647D; border:1px solid #D4DEEB; }", nullptr));
        backTo0Btn->setText(QCoreApplication::translate("selectionWidget", "Home", nullptr));
        selectTitleLabel->setText(QCoreApplication::translate("selectionWidget", "Choose a workshop or laboratory ", nullptr));
        selectionHintLabel->setText(QCoreApplication::translate("selectionWidget", "Step 1 of 3 \302\267 Select a laboratory or workshop, then continue to choose your time.", nullptr));
        cardFrame01->setStyleSheet(QCoreApplication::translate("selectionWidget", "QFrame#cardFrame01 { background:#FFFFFF; border:2px solid #D4DEEB; border-radius:12px; } QFrame#cardFrame01[selected=\"true\"] { background:#EAF2FF; border-color:#6099F5; }", nullptr));
        cardFrame01->setProperty("category", QVariant(QCoreApplication::translate("selectionWidget", "Biochemistry", nullptr)));
        workshopTitle01->setText(QCoreApplication::translate("selectionWidget", "Biochemistry Lab", nullptr));
        workshopDepartment01->setText(QCoreApplication::translate("selectionWidget", "Department of Nano Science Technology", nullptr));
#if QT_CONFIG(tooltip)
        workshopCard_Biochemistry->setToolTip(QCoreApplication::translate("selectionWidget", "Biochemistry Lab\n"
"Department of Nano Science Technology", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(accessibility)
        workshopCard_Biochemistry->setAccessibleName(QCoreApplication::translate("selectionWidget", "Biochemistry Lab", nullptr));
#endif // QT_CONFIG(accessibility)
        workshopCard_Biochemistry->setStyleSheet(QCoreApplication::translate("selectionWidget", "QPushButton { background:transparent; border:2px solid transparent; border-radius:12px; padding:0; } QPushButton:hover, QPushButton:focus { border:2px solid #6099F5; background:rgba(80,140,240,18); }", nullptr));
        workshopCard_Biochemistry->setText(QString());
        cardFrame02->setStyleSheet(QCoreApplication::translate("selectionWidget", "QFrame#cardFrame02 { background:#FFFFFF; border:2px solid #D4DEEB; border-radius:12px; } QFrame#cardFrame02[selected=\"true\"] { background:#EAF2FF; border-color:#6099F5; }", nullptr));
        cardFrame02->setProperty("category", QVariant(QCoreApplication::translate("selectionWidget", "Electronics", nullptr)));
        workshopTitle02->setText(QCoreApplication::translate("selectionWidget", "Electronics Lab", nullptr));
        workshopDepartment02->setText(QCoreApplication::translate("selectionWidget", "Department of Nano Science Technology", nullptr));
#if QT_CONFIG(tooltip)
        workshopCard_Electronics->setToolTip(QCoreApplication::translate("selectionWidget", "Electronics Lab\n"
"Department of Nano Science Technology", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(accessibility)
        workshopCard_Electronics->setAccessibleName(QCoreApplication::translate("selectionWidget", "Electronics Lab", nullptr));
#endif // QT_CONFIG(accessibility)
        workshopCard_Electronics->setStyleSheet(QCoreApplication::translate("selectionWidget", "QPushButton { background:transparent; border:2px solid transparent; border-radius:12px; padding:0; } QPushButton:hover, QPushButton:focus { border:2px solid #6099F5; background:rgba(80,140,240,18); }", nullptr));
        workshopCard_Electronics->setText(QString());
        cardFrame03->setStyleSheet(QCoreApplication::translate("selectionWidget", "QFrame#cardFrame03 { background:#FFFFFF; border:2px solid #D4DEEB; border-radius:12px; } QFrame#cardFrame03[selected=\"true\"] { background:#EAF2FF; border-color:#6099F5; }", nullptr));
        cardFrame03->setProperty("category", QVariant(QCoreApplication::translate("selectionWidget", "Computer", nullptr)));
        workshopTitle03->setText(QCoreApplication::translate("selectionWidget", "Computer Lab", nullptr));
        workshopDepartment03->setText(QCoreApplication::translate("selectionWidget", "Department of Nano Science Technology", nullptr));
#if QT_CONFIG(tooltip)
        workshopCard_Computer->setToolTip(QCoreApplication::translate("selectionWidget", "Computer Lab\n"
"Department of Nano Science Technology", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(accessibility)
        workshopCard_Computer->setAccessibleName(QCoreApplication::translate("selectionWidget", "Computer Lab", nullptr));
#endif // QT_CONFIG(accessibility)
        workshopCard_Computer->setStyleSheet(QCoreApplication::translate("selectionWidget", "QPushButton { background:transparent; border:2px solid transparent; border-radius:12px; padding:0; } QPushButton:hover, QPushButton:focus { border:2px solid #6099F5; background:rgba(80,140,240,18); }", nullptr));
        workshopCard_Computer->setText(QString());
        cardFrame05->setStyleSheet(QCoreApplication::translate("selectionWidget", "QFrame#cardFrame05 { background:#FFFFFF; border:2px solid #D4DEEB; border-radius:12px; } QFrame#cardFrame05[selected=\"true\"] { background:#EAF2FF; border-color:#6099F5; }", nullptr));
        cardFrame05->setProperty("category", QVariant(QCoreApplication::translate("selectionWidget", "Geotechnical", nullptr)));
        workshopTitle05->setText(QCoreApplication::translate("selectionWidget", "Geotechnical Lab", nullptr));
        workshopDepartment05->setText(QCoreApplication::translate("selectionWidget", "Department of Construction Technology", nullptr));
#if QT_CONFIG(tooltip)
        workshopCard_Geotechnical->setToolTip(QCoreApplication::translate("selectionWidget", "Geotechnical Lab\n"
"Department of Construction Technology", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(accessibility)
        workshopCard_Geotechnical->setAccessibleName(QCoreApplication::translate("selectionWidget", "Geotechnical Lab", nullptr));
#endif // QT_CONFIG(accessibility)
        workshopCard_Geotechnical->setStyleSheet(QCoreApplication::translate("selectionWidget", "QPushButton { background:transparent; border:2px solid transparent; border-radius:12px; padding:0; } QPushButton:hover, QPushButton:focus { border:2px solid #6099F5; background:rgba(80,140,240,18); }", nullptr));
        workshopCard_Geotechnical->setText(QString());
        cardFrame06->setStyleSheet(QCoreApplication::translate("selectionWidget", "QFrame#cardFrame06 { background:#FFFFFF; border:2px solid #D4DEEB; border-radius:12px; } QFrame#cardFrame06[selected=\"true\"] { background:#EAF2FF; border-color:#6099F5; }", nullptr));
        cardFrame06->setProperty("category", QVariant(QCoreApplication::translate("selectionWidget", "Surveying", nullptr)));
        workshopTitle06->setText(QCoreApplication::translate("selectionWidget", "Surveying Lab", nullptr));
        workshopDepartment06->setText(QCoreApplication::translate("selectionWidget", "Department of Construction Technology", nullptr));
#if QT_CONFIG(tooltip)
        workshopCard_Surveying->setToolTip(QCoreApplication::translate("selectionWidget", "Surveying Lab\n"
"Department of Construction Technology", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(accessibility)
        workshopCard_Surveying->setAccessibleName(QCoreApplication::translate("selectionWidget", "Surveying Lab", nullptr));
#endif // QT_CONFIG(accessibility)
        workshopCard_Surveying->setStyleSheet(QCoreApplication::translate("selectionWidget", "QPushButton { background:transparent; border:2px solid transparent; border-radius:12px; padding:0; } QPushButton:hover, QPushButton:focus { border:2px solid #6099F5; background:rgba(80,140,240,18); }", nullptr));
        workshopCard_Surveying->setText(QString());
        cardFrame07->setStyleSheet(QCoreApplication::translate("selectionWidget", "QFrame#cardFrame07 { background:#FFFFFF; border:2px solid #D4DEEB; border-radius:12px; } QFrame#cardFrame07[selected=\"true\"] { background:#EAF2FF; border-color:#6099F5; }", nullptr));
        cardFrame07->setProperty("category", QVariant(QCoreApplication::translate("selectionWidget", "Concrete", nullptr)));
        workshopTitle07->setText(QCoreApplication::translate("selectionWidget", "Concrete Lab", nullptr));
        workshopDepartment07->setText(QCoreApplication::translate("selectionWidget", "Department of Construction Technology", nullptr));
#if QT_CONFIG(tooltip)
        workshopCard_Concrete->setToolTip(QCoreApplication::translate("selectionWidget", "Concrete Lab\n"
"Department of Construction Technology", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(accessibility)
        workshopCard_Concrete->setAccessibleName(QCoreApplication::translate("selectionWidget", "Concrete Lab", nullptr));
#endif // QT_CONFIG(accessibility)
        workshopCard_Concrete->setStyleSheet(QCoreApplication::translate("selectionWidget", "QPushButton { background:transparent; border:2px solid transparent; border-radius:12px; padding:0; } QPushButton:hover, QPushButton:focus { border:2px solid #6099F5; background:rgba(80,140,240,18); }", nullptr));
        workshopCard_Concrete->setText(QString());
        cardFrame11->setStyleSheet(QCoreApplication::translate("selectionWidget", "QFrame#cardFrame11 { background:#FFFFFF; border:2px solid #D4DEEB; border-radius:12px; } QFrame#cardFrame11[selected=\"true\"] { background:#EAF2FF; border-color:#6099F5; }", nullptr));
        cardFrame11->setProperty("category", QVariant(QCoreApplication::translate("selectionWidget", "Drawing Office", nullptr)));
        workshopTitle11->setText(QCoreApplication::translate("selectionWidget", "Drawing Office", nullptr));
        workshopDepartment11->setText(QCoreApplication::translate("selectionWidget", "Department of Mechanical and Manufacturing Technology", nullptr));
#if QT_CONFIG(tooltip)
        workshopCard_Drawing_Office->setToolTip(QCoreApplication::translate("selectionWidget", "Drawing Office\n"
"Department of Mechanical and Manufacturing Technology", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(accessibility)
        workshopCard_Drawing_Office->setAccessibleName(QCoreApplication::translate("selectionWidget", "Drawing Office", nullptr));
#endif // QT_CONFIG(accessibility)
        workshopCard_Drawing_Office->setStyleSheet(QCoreApplication::translate("selectionWidget", "QPushButton { background:transparent; border:2px solid transparent; border-radius:12px; padding:0; } QPushButton:hover, QPushButton:focus { border:2px solid #6099F5; background:rgba(80,140,240,18); }", nullptr));
        workshopCard_Drawing_Office->setText(QString());
        cardFrame12->setStyleSheet(QCoreApplication::translate("selectionWidget", "QFrame#cardFrame12 { background:#FFFFFF; border:2px solid #D4DEEB; border-radius:12px; } QFrame#cardFrame12[selected=\"true\"] { background:#EAF2FF; border-color:#6099F5; }", nullptr));
        cardFrame12->setProperty("category", QVariant(QCoreApplication::translate("selectionWidget", "Manufacturing", nullptr)));
        workshopTitle12->setText(QCoreApplication::translate("selectionWidget", "Manufacturing Lab", nullptr));
        workshopDepartment12->setText(QCoreApplication::translate("selectionWidget", "Department of Mechanical and Manufacturing Technology", nullptr));
#if QT_CONFIG(tooltip)
        workshopCard_Manufacturing->setToolTip(QCoreApplication::translate("selectionWidget", "Manufacturing Lab\n"
"Department of Mechanical and Manufacturing Technology", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(accessibility)
        workshopCard_Manufacturing->setAccessibleName(QCoreApplication::translate("selectionWidget", "Manufacturing Lab", nullptr));
#endif // QT_CONFIG(accessibility)
        workshopCard_Manufacturing->setStyleSheet(QCoreApplication::translate("selectionWidget", "QPushButton { background:transparent; border:2px solid transparent; border-radius:12px; padding:0; } QPushButton:hover, QPushButton:focus { border:2px solid #6099F5; background:rgba(80,140,240,18); }", nullptr));
        workshopCard_Manufacturing->setText(QString());
        cardFrame14->setStyleSheet(QCoreApplication::translate("selectionWidget", "QFrame#cardFrame14 { background:#FFFFFF; border:2px solid #D4DEEB; border-radius:12px; } QFrame#cardFrame14[selected=\"true\"] { background:#EAF2FF; border-color:#6099F5; }", nullptr));
        cardFrame14->setProperty("category", QVariant(QCoreApplication::translate("selectionWidget", "Electrical Machines & Communications Laboratory", nullptr)));
        workshopTitle14->setText(QCoreApplication::translate("selectionWidget", "Electrical Machines & Communications Laboratory", nullptr));
        workshopDepartment14->setText(QCoreApplication::translate("selectionWidget", "Department of Electrotechnology", nullptr));
#if QT_CONFIG(tooltip)
        workshopCard_Electrical_Machines___Communications_Laboratory->setToolTip(QCoreApplication::translate("selectionWidget", "Electrical Machines & Communications Laboratory\n"
"Department of Electrotechnology", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(accessibility)
        workshopCard_Electrical_Machines___Communications_Laboratory->setAccessibleName(QCoreApplication::translate("selectionWidget", "Electrical Machines & Communications Laboratory", nullptr));
#endif // QT_CONFIG(accessibility)
        workshopCard_Electrical_Machines___Communications_Laboratory->setStyleSheet(QCoreApplication::translate("selectionWidget", "QPushButton { background:transparent; border:2px solid transparent; border-radius:12px; padding:0; } QPushButton:hover, QPushButton:focus { border:2px solid #6099F5; background:rgba(80,140,240,18); }", nullptr));
        workshopCard_Electrical_Machines___Communications_Laboratory->setText(QString());
        cardFrame15->setStyleSheet(QCoreApplication::translate("selectionWidget", "QFrame#cardFrame15 { background:#FFFFFF; border:2px solid #D4DEEB; border-radius:12px; } QFrame#cardFrame15[selected=\"true\"] { background:#EAF2FF; border-color:#6099F5; }", nullptr));
        cardFrame15->setProperty("category", QVariant(QCoreApplication::translate("selectionWidget", "Measurement Laboratory", nullptr)));
        workshopTitle15->setText(QCoreApplication::translate("selectionWidget", "Measurement Laboratory", nullptr));
        workshopDepartment15->setText(QCoreApplication::translate("selectionWidget", "Department of Electrotechnology", nullptr));
#if QT_CONFIG(tooltip)
        workshopCard_Measurement_Laboratory->setToolTip(QCoreApplication::translate("selectionWidget", "Measurement Laboratory\n"
"Department of Electrotechnology", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(accessibility)
        workshopCard_Measurement_Laboratory->setAccessibleName(QCoreApplication::translate("selectionWidget", "Measurement Laboratory", nullptr));
#endif // QT_CONFIG(accessibility)
        workshopCard_Measurement_Laboratory->setStyleSheet(QCoreApplication::translate("selectionWidget", "QPushButton { background:transparent; border:2px solid transparent; border-radius:12px; padding:0; } QPushButton:hover, QPushButton:focus { border:2px solid #6099F5; background:rgba(80,140,240,18); }", nullptr));
        workshopCard_Measurement_Laboratory->setText(QString());
        nextBtn->setText(QCoreApplication::translate("selectionWidget", "Continue to date and time", nullptr));
    } // retranslateUi

};

namespace Ui {
    class selectionWidget: public Ui_selectionWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SELECTION_H
