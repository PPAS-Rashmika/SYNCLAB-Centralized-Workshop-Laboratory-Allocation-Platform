#include "databasemanager.h"
#include "workshopcatalog.h"

#include <QFileInfo>
#include <QMap>
#include <QRegularExpression>
#include <QSet>
#include <QSqlError>
#include <QSqlQuery>
#include <QUuid>
#include <QVariant>

// ---------------------------------------------------------------------------
// Small helper functions. They are only used inside this file, so they live in
// an unnamed namespace instead of needing their own header file.
// ---------------------------------------------------------------------------
namespace
{
// "8.30 - 9.30, 9.30 - 10.30" becomes a clean list with no blanks and no repeats.
QStringList splitSlots(const QString &value)
{
    QStringList slotList;
    for (const QString &part : value.split(',')) {
        const QString slot = part.trimmed();
        if (!slot.isEmpty() && !slotList.contains(slot))
            slotList.append(slot);
    }
    return slotList;
}

// The program only ever stores three statuses. Anything else is refused.
QString canonicalStatus(const QString &value)
{
    const QString status = value.trimmed().toLower();
    if (status == "pending")
        return "Pending";
    if (status == "approve" || status == "approved")
        return "Approve";
    if (status == "reject" || status == "rejected")
        return "Reject";
    return QString();
}

// Allocation dates are stored as text in ISO form, for example 2026-09-29.
bool validDate(const QString &value)
{
    const QDate date = QDate::fromString(value, Qt::ISODate);
    return date.isValid() && date.toString(Qt::ISODate) == value;
}

// Assemble a slot list only when reading for the UI. Stored slots remain atomic.
const QString slotListSql =
    "(SELECT group_concat(time_slot, ', ') FROM "
    "(SELECT time_slot FROM booking_slots WHERE record_id = r.id ORDER BY "
    "CASE time_slot WHEN '8.30 am - 10.30 am' THEN 0 "
    "WHEN '10.30 am - 12.30 pm' THEN 1 WHEN '1.30 pm - 3.30 pm' THEN 2 "
    "WHEN '3.30 pm - 5.30 pm' THEN 3 ELSE 4 END, time_slot))";

// The columns every SELECT below asks for.
// Important: this order must match the value(0)..value(15) order in readRecord().
const QString recordColumns =
    "r.id, r.category, s.name, r.index_no, s.department, s.email, r.record_date, " +
    slotListSql + ", r.description, r.passcode, r.created_at, r.status, r.admin_remarks, "
    "r.category, r.supervisor_name, r.project_name";
const QString recordJoins =
    " FROM records r JOIN students s ON s.index_no = r.index_no ";

QSet<QString> tableColumns(QSqlDatabase &db, const QString &table)
{
    QSet<QString> columns;
    QSqlQuery query(db);
    // table is always an internal constant, never user input.
    if (query.exec("PRAGMA table_info(" + table + ")")) {
        while (query.next())
            columns.insert(query.value(1).toString().toLower());
    }
    return columns;
}

// The same definitions create fresh databases and migration staging tables.
QStringList schemaStatements(const QString &prefix = QString())
{
    return {
        "CREATE TABLE " + prefix + "workshops (category TEXT NOT NULL PRIMARY KEY, "
        "department TEXT NOT NULL)",
        "CREATE TABLE " + prefix + "students (index_no TEXT NOT NULL PRIMARY KEY, "
        "name TEXT NOT NULL, department TEXT NOT NULL, email TEXT NOT NULL)",
        "CREATE TABLE " + prefix + "records (id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "category TEXT NOT NULL REFERENCES " + prefix + "workshops(category), "
        "index_no TEXT NOT NULL REFERENCES " + prefix + "students(index_no), "
        "supervisor_name TEXT NOT NULL, project_name TEXT NOT NULL, record_date TEXT NOT NULL, "
        "description TEXT, passcode TEXT NOT NULL, created_at TEXT NOT NULL, "
        "status TEXT NOT NULL DEFAULT 'Pending' CHECK(status IN ('Pending', 'Approve', 'Reject')), "
        "admin_remarks TEXT)",
        "CREATE TABLE " + prefix + "booking_slots (record_id INTEGER NOT NULL REFERENCES " +
        prefix + "records(id) ON DELETE CASCADE, time_slot TEXT NOT NULL "
        "CHECK(length(trim(time_slot)) > 0 AND instr(time_slot, ',') = 0), "
        "PRIMARY KEY(record_id, time_slot))"
    };
}

// Turn one SQL row into a StudentRecord object.
StudentRecord readRecord(const QSqlQuery &query)
{
    StudentRecord record;
    record.setId(query.value(0).toInt());
    record.setWorkshopId(query.value(1).toString());
    record.setName(query.value(2).toString());
    record.setIndexNo(query.value(3).toString());
    record.setDepartment(query.value(4).toString());
    record.setEmail(query.value(5).toString());
    record.setRecordDate(query.value(6).toString());
    record.setTimeSlots(query.value(7).toString());
    record.setDescription(query.value(8).toString());
    record.setPasscode(query.value(9).toString());
    record.setCreatedAt(query.value(10).toString());
    record.setStatus(query.value(11).toString());
    record.setAdminRemarks(query.value(12).toString());
    record.setCategory(query.value(13).toString());
    record.setSupervisorName(query.value(14).toString());
    record.setProjectName(query.value(15).toString());
    return record;
}
} // namespace

// ---------------------------------------------------------------------------
// Opening and closing the database
// ---------------------------------------------------------------------------

// Constructor: open the SQLite database file whose path was given.
SqliteDatabaseManager::SqliteDatabaseManager(const QString &dbName)
    : connectionName("workshop-" + QUuid::createUuid().toString(QUuid::WithoutBraces))
{
    // Ask Qt for the SQLite driver. The unique connection name means this
    // object never clashes with another connection in the same program.
    db = QSqlDatabase::addDatabase("QSQLITE", connectionName);
    db.setConnectOptions("QSQLITE_BUSY_TIMEOUT=5000"); // wait 5 seconds if the file is locked
    db.setDatabaseName(dbName);
    if (!db.open()) {
        lastError = db.lastError().text();
    } else {
        QSqlQuery query(db);
        if (!query.exec("PRAGMA foreign_keys = ON")) {
            lastError = query.lastError().text();
            db.close();
        }
    }
}

// Destructor: close the connection and hand the name back to Qt.
SqliteDatabaseManager::~SqliteDatabaseManager()
{
    db.close();
    db = QSqlDatabase();
    QSqlDatabase::removeDatabase(connectionName);
}

// Every operation starts here so the user always gets a clear message.
bool SqliteDatabaseManager::ensureOpen()
{
    lastError.clear();
    if (db.isOpen())
        return true;
    lastError = db.lastError().text();
    if (lastError.isEmpty())
        lastError = "The allocation database is not open.";
    return false;
}

// Start a transaction. A transaction groups a check and its write together,
// so an allocation can never be saved while its time slot is already taken.
bool SqliteDatabaseManager::beginWrite()
{
    if (!ensureOpen())
        return false;
    QSqlQuery query(db);
    if (query.exec("BEGIN IMMEDIATE"))
        return true;
    lastError = query.lastError().text();
    return false;
}

// Save the transaction. If saving fails, undo it so the database is unchanged.
bool SqliteDatabaseManager::commitWrite()
{
    if (db.commit())
        return true;
    lastError = db.lastError().text();
    db.rollback();
    return false;
}

// Check an allocation before it is written to the database.
bool SqliteDatabaseManager::validateRecord(const StudentRecord &record)
{
    if (record.getCategory().trimmed().isEmpty() || !validDate(record.getRecordDate()) ||
        splitSlots(record.getTimeSlots()).isEmpty()) {
        lastError = "Choose a workshop, a valid allocation date, and at least one time slot.";
        return false;
    }
    if (record.getName().trimmed().isEmpty() || record.getIndexNo().trimmed().isEmpty() ||
        record.getDepartment().trimmed().isEmpty() ||
        record.getSupervisorName().trimmed().isEmpty() ||
        record.getProjectName().trimmed().isEmpty()) {
        lastError = "Name, index number, department, supervisor, and project name are required.";
        return false;
    }

    // A simple pattern that accepts ordinary email addresses.
    static const QRegularExpression emailPattern(
        "^[A-Za-z0-9._%+-]+@[A-Za-z0-9.-]+\\.[A-Za-z]{2,}$");
    if (!emailPattern.match(record.getEmail().trimmed()).hasMatch()) {
        lastError = "Enter a valid email address.";
        return false;
    }
    return true;
}

// ---------------------------------------------------------------------------
// Creating the table
// ---------------------------------------------------------------------------

// Create four 4NF tables, or migrate a wide/hybrid older database atomically.
bool SqliteDatabaseManager::createTable()
{
    if (!ensureOpen())
        return false;

    const QSet<QString> oldColumns = tableColumns(db, "records");
    QSqlQuery query(db);
    if (!query.exec("PRAGMA user_version") || !query.next()) {
        lastError = query.lastError().text();
        return false;
    }
    const int version = query.value(0).toInt();
    query.finish();
    if (version > 4) {
        lastError = "This database was created by a newer version of SYNCLAB.";
        return false;
    }
    if (version == 4) {
        const QSet<QString> recordFields = {"id", "category", "index_no", "supervisor_name",
                                            "project_name", "record_date", "description", "passcode",
                                            "created_at", "status", "admin_remarks"};
        if (oldColumns != recordFields ||
            tableColumns(db, "students") != QSet<QString>{"index_no", "name", "department", "email"} ||
            tableColumns(db, "workshops") != QSet<QString>{"category", "department"} ||
            tableColumns(db, "booking_slots") != QSet<QString>{"record_id", "time_slot"}) {
            lastError = "The allocation database schema is incomplete. Restore a compatible backup.";
            return false;
        }
        if (!query.exec("PRAGMA foreign_key_check")) {
            lastError = query.lastError().text();
            return false;
        }
        if (query.next()) {
            lastError = "The allocation database contains a broken relationship. Restore a compatible backup.";
            return false;
        }
        return true;
    }

    const bool migrating = !oldColumns.isEmpty();
    if (migrating && !oldColumns.contains("id")) {
        lastError = "The records table has no ID column. Restore a compatible allocation database.";
        return false;
    }

    // SQLite makes a consistent backup including any committed WAL contents.
    // Never overwrite an earlier backup. In-memory test databases need no file.
    if (migrating && db.databaseName() != ":memory:") {
        QString backupPath = db.databaseName() + ".pre-4nf.bak";
        for (int suffix = 1; QFileInfo::exists(backupPath); ++suffix)
            backupPath = db.databaseName() + ".pre-4nf." + QString::number(suffix) + ".bak";
        query.prepare("VACUUM INTO :backup");
        query.bindValue(":backup", backupPath);
        if (!query.exec()) {
            lastError = "Cannot back up the allocation database: " + query.lastError().text();
            return false;
        }
        query.finish();
    }

    if (!beginWrite())
        return false;

    auto fail = [this](const QString &message) {
        lastError = message;
        db.rollback();
        return false;
    };

    const QString prefix = migrating ? "_4nf_" : "";
    for (const QString &statement : schemaStatements(prefix)) {
        if (!query.exec(statement))
            return fail(query.lastError().text());
    }

    // Keep old master rows even if they do not currently have a booking.
    const QSet<QString> studentColumns = tableColumns(db, "students");
    const QSet<QString> workshopColumns = tableColumns(db, "workshops");
    if (migrating && workshopColumns.contains("category")) {
        const QString department = workshopColumns.contains("department")
                                       ? "COALESCE(department, '')" : "''";
        if (!query.exec("INSERT INTO _4nf_workshops SELECT category, " + department +
                        " FROM workshops WHERE category IS NOT NULL"))
            return fail(query.lastError().text());
    }
    if (migrating && studentColumns.contains("index_no")) {
        QStringList fields;
        for (const QString &field : {QString("name"), QString("department"), QString("email")})
            fields.append(studentColumns.contains(field) ? "COALESCE(" + field + ", '')" : "''");
        if (!query.exec("INSERT INTO _4nf_students SELECT index_no, " + fields.join(", ") +
                        " FROM students WHERE index_no IS NOT NULL"))
            return fail(query.lastError().text());
    }

    // Catalog department describes the laboratory, never the student's faculty.
    for (const QString &category : workshopCategories()) {
        query.prepare("INSERT INTO " + prefix + "workshops(category, department) VALUES(?, ?) "
                      "ON CONFLICT(category) DO UPDATE SET department=excluded.department");
        query.addBindValue(category);
        query.addBindValue(workshopDepartment(category));
        if (!query.exec())
            return fail(query.lastError().text());
    }

    if (migrating) {
        qlonglong previousSequence = 0;
        if (query.exec("SELECT seq FROM sqlite_sequence WHERE name='records'") && query.next())
            previousSequence = query.value(0).toLongLong();
        query.finish();

        // Some older files have both normalized tables and empty legacy columns.
        // Prefer nonempty booking values, falling back to the stored student.
        auto oldValue = [&oldColumns](const QString &column) {
            // Optional notes retain SQL NULL as well as their text contents.
            if (column == "description" || column == "admin_remarks")
                return oldColumns.contains(column) ? "r." + column : QString("NULL");
            return oldColumns.contains(column) ? "COALESCE(r." + column + ", '')" : QString("''");
        };
        const bool hasStudents = studentColumns.contains("index_no") && oldColumns.contains("index_no");
        auto studentValue = [&](const QString &column) {
            const QString legacy = oldValue(column);
            if (hasStudents && studentColumns.contains(column))
                return "COALESCE(NULLIF(" + legacy + ", ''), s." + column + ", '')";
            return legacy;
        };
        const QString categoryValue = "COALESCE(NULLIF(" + oldValue("category") + ", ''), " +
                                      oldValue("workshop_id") + ")";
        const QStringList values = {"r.id", categoryValue, studentValue("name"), oldValue("index_no"),
                                    studentValue("department"), studentValue("email"),
                                    oldValue("record_date"), oldValue("time_slot"), oldValue("description"),
                                    oldValue("passcode"), oldValue("created_at"), oldValue("status"),
                                    oldValue("admin_remarks"), categoryValue,
                                    oldValue("supervisor_name"), oldValue("project_name")};
        const QString join = hasStudents ? " LEFT JOIN students s ON s.index_no=r.index_no" : "";
        if (!query.exec("SELECT " + values.join(", ") + " FROM records r" + join + " ORDER BY r.id"))
            return fail(query.lastError().text());
        QList<StudentRecord> legacyRecords;
        while (query.next())
            legacyRecords.append(readRecord(query));
        query.finish();

        QMap<int, QStringList> existingSlots;
        const QSet<QString> slotColumns = tableColumns(db, "booking_slots");
        if (slotColumns.contains("record_id") && slotColumns.contains("time_slot")) {
            if (!query.exec("SELECT record_id, time_slot FROM booking_slots ORDER BY record_id, time_slot"))
                return fail(query.lastError().text());
            while (query.next())
                existingSlots[query.value(0).toInt()].append(query.value(1).toString());
            query.finish();
        }

        for (StudentRecord record : legacyRecords) {
            QString category = record.getCategory().trimmed();
            if (category.isEmpty())
                category = "LEGACY-MISSING-WORKSHOP-" + QString::number(record.getId());
            QString index = record.getIndexNo().trimmed();
            if (index.isEmpty())
                index = "LEGACY-MISSING-INDEX-" + QString::number(record.getId());
            query.prepare("INSERT INTO _4nf_workshops(category, department) VALUES(?, ?) "
                          "ON CONFLICT(category) DO NOTHING");
            query.addBindValue(category);
            query.addBindValue(workshopDepartment(category).isNull() ? QString("") : workshopDepartment(category));
            if (!query.exec())
                return fail(query.lastError().text());

            // Ascending IDs mean the newest booking supplies the current profile.
            query.prepare("INSERT INTO _4nf_students(index_no, name, department, email) VALUES(?, ?, ?, ?) "
                          "ON CONFLICT(index_no) DO UPDATE SET name=excluded.name, "
                          "department=excluded.department, email=excluded.email");
            query.addBindValue(index);
            query.addBindValue(record.getName().trimmed());
            query.addBindValue(record.getDepartment().trimmed());
            query.addBindValue(record.getEmail().trimmed());
            if (!query.exec())
                return fail(query.lastError().text());

            QString status = canonicalStatus(record.getStatus());
            if (status.isEmpty())
                status = "Pending";
            query.prepare("INSERT INTO _4nf_records(id, category, index_no, supervisor_name, project_name, "
                          "record_date, description, passcode, created_at, status, admin_remarks) "
                          "VALUES(?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");
            query.addBindValue(record.getId());
            query.addBindValue(category);
            query.addBindValue(index);
            query.addBindValue(record.getSupervisorName());
            query.addBindValue(record.getProjectName());
            query.addBindValue(record.getRecordDate());
            query.addBindValue(record.getDescription());
            query.addBindValue(record.getPasscode());
            query.addBindValue(record.getCreatedAt());
            query.addBindValue(status);
            query.addBindValue(record.getAdminRemarks());
            if (!query.exec())
                return fail(query.lastError().text());

            QStringList slotList = splitSlots(record.getTimeSlots());
            if (slotList.isEmpty())
                slotList = splitSlots(existingSlots.value(record.getId()).join(','));
            if (slotList.isEmpty())
                return fail("Allocation " + QString::number(record.getId()) +
                            " has no time slots. The original database was retained; repair this allocation first.");
            for (const QString &slot : slotList) {
                query.prepare("INSERT INTO _4nf_booking_slots(record_id, time_slot) VALUES(?, ?)");
                query.addBindValue(record.getId());
                query.addBindValue(slot);
                if (!query.exec())
                    return fail(query.lastError().text());
            }
        }

        // Drop children before parents while foreign-key enforcement stays on.
        for (const QString &table : {QString("booking_slots"), QString("records"),
                                     QString("students"), QString("workshops")}) {
            if (!query.exec("DROP TABLE IF EXISTS " + table))
                return fail(query.lastError().text());
        }
        // SQLite updates the referencing foreign keys when each parent is renamed.
        for (const QString &table : {QString("workshops"), QString("students"),
                                     QString("records"), QString("booking_slots")}) {
            if (!query.exec("ALTER TABLE _4nf_" + table + " RENAME TO " + table))
                return fail(query.lastError().text());
        }
        // A completely empty legacy table has no staging sequence row yet.
        query.prepare("INSERT INTO sqlite_sequence(name, seq) SELECT 'records', ? "
                      "WHERE NOT EXISTS(SELECT 1 FROM sqlite_sequence WHERE name='records')");
        query.addBindValue(previousSequence);
        if (!query.exec())
            return fail(query.lastError().text());
        query.prepare("UPDATE sqlite_sequence SET seq = max(seq, ?) WHERE name='records'");
        query.addBindValue(previousSequence);
        if (!query.exec())
            return fail(query.lastError().text());
    }

    if (!query.exec("CREATE INDEX records_category_date ON records(category, record_date)"))
        return fail(query.lastError().text());
    if (!query.exec("PRAGMA foreign_key_check"))
        return fail(query.lastError().text());
    if (query.next()) {
        query.finish();
        return fail("The database migration found a broken relationship. The original database was retained.");
    }
    query.finish();
    if (!query.exec("PRAGMA user_version = 4"))
        return fail(query.lastError().text());
    return commitWrite();
}

// Store each workshop and each student once. Profile edits affect all bookings
// for that index number. The caller's transaction also contains the booking.
bool SqliteDatabaseManager::saveRelatedFacts(const StudentRecord &record)
{
    QSqlQuery query(db);
    query.prepare("INSERT INTO workshops(category, department) VALUES(?, ?) "
                  "ON CONFLICT(category) DO NOTHING");
    query.addBindValue(record.getCategory());
    const QString department = workshopDepartment(record.getCategory());
    query.addBindValue(department.isNull() ? QString("") : department);
    if (!query.exec()) {
        lastError = query.lastError().text();
        return false;
    }
    query.prepare("INSERT INTO students(index_no, name, department, email) VALUES(?, ?, ?, ?) "
                  "ON CONFLICT(index_no) DO UPDATE SET name=excluded.name, "
                  "department=excluded.department, email=excluded.email");
    query.addBindValue(record.getIndexNo().trimmed());
    query.addBindValue(record.getName().trimmed());
    query.addBindValue(record.getDepartment().trimmed());
    query.addBindValue(record.getEmail().trimmed());
    if (!query.exec()) {
        lastError = query.lastError().text();
        return false;
    }
    return true;
}

bool SqliteDatabaseManager::replaceSlots(int recordId, const QStringList &slotList)
{
    QSqlQuery query(db);
    query.prepare("DELETE FROM booking_slots WHERE record_id = ?");
    query.addBindValue(recordId);
    if (!query.exec()) {
        lastError = query.lastError().text();
        return false;
    }
    for (const QString &slot : slotList) {
        query.prepare("INSERT INTO booking_slots(record_id, time_slot) VALUES(?, ?)");
        query.addBindValue(recordId);
        query.addBindValue(slot);
        if (!query.exec()) {
            lastError = query.lastError().text();
            return false;
        }
    }
    return true;
}

// ---------------------------------------------------------------------------
// Writing: INSERT, UPDATE and DELETE
// ---------------------------------------------------------------------------

// Insert one new allocation.
bool SqliteDatabaseManager::addRecord(const StudentRecord &record)
{
    if (!ensureOpen() || !validateRecord(record))
        return false;

    const QString status = canonicalStatus(record.getStatus());
    if (record.getPasscode().trimmed().isEmpty() || status.isEmpty()) {
        lastError = "An allocation key and a valid allocation status are required.";
        return false;
    }
    if (!beginWrite())
        return false;

    QSqlQuery query(db);

    // A prepared query keeps the SQL text and the values separate, which stops
    // a value from being read as SQL code.
    query.prepare("SELECT id FROM records WHERE passcode = :passcode LIMIT 1");
    query.bindValue(":passcode", record.getPasscode());
    if (!query.exec()) {
        lastError = query.lastError().text();
        db.rollback();
        return false;
    }
    const bool duplicateKey = query.next();
    query.finish();
    if (duplicateKey) {
        lastError = "This allocation key already exists. Please submit the allocation again.";
        db.rollback();
        return false;
    }

    // A rejected allocation does not hold its time slots, so it is not checked.
    if (status != "Reject" &&
        !isTimeSlotAvailable(record.getRecordDate(), record.getCategory(),
                             splitSlots(record.getTimeSlots()))) {
        db.rollback();
        return false;
    }

    if (!saveRelatedFacts(record)) {
        db.rollback();
        return false;
    }

    // INSERT only the facts belonging to this booking.
    query.prepare("INSERT INTO records (category, index_no, supervisor_name, project_name, "
                  "record_date, description, "
                  "passcode, created_at, status, admin_remarks) "
                  "VALUES (:category, :index, :supervisor, :project, "
                  ":date, :desc, :passcode, :created_at, :status, :remarks)");
    query.bindValue(":category", record.getCategory());
    query.bindValue(":index", record.getIndexNo().trimmed());
    query.bindValue(":supervisor", record.getSupervisorName().trimmed());
    query.bindValue(":project", record.getProjectName().trimmed());
    query.bindValue(":date", record.getRecordDate());
    query.bindValue(":desc", record.getDescription());
    query.bindValue(":passcode", record.getPasscode());
    query.bindValue(":created_at", record.getCreatedAt());
    query.bindValue(":status", status);
    query.bindValue(":remarks", record.getAdminRemarks());
    if (!query.exec()) {
        lastError = query.lastError().text();
        db.rollback();
        return false;
    }
    const int recordId = query.lastInsertId().toInt();
    query.finish();
    if (!replaceSlots(recordId, splitSlots(record.getTimeSlots()))) {
        db.rollback();
        return false;
    }
    return commitWrite();
}

// Save changes to an existing allocation.
// The allocation key, the created date and the admin remarks are never changed here.
bool SqliteDatabaseManager::updateRecord(const StudentRecord &record)
{
    if (!ensureOpen() || !validateRecord(record))
        return false;

    const QString status = canonicalStatus(record.getStatus());
    if (status.isEmpty()) {
        lastError = "Choose Pending, Approve, or Reject as the allocation status.";
        return false;
    }
    if (!beginWrite())
        return false;

    QSqlQuery query(db);

    // First make sure the allocation is still in the database.
    query.prepare("SELECT id FROM records WHERE id = :id");
    query.bindValue(":id", record.getId());
    if (!query.exec()) {
        lastError = query.lastError().text();
        db.rollback();
        return false;
    }
    const bool found = query.next();
    query.finish();
    if (!found) {
        lastError = "The allocation no longer exists. Refresh the allocation list.";
        db.rollback();
        return false;
    }

    // The allocation being edited must not block its own time slots,
    // so its id is passed in and skipped.
    if (status != "Reject" &&
        !isTimeSlotAvailable(record.getRecordDate(), record.getCategory(),
                             splitSlots(record.getTimeSlots()), record.getId())) {
        db.rollback();
        return false;
    }

    if (!saveRelatedFacts(record)) {
        db.rollback();
        return false;
    }

    // UPDATE the booking, then replace its slot rows in the same transaction.
    query.prepare("UPDATE records SET category=:category, index_no=:index, "
                  "supervisor_name=:supervisor, project_name=:project, record_date=:date, "
                  "description=:desc, status=:status WHERE id=:id");
    query.bindValue(":category", record.getCategory());
    query.bindValue(":index", record.getIndexNo().trimmed());
    query.bindValue(":supervisor", record.getSupervisorName().trimmed());
    query.bindValue(":project", record.getProjectName().trimmed());
    query.bindValue(":date", record.getRecordDate());
    query.bindValue(":desc", record.getDescription());
    query.bindValue(":status", status);
    query.bindValue(":id", record.getId());
    if (!query.exec()) {
        lastError = query.lastError().text();
        db.rollback();
        return false;
    }
    query.finish();
    if (!replaceSlots(record.getId(), splitSlots(record.getTimeSlots()))) {
        db.rollback();
        return false;
    }
    return commitWrite();
}

// Delete one allocation by its id.
bool SqliteDatabaseManager::deleteRecord(int id)
{
    if (!ensureOpen())
        return false;

    QSqlQuery query(db);
    query.prepare("DELETE FROM records WHERE id = :id");
    query.bindValue(":id", id);
    if (!query.exec()) {
        lastError = query.lastError().text();
        return false;
    }
    if (query.numRowsAffected() != 1) {
        lastError = "The allocation no longer exists. Refresh the allocation list.";
        return false;
    }
    return true;
}

// Change only the status of an allocation (Pending, Approve or Reject).
bool SqliteDatabaseManager::updateRecordStatus(int id, const QString &value)
{
    if (!ensureOpen())
        return false;

    const QString status = canonicalStatus(value);
    if (status.isEmpty()) {
        lastError = "Choose Pending, Approve, or Reject as the allocation status.";
        return false;
    }
    if (!beginWrite())
        return false;

    QSqlQuery query(db);

    // Read the date, workshop and slots of this allocation so they can be re-checked.
    query.prepare("SELECT r.record_date, r.category, " + slotListSql +
                  " FROM records r WHERE r.id = :id");
    query.bindValue(":id", id);
    if (!query.exec()) {
        lastError = query.lastError().text();
        db.rollback();
        return false;
    }
    if (!query.next()) {
        lastError = "The allocation no longer exists. Refresh the allocation list.";
        query.finish();
        db.rollback();
        return false;
    }
    const QString date = query.value(0).toString();
    const QString category = query.value(1).toString();
    const QStringList slotList = splitSlots(query.value(2).toString());
    query.finish();

    // Approving an allocation gives it its slots back, so they must still be free.
    if (status != "Reject" && !isTimeSlotAvailable(date, category, slotList, id)) {
        db.rollback();
        return false;
    }

    query.prepare("UPDATE records SET status = :status WHERE id = :id");
    query.bindValue(":status", status);
    query.bindValue(":id", id);
    if (!query.exec()) {
        lastError = query.lastError().text();
        db.rollback();
        return false;
    }
    return commitWrite();
}

// Change only the administrator's remark for an allocation.
bool SqliteDatabaseManager::updateAdminRemarks(int id, const QString &remarks)
{
    if (!ensureOpen())
        return false;

    QSqlQuery query(db);
    query.prepare("UPDATE records SET admin_remarks = :remarks WHERE id = :id");
    query.bindValue(":remarks", remarks);
    query.bindValue(":id", id);
    if (!query.exec()) {
        lastError = query.lastError().text();
        return false;
    }
    if (query.numRowsAffected() != 1) {
        lastError = "The allocation no longer exists. Refresh the allocation list.";
        return false;
    }
    return true;
}

// ---------------------------------------------------------------------------
// Reading: SELECT
// ---------------------------------------------------------------------------

// All allocations for one workshop on one date, newest first.
QList<StudentRecord> SqliteDatabaseManager::getRecordsByDateAndCategory(const QString &date,
                                                                        const QString &category)
{
    QList<StudentRecord> records;
    if (!ensureOpen())
        return records;

    QSqlQuery query(db);
    query.prepare("SELECT " + recordColumns +
                  recordJoins + "WHERE r.record_date = :date AND r.category = :category "
                  "ORDER BY r.id DESC");
    query.bindValue(":date", date);
    query.bindValue(":category", category);
    if (!query.exec()) {
        lastError = query.lastError().text();
        return records;
    }
    while (query.next())
        records.append(readRecord(query));
    return records;
}

// The dates on which a workshop already has allocations that are not rejected.
QList<QDate> SqliteDatabaseManager::getAllocatedDates(const QString &category)
{
    QList<QDate> dates;
    if (!ensureOpen())
        return dates;

    QSqlQuery query(db);
    query.prepare("SELECT DISTINCT record_date FROM records WHERE category = :category "
                  "AND lower(trim(COALESCE(status, 'Pending'))) NOT IN ('reject', 'rejected') "
                  "ORDER BY record_date");
    query.bindValue(":category", category);
    if (!query.exec()) {
        lastError = query.lastError().text();
        return dates;
    }
    while (query.next()) {
        const QDate date = QDate::fromString(query.value(0).toString(), Qt::ISODate);
        if (date.isValid())
            dates.append(date);
    }
    return dates;
}

// True when none of the requested slots is already taken.
bool SqliteDatabaseManager::isTimeSlotAvailable(const QString &date, const QString &category,
                                                const QStringList &requestedSlots,
                                                int excludeRecordId)
{
    if (!ensureOpen())
        return false;

    const QStringList slotList = splitSlots(requestedSlots.join(','));
    if (!validDate(date) || category.trimmed().isEmpty() || slotList.isEmpty()) {
        lastError = "Choose a workshop, a valid allocation date, and at least one time slot.";
        return false;
    }

    const QStringList allocatedSlots = getAllocatedTimeSlotsStringList(date, category, excludeRecordId);
    if (!lastError.isEmpty())
        return false;

    for (const QString &slot : slotList) {
        if (allocatedSlots.contains(slot)) {
            lastError = "The selected time slot is already allocated. Choose another time slot.";
            return false;
        }
    }
    return true;
}

// Every slot already allocated for one workshop on one date.
QStringList SqliteDatabaseManager::getAllocatedTimeSlotsStringList(const QString &date,
                                                                const QString &category,
                                                                int excludeRecordId)
{
    QStringList slotList;
    if (!ensureOpen())
        return slotList;

    QSqlQuery query(db);
    query.prepare("SELECT DISTINCT bs.time_slot FROM booking_slots bs "
                  "JOIN records r ON r.id = bs.record_id "
                  "WHERE r.record_date = :date AND r.category = :category "
                  "AND r.id != :id AND r.status != 'Reject' ORDER BY bs.time_slot");
    query.bindValue(":date", date);
    query.bindValue(":category", category);
    query.bindValue(":id", excludeRecordId);
    if (!query.exec()) {
        lastError = query.lastError().text();
        return slotList;
    }
    while (query.next())
        slotList.append(query.value(0).toString());
    return slotList;
}

// Every allocation of one workshop, most recent date first.
QList<StudentRecord> SqliteDatabaseManager::getAllRecordsForCategory(const QString &category)
{
    QList<StudentRecord> records;
    if (!ensureOpen())
        return records;

    QSqlQuery query(db);
    query.prepare("SELECT " + recordColumns +
                  recordJoins + "WHERE r.category = :category ORDER BY r.record_date DESC, r.id DESC");
    query.bindValue(":category", category);
    if (!query.exec()) {
        lastError = query.lastError().text();
        return records;
    }
    while (query.next())
        records.append(readRecord(query));
    return records;
}

// The reason the last database operation failed.
QString SqliteDatabaseManager::getLastError() const
{
    return lastError;
}
