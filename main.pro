# ----------------------------------------------------
# This file is generated for the Student Workshop App
# ----------------------------------------------------

# Core Qt modules required for the application
QT       += core gui sql

# Enable QtWidgets for standard UI components (Qt 5 and Qt 6 compatibility)
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

# Use modern C++ standard
CONFIG += c++17

# You can change the compiled executable name here
TARGET = StudentWorkshopApp
TEMPLATE = app

# Source files
SOURCES += \
    main.cpp \
    mainwindow.cpp

# Header files
HEADERS += \
    mainwindow.h