#ifndef DATABASEHELPERS_H
#define DATABASEHELPERS_H

#include <QString>
#include <QStringList>

// Internal functions shared by the database implementation files.
namespace DatabaseHelpers
{
QStringList splitSlots(const QString &value);
QString canonicalStatus(const QString &value);
bool validDate(const QString &value);
} // namespace DatabaseHelpers

#endif // DATABASEHELPERS_H
