QT       += core gui sql printsupport
QT += widgets
lessThan(QT_MAJOR_VERSION, 6): error("This project requires Qt 6 or newer.")

# Qt 6 සඳහා C++17 අනිවාර්ය වේ
CONFIG += c++17

TARGET = StudentWorkshopApp
TEMPLATE = app

SOURCES += \
    customcalendarwidget.cpp \
    customdialogs.cpp \
    databasemanager.cpp \
    main.cpp \
    mainwindow.cpp \
    studentrecord.cpp

HEADERS += \
    customcalendarwidget.h \
    customdialogs.h \
    databasemanager.h \
    mainwindow.h \
    studentrecord.h \
    workshopcatalog.h

FORMS += \
    adminsettingsdialog.ui \
    customalertdialog.ui \
    custominputdialog.ui \
    mainwindow.ui \
    page0.ui \
    page1.ui \
    page2.ui \
    page3.ui \
    page4.ui \
    systemsettingsdialog.ui
