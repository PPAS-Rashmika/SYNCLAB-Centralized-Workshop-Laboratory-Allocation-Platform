#ifndef WORKSHOPCATALOG_H
#define WORKSHOPCATALOG_H

#include <QPixmap>
#include <QSize>
#include <QString>
#include <QStringList>

// ---------------------------------------------------------------------------
// Information about the workshops this program offers.
// These are plain functions and not a class, because there is no data to
// store. They only return fixed information about the workshops.
// ---------------------------------------------------------------------------

// The names of all workshops, in the order they appear on the home page.
QStringList workshopCategories();

// The heading shown for a workshop, for example "Physics Lab".
QString workshopTitle(const QString &category);

// The department that runs the workshop.
QString workshopDepartment(const QString &category);

// The picture shown on a workshop card.
// It uses assets/<workshop>.png when that file exists. Otherwise it draws a
// simple coloured badge with the initials of the workshop.
QPixmap workshopImage(const QString &category, const QSize &size);

#endif // WORKSHOPCATALOG_H
