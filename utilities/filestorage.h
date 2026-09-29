#ifndef FILESTORAGE_H
#define FILESTORAGE_H

#include <QString>

// ---------------------------------------------------------------------------
// Where the program keeps its files.
// This is a plain function and not a class, because there is no data to store.
// ---------------------------------------------------------------------------

// The full path of the SQLite database file the program should use.
QString defaultDatabasePath();

#endif // FILESTORAGE_H
