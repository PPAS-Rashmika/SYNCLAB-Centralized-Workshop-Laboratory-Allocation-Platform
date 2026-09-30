#include "filestorage.h"

#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>
#include <QStandardPaths>

QString defaultDatabasePath()
{
    // All input data lives in one "data" folder beside these source files.
    // The .pro file passes that folder to the compiler as SOURCE_DIR, so the
    // program finds it no matter which directory the exe is started from.
    const QString dataFolder = QDir(QString(SOURCE_DIR)).filePath("data");
    const QString dataPath = QDir(dataFolder).filePath("student_data.db");
    if (QFileInfo::exists(dataPath))
        return dataPath;

    // An older installation kept its database next to the program or in the
    // user's profile. Keep using that file so no saved allocation is lost; it can
    // simply be moved into the "data" folder.
    for (const QString &folder :
         {QDir::currentPath(), QCoreApplication::applicationDirPath(),
          QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation)}) {
        const QString path = QDir(folder).filePath("student_data.db");
        if (QFileInfo::exists(path))
            return path;
    }

    // A new installation: create the data folder. If the source folder is
    // read-only, fall back to a writable folder inside the user's profile.
    if (QDir().mkpath(dataFolder))
        return dataPath;
    const QString profile = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation);
    QDir().mkpath(profile);
    return QDir(profile).filePath("student_data.db");
}
