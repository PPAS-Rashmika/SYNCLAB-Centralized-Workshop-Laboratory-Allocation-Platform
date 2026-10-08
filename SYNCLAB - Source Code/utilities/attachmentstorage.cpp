#include "attachmentstorage.h"
#include "apphelpers.h"
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QRegularExpression>
#include <QSaveFile>

QString AttachmentStorage::savePdf(const QString &sourcePath, const QString &allocationKey,
                                   const QString &dataDirectory, QString &error)
{
    error.clear();
    if (sourcePath.isEmpty())
        return {};
    QFile input(sourcePath);
    if (!input.open(QIODevice::ReadOnly) || input.peek(5) != "%PDF-") {
        error = "Select a readable PDF document before saving.";
        return {};
    }
    const QString folder = QDir(dataDirectory).filePath("uploads/pdfs");
    if (QFileInfo(sourcePath).canonicalPath() == QFileInfo(folder).canonicalFilePath())
        return QFileInfo(sourcePath).absoluteFilePath();
    if (!QDir().mkpath(folder)) {
        error = "The attachment folder could not be created.";
        return {};
    }
    QString safeName = allocationKey;
    safeName.replace(QRegularExpression("[^A-Za-z0-9_-]"), "_");
    const QString destination = QDir(folder).filePath(safeName + "-" + token().left(12) + ".pdf");
    QSaveFile output(destination);
    bool ok = output.open(QIODevice::WriteOnly);
    while (ok && !input.atEnd()) {
        const QByteArray block = input.read(65536);
        if (block.isEmpty() && input.error() != QFile::NoError) {
            ok = false;
            break;
        }
        ok = output.write(block) == block.size();
    }
    if (!ok || !output.commit()) {
        error = "The PDF could not be saved. The existing attachment is unchanged.";
        return {};
    }
    return destination;
}
