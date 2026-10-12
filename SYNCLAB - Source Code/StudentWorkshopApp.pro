# Qt Creator තුළ මෙම file එක විවෘත කර Qt 6 Desktop kit එකෙන් build/run කරන්න.
# අවශ්‍ය Qt modules: core මූලික වැඩ, gui රූප/font, widgets UI, sql SQLite දත්ත.
QT += core gui widgets sql
# Qt 6 ට අඩු kit එකක් තෝරා ඇත්නම් build එක නවතයි.
lessThan(QT_MAJOR_VERSION, 6): error("This project requires Qt 6 or newer.")
# මෙම project එක C++17 භාවිත කරයි.
CONFIG += c++17
# Library එකක් වෙනුවට ධාවනය කළ හැකි application එකක් build කරයි.
TEMPLATE = app
# Build වූ executable file එකේ නමයි.
TARGET = StudentWorkshopApp

# Project folder path එක SOURCE_DIR ලෙස C++ වෙත දෙයි; database data folder එකට එය භාවිත වේ.
DEFINES += "\"SOURCE_DIR=\\\"$$PWD\\\"\""

# Header files සොයන folders. අලුත් folder එකක header එකක් එක් කළොත් මෙහි එක් කරන්න.
INCLUDEPATH += \
    $$PWD/ui \
    $$PWD/ui/dialogs \
    $$PWD/ui/widgets \
    $$PWD/models \
    $$PWD/database \
    $$PWD/logic \
    $$PWD/utilities

# Compile කරන .cpp files ලැයිස්තුව. අලුත් implementation file එකක් මෙහි එක් කරන්න.
SOURCES += \
    main.cpp \
    ui/mainwindow.cpp \
    ui/designertheme.cpp \
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

# Class declarations ඇති .h files. Q_OBJECT ඇති headers moc සඳහාද මෙහි ලැයිස්තුගත වේ.
HEADERS += \
    ui/mainwindow.h \
    ui/designertheme.h \
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

# Qt Designer .ui files. uic මගින් ui_<name>.h files සාදයි; ඒ generated files අතින් edit නොකරයි.
FORMS += \
    ui/forms/welcome.ui \
    ui/forms/selection.ui \
    ui/forms/calander.ui \
    ui/forms/form.ui \
    ui/forms/admin.ui \
    ui/forms/customalertdialog.ui \
    ui/forms/custominputdialog.ui \
    ui/forms/adminsettingsdialog.ui \
    ui/forms/systemsettingsdialog.ui

# Logo සහ workshop images executable එකට ඇතුළත් කරන .qrc file එක.
RESOURCES += resources/images.qrc

