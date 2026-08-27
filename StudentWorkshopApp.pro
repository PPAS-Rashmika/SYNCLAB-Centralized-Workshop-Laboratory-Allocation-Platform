QT       += core gui sql
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

TARGET = StudentWorkshopApp
TEMPLATE = app

SOURCES += \
    databasemanager.cpp \
    main.cpp \
    mainwindow.cpp \
    recordswindow.cpp \
    studentrecord.cpp

HEADERS += \
    databasemanager.h \
    mainwindow.h \
    recordswindow.h \
    studentrecord.h

FORMS += \
    mainwindow.ui