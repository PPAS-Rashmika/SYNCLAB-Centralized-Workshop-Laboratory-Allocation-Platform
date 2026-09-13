QT       += core gui sql printsupport
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

# Qt 6 සඳහා C++17 අනිවාර්ය වේ
CONFIG += c++17

TARGET = StudentWorkshopApp
TEMPLATE = app

SOURCES += \
    customcalendarwidget.cpp \
    databasemanager.cpp \
    main.cpp \
    mainwindow.cpp \
    recordswindow.cpp \
    studentrecord.cpp

HEADERS += \
    customcalendarwidget.h \
    databasemanager.h \
    mainwindow.h \
    recordswindow.h \
    studentrecord.h

FORMS += \
    mainwindow.ui