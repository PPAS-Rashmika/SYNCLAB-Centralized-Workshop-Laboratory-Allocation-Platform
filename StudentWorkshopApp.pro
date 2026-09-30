# Open this file in Qt Creator and select a Qt 6 Desktop kit.
#
# Qt modules the program needs:
#   core, gui, widgets - the windows, buttons, tables and calendars
#   sql                - the SQLite database
#   printsupport       - the administrator's "Print list" button
QT += core gui widgets sql printsupport
lessThan(QT_MAJOR_VERSION, 6): error("This project requires Qt 6 or newer.")
CONFIG += c++17
TEMPLATE = app
TARGET = StudentWorkshopApp

# The build tells the program which folder these source files live in, so the
# program can keep all of its input data in a "data" subfolder beside them,
# no matter which directory the finished exe is started from.
DEFINES += "\"SOURCE_DIR=\\\"$$PWD\\\"\""

# Every folder is listed here so that a source file can write
# #include "studentrecord.h" instead of a long relative path.
INCLUDEPATH += \
    $$PWD/ui \
    $$PWD/ui/dialogs \
    $$PWD/ui/widgets \
    $$PWD/models \
    $$PWD/database \
    $$PWD/logic \
    $$PWD/utilities

# The .cpp files that are compiled into the program.
SOURCES += \
    main.cpp \
    ui/mainwindow.cpp \
    ui/dialogs/customalertdialog.cpp \
    ui/dialogs/custominputdialog.cpp \
    ui/dialogs/adminsettingsdialog.cpp \
    ui/dialogs/systemsettingsdialog.cpp \
    ui/dialogs/dialogtheme.cpp \
    ui/widgets/customcalendarwidget.cpp \
    database/databasemanager.cpp \
    logic/allocationrules.cpp \
    utilities/workshopcatalog.cpp \
    utilities/filestorage.cpp

# The .h files. They are listed so that Qt Creator shows them in the project
# tree and so that the moc tool can find the classes that use signals and slots.
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
    logic/allocationrules.h \
    utilities/workshopcatalog.h \
    utilities/filestorage.h

# The screens drawn in Qt Designer. The uic tool turns each one into a
# ui_<name>.h file that the program includes.
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

# The application logo and workshop pictures. The rcc tool compiles them into the exe, so the
# program finds them from any folder; workshopImage() loads them by category.
RESOURCES += resources/images.qrc

# Files kept with the project but not compiled. The SQL files are for reference;
# the program creates its own tables when it first runs. CODE_GUIDE.md explains
# the whole project.
DISTFILES += \
    CODE_GUIDE.md \
    sql/schema.sql \
    sql/sample_data.sql \
    sql/queries.sql
