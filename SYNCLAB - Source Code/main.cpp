// මෙතැනින් program එක ආරම්භ වේ. මුල් font එක, icon එක සහ window එක මෙහි සකසයි.
// #include වලින් මේ file එකට අවශ්‍ය Qt සහ project declarations ලබාගනී.
#include "mainwindow.h"
#include <QApplication>
#include <QFont>
#include <QIcon>

// Qt application එක හදා MainWindow පෙන්වයි. exec() මගින් button events එනතුරු program එක ක්‍රියා කරයි.
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    // QSettings save කරන තැනට මෙම නම් භාවිත වේ; කලින් settings කියවීමට මේ නම් අවශ්‍යයි.
    QCoreApplication::setOrganizationName("StudentWorkshopApp");
    QCoreApplication::setApplicationName("SYNCLAB");
    // Window/application නම වෙනස් කිරීමට මේ text එක වෙනස් කරන්න.
    QApplication::setApplicationDisplayName(
        "SYNCLAB | Centralized Workshop & Laboratory Allocation Platform");
    // Logo resource alias එකයි; සැබෑ image path එක resources/images.qrc හි ඇත.
    QApplication::setWindowIcon(QIcon(":/branding/logo.png"));
    // Qt controls වල එකම මූලික පෙනුම ලබාදෙයි.
    a.setStyle("Fusion");
    // මුල් font සහ size මෙහි වෙනස් කරන්න; .ui file එකේ fonts තිබේ නම් ඒවා භාවිත වේ.
    a.setFont(QFont("Segoe UI", 10));
    MainWindow w;
    // ආරම්භයේ window එක screen එකේ උපරිම ප්‍රමාණයෙන් පෙන්වයි.
    w.showMaximized();
    return a.exec();
}
