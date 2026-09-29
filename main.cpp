#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QCoreApplication::setOrganizationName("StudentWorkshopApp");
    QCoreApplication::setApplicationName("SYNCLAB");
    a.setStyle("Fusion");
    MainWindow w;
    w.show();
    return a.exec();
}
