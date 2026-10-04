// This is the entry point of the program.
// It creates the Qt application object, shows the main window and starts the
// event loop that waits for mouse clicks and key presses.
#include "mainwindow.h"
#include <QApplication>
#include <QFont>
#include <QIcon>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    // These two names decide where QSettings saves its files on the computer.
    QCoreApplication::setOrganizationName("StudentWorkshopApp");
    QCoreApplication::setApplicationName("SYNCLAB");
    QApplication::setApplicationDisplayName(
        "SYNCLAB | Centralized Workshop & Laboratory Allocation Platform");
    QApplication::setWindowIcon(QIcon(":/branding/logo.png"));
    // Fusion gives the same look on Windows, Linux and macOS.
    a.setStyle("Fusion");
    // This is the fallback for Qt's internal controls. Designer font properties
    // on individual widgets remain editable and take precedence.
    a.setFont(QFont("Segoe UI", 10));
    MainWindow w;
    w.showMaximized();
    return a.exec();
}
