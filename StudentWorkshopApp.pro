# Open this file in Qt Creator and select a Qt 6 Desktop kit.
QT += core gui widgets sql printsupport
lessThan(QT_MAJOR_VERSION, 6): error("This project requires Qt 6 or newer.")
CONFIG += c++17
TEMPLATE = app
TARGET = StudentWorkshopApp

INCLUDEPATH += \
    $$PWD/ui \
    $$PWD/ui/dialogs \
    $$PWD/ui/widgets \
    $$PWD/models \
    $$PWD/database \
    $$PWD/logic \
    $$PWD/utilities

SOURCES += \
    main.cpp \
    ui/mainwindow.cpp \
    ui/mainwindow_design.cpp \
    ui/mainwindow_navigation.cpp \
    ui/mainwindow_calendar.cpp \
    ui/mainwindow_booking.cpp \
    ui/mainwindow_admin.cpp \
    ui/dialogs/customalertdialog.cpp \
    ui/dialogs/custominputdialog.cpp \
    ui/dialogs/adminsettingsdialog.cpp \
    ui/dialogs/systemsettingsdialog.cpp \
    ui/dialogs/dialogtheme.cpp \
    ui/widgets/customcalendarwidget.cpp \
    models/studentrecord.cpp \
    database/databasemanager.cpp \
    database/databasewrite.cpp \
    database/databasequeries.cpp \
    logic/bookingrules.cpp \
    utilities/apphelpers.cpp \
    utilities/attachmentstorage.cpp

HEADERS += \
    ui/mainwindow.h \
    ui/dialogs/customdialogs.h \
    ui/dialogs/customalertdialog.h \
    ui/dialogs/custominputdialog.h \
    ui/dialogs/adminsettingsdialog.h \
    ui/dialogs/systemsettingsdialog.h \
    ui/dialogs/dialogtheme.h \
    ui/widgets/customcalendarwidget.h \
    models/studentrecord.h \
    database/idatabasemanager.h \
    database/databasemanager.h \
    database/databasehelpers.h \
    logic/bookingrules.h \
    utilities/apphelpers.h \
    utilities/attachmentstorage.h \
    utilities/workshopcatalog.h

FORMS += \
    ui/forms/mainwindow.ui \
    ui/forms/page0.ui \
    ui/forms/page1.ui \
    ui/forms/page2.ui \
    ui/forms/page3.ui \
    ui/forms/page4.ui \
    ui/forms/customalertdialog.ui \
    ui/forms/custominputdialog.ui \
    ui/forms/adminsettingsdialog.ui \
    ui/forms/systemsettingsdialog.ui

DISTFILES += \
    README.md \
    CODE_GUIDE.md \
    TESTING.md \
    sql/schema.sql \
    sql/sample_data.sql \
    sql/queries.sql
