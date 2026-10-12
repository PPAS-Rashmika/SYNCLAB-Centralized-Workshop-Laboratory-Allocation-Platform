// Database file එක සොයාගන්නා තැනයි. Folder/file නම වෙනස් කිරීමට පහත dataFolder සහ dataPath බලන්න.
// #include වලින් මේ file එකට අවශ්‍ය Qt සහ project declarations ලබාගනී.
#include "filestorage.h"

#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>
#include <QStandardPaths>

// මුලින් තිබෙන database file එක භාවිත කරයි; නැතිනම් අලුත් data folder එකේ path එක දෙයි.
QString defaultDatabasePath()
{
    // SOURCE_DIR යනු .pro file එක compiler වෙත දෙන project folder එකයි.
    const QString dataFolder = QDir(QString(SOURCE_DIR)).filePath("data");
    const QString dataPath = QDir(dataFolder).filePath("student_data.db");
    if (QFileInfo::exists(dataPath))
        return dataPath;

    // පරණ database තිබේ නම් working folder, executable folder සහ user data folder අනුපිළිවෙළින් සොයයි.
    for (const QString &folder :
         {QDir::currentPath(), QCoreApplication::applicationDirPath(),
          QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation)}) {
        const QString path = QDir(folder).filePath("student_data.db");
        if (QFileInfo::exists(path))
            return path;
    }

    // Project data folder සාදන්න බැරි නම් user profile හි writable folder එක භාවිත කරයි.
    if (QDir().mkpath(dataFolder))
        return dataPath;
    const QString profile = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation);
    QDir().mkpath(profile);
    return QDir(profile).filePath("student_data.db");
}
