#include "filestorage.h"

#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>
#include <QStandardPaths>

QString defaultDatabasePath()
{
    // If a database file already sits next to the program, keep using it so an
    // existing installation never loses its saved bookings.
    for (const QString &folder : {QDir::currentPath(), QCoreApplication::applicationDirPath()}) {
        const QString path = QDir(folder).filePath("student_data.db");
        if (QFileInfo::exists(path))
            return path;
    }

    // A new installation uses a writable folder inside the user's profile.
    const QString folder = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation);
    QDir().mkpath(folder);
    return QDir(folder).filePath("student_data.db");
}
