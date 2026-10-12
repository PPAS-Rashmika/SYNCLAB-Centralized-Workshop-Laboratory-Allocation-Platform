#ifndef ATTACHMENTSTORAGE_H
#define ATTACHMENTSTORAGE_H

#include <QString>

// File handling is separate from the form. The form decides how to display an error.
class AttachmentStorage
{
  public:
    static QString savePdf(const QString &sourcePath, const QString &allocationKey,
                           const QString &dataDirectory, QString &error);
};

#endif
