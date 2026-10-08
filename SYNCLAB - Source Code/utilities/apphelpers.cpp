#include "apphelpers.h"
#include "workshopcatalog.h"
#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>
#include <QPainter>
#include <QStandardPaths>
#include <QUuid>

QString token()
{
    return QUuid::createUuid().toString(QUuid::Id128).toUpper();
}
QPixmap workshopImage(const QString &category, const QSize &size)
{
    QPixmap supplied(QCoreApplication::applicationDirPath() + "/assets/" + category.toLower() +
                     ".png");
    if (!supplied.isNull())
        return supplied.scaled(size, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    QPixmap image(size * 2);
    image.setDevicePixelRatio(2);
    image.fill(Qt::transparent);
    QPainter painter(&image);
    painter.setRenderHint(QPainter::Antialiasing);
    const QColor accent = QColor::fromHsv(
        (210 + qMax(0, workshopCategories().indexOf(category)) * 19) % 360, 170, 195);
    QColor soft = accent;
    soft.setAlpha(24);
    const int diameter = qMin(size.width(), size.height()) - 8;
    QRect circle((size.width() - diameter) / 2, 4, diameter, diameter);
    painter.setPen(Qt::NoPen);
    painter.setBrush(soft);
    painter.drawRoundedRect(circle, 20, 20);
    QString initials;
    for (const QString &word : category.split(' ', Qt::SkipEmptyParts))
        initials += word.left(1);
    if (category == "SYNCLAB")
        initials = "SL";
    painter.setPen(accent);
    painter.setFont(QFont("Segoe UI", diameter / 4, QFont::Bold));
    painter.drawText(circle, Qt::AlignCenter, initials.left(2));
    return image;
}
QString defaultDatabasePath()
{
    // Preserve an existing installation's location. New installations use a writable user folder.
    for (const QString &folder : {QDir::currentPath(), QCoreApplication::applicationDirPath()}) {
        const QString path = QDir(folder).filePath("student_data.db");
        if (QFileInfo::exists(path))
            return path;
    }
    const QString folder = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation);
    QDir().mkpath(folder);
    return QDir(folder).filePath("student_data.db");
}
