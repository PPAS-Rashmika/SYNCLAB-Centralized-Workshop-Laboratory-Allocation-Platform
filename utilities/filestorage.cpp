#include "filestorage.h"

#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QRegularExpression>
#include <QSaveFile>
#include <QStandardPaths>
#include <QUuid>

namespace
{
// A short random text used to give every saved PDF a unique file name, so a
// second attachment never overwrites the first one.
QString randomText()
{
    return QUuid::createUuid().toString(QUuid::Id128).toUpper();
}
} // namespace

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

QString savePdfAttachment(const QString &sourcePath, const QString &allocationKey,
                          const QString &dataDirectory, QString &error)
{
    error.clear();
    if (sourcePath.isEmpty())
        return QString();

    // Check that the chosen file really is a PDF: every PDF file starts with "%PDF-".
    QFile input(sourcePath);
    if (!input.open(QIODevice::ReadOnly) || input.peek(5) != "%PDF-") {
        error = "Select a readable PDF document before saving.";
        return QString();
    }

    const QString folder = QDir(dataDirectory).filePath("uploads/pdfs");

    // The file is already inside the uploads folder, so there is nothing to copy.
    if (QFileInfo(sourcePath).canonicalPath() == QFileInfo(folder).canonicalFilePath())
        return QFileInfo(sourcePath).absoluteFilePath();

    if (!QDir().mkpath(folder)) {
        error = "The attachment folder could not be created.";
        return QString();
    }

    // Build the new file name from the allocation key, replacing any character
    // that is not allowed in a file name.
    QString safeName = allocationKey;
    safeName.replace(QRegularExpression("[^A-Za-z0-9_-]"), "_");
    const QString destination =
        QDir(folder).filePath(safeName + "-" + randomText().left(12) + ".pdf");

    // QSaveFile writes to a temporary file first and only replaces the real file
    // when everything succeeded, so a failed copy can never destroy an
    // attachment that is already stored.
    QSaveFile output(destination);
    bool ok = output.open(QIODevice::WriteOnly);
    while (ok && !input.atEnd()) {
        const QByteArray block = input.read(65536); // copy 64 KB at a time
        if (block.isEmpty() && input.error() != QFile::NoError) {
            ok = false;
            break;
        }
        ok = output.write(block) == block.size();
    }
    if (!ok || !output.commit()) {
        error = "The PDF could not be saved. The existing attachment is unchanged.";
        return QString();
    }
    return destination;
}
