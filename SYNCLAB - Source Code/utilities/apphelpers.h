#ifndef APPHELPERS_H
#define APPHELPERS_H

#include <QPixmap>
#include <QSize>
#include <QString>

// Small shared helpers: IDs, workshop images, and the default database location.
QString token();
QPixmap workshopImage(const QString &category, const QSize &size);
QString defaultDatabasePath();

#endif
