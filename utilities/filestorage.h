#ifndef FILESTORAGE_H
#define FILESTORAGE_H

#include <QString>

// ---------------------------------------------------------------------------
// Where the program keeps its files: the SQLite database and the uploaded PDFs.
// These are plain functions because there is no data to store in an object.
// ---------------------------------------------------------------------------

// The full path of the SQLite database file the program should use.
QString defaultDatabasePath();

// Copy a student's PDF into the uploads folder and return the path of the copy.
// On failure it returns an empty string and puts a message in 'error'.
QString savePdfAttachment(const QString &sourcePath, const QString &allocationKey,
                          const QString &dataDirectory, QString &error);

#endif // FILESTORAGE_H
