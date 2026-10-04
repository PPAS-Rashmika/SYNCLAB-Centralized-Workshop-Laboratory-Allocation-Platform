// SQLite tables සෑදීම, booking save/edit/delete කිරීම සහ දත්ත කියවීම මෙහි සිදු කරයි.
// #include වලින් මේ file එකට අවශ්‍ය Qt සහ project declarations ලබාගනී.
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

// නම් නැති namespace එකේ helpers මේ file එක තුළට පමණක් සීමා වේ.
namespace
{
// Comma වෙන් කළ slots පිරිසිදු කර හිස් සහ නැවත ඇති values ඉවත් කරයි.
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

// ලැබුණු status එක Pending, Approve හෝ Reject නමට ගළපයි; නොදන්නා අගයක් නම් හිස් අගයක් දෙයි.
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

// දිනය නිවැරදි YYYY-MM-DD අගයක්දැයි පරීක්ෂා කරයි.
bool validDate(const QString &value)
{
    const QDate date = QDate::fromString(value, Qt::ISODate);
    return date.isValid() && date.toString(Qt::ISODate) == value;
}

// Screen එකට පමණක් slots එක text list එකකට එකතු කරයි; database හි slot එකකට row එක බැගින් ඇත.
const QString slotListSql =
    "(SELECT group_concat(time_slot, ', ') FROM "
    "(SELECT time_slot FROM booking_slots WHERE record_id = r.id ORDER BY "
    "CASE time_slot WHEN '8.30 am - 10.30 am' THEN 0 "
    "WHEN '10.30 am - 12.30 pm' THEN 1 WHEN '1.30 pm - 3.30 pm' THEN 2 "
    "WHEN '3.30 pm - 5.30 pm' THEN 3 ELSE 4 END, time_slot))";

// SELECT කරන columns අනුපිළිවෙළයි; readRecord() හි value(index) ද ඒ අනුව ගළපන්න.
const QString recordColumns =
    "r.id, r.category, s.name, r.index_no, s.department, s.email, r.record_date, " +
    slotListSql + ", r.description, r.passcode, r.created_at, r.status, r.admin_remarks, "
    "r.category, r.supervisor_name, r.project_name";
// Booking එකේ index_no අනුව student profile එක සම්බන්ධ කරයි.
const QString recordJoins =
    " FROM records r JOIN students s ON s.index_no = r.index_no ";

// Table එකේ column නම් ලබාගනී; පැරණි database හැඳිනීමට භාවිත කරයි.
QSet<QString> tableColumns(QSqlDatabase &db, const QString &table)
{
    QSet<QString> columns;
    QSqlQuery query(db);
    if (query.exec("PRAGMA table_info(" + table + ")")) {
        while (query.next())
            columns.insert(query.value(1).toString().toLower());
    }
    return columns;
}

// Tables හතරේ SQL definitions මෙහි ඇත. Column වෙනස් කිරීම සහ migration එක එකට සැලසුම් කරන්න.
QStringList schemaStatements(const QString &prefix = QString())
{
    // PK එක row හඳුනාගනී; REFERENCES සම්බන්ධ tables වල තිබෙන අගයන් පමණක් භාවිත කිරීමට දෙයි.
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
        // එක booking එකක එකම slot දෙවරක් save නොවන composite primary key එකයි.
        "PRIMARY KEY(record_id, time_slot))"
    };
}

// SQL result එකේ columns StudentRecord fields වලට දමයි; value() අනුපිළිවෙළ recordColumns සමඟ ගළපන්න.
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
}


// SQLite connection එක සාදා foreign keys ක්‍රියාත්මක කර database file එක විවෘත කරයි.
SqliteDatabaseManager::SqliteDatabaseManager(const QString &dbName)
    : connectionName("workshop-" + QUuid::createUuid().toString(QUuid::WithoutBraces))
{
    db = QSqlDatabase::addDatabase("QSQLITE", connectionName);
    // Database එක වෙනත් වැඩකින් lock නම් මිලි තත්පර 5000ක් බලා සිටියි.
    db.setConnectOptions("QSQLITE_BUSY_TIMEOUT=5000");  
    db.setDatabaseName(dbName);
    if (!db.open()) {
        lastError = db.lastError().text();
    } else {
        QSqlQuery query(db);
        // Foreign key නීති SQLite හි මෙම connection එකට ක්‍රියාත්මක කරයි.
        if (!query.exec("PRAGMA foreign_keys = ON")) {
            lastError = query.lastError().text();
            db.close();
        }
    }
}

// Database connection එක වසා Qt connection ලැයිස්තුවෙන් ඉවත් කරයි.
SqliteDatabaseManager::~SqliteDatabaseManager()
{
    db.close();
    db = QSqlDatabase();
    QSqlDatabase::removeDatabase(connectionName);
}

// Database එක විවෘතදැයි බලයි; නැතිනම් lastError සකසයි.
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

// සම්බන්ධ save වැඩ එකම transaction එකකට අරඹයි; වෙනත් writes අවසන් වනතුරු SQLite බලා සිටියි.
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

// Transaction එක සම්පූර්ණයෙන් save කරයි; අසාර්ථක නම් rollback කරයි.
bool SqliteDatabaseManager::commitWrite()
{
    if (db.commit())
        return true;
    lastError = db.lastError().text();
    db.rollback();
    return false;
}

// Database වෙත යවන booking එකේ අනිවාර්ය දත්ත සහ email පරීක්ෂා කරයි.
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

    // Database වෙත save කරන email එකේ රටාවත් පරීක්ෂා කරයි.
    static const QRegularExpression emailPattern(
        "^[A-Za-z0-9._%+-]+@[A-Za-z0-9.-]+\\.[A-Za-z]{2,}$");
    if (!emailPattern.match(record.getEmail().trimmed()).hasMatch()) {
        lastError = "Enter a valid email address.";
        return false;
    }
    return true;
}


// නව tables සාදයි; පැරණි database එකක් නම් දත්ත රැකගෙන නව tables වලට මාරු කරයි.
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
    // Database schema version එකයි; නව columns එක් කළොත් migration සැලසුමත් යාවත්කාලීන කරන්න.
    const int version = query.value(0).toInt();
    query.finish();
    if (version > 4) {
        lastError = "This database was created by a newer version of SYNCLAB.";
        return false;
    }
    // දැනට භාවිත schema එක නම් tables/relationships පරීක්ෂා කර අලුතින් නැවත සාදන්නේ නැහැ.
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

    // පරණ database මාරු කිරීමට පෙර backup එකක් ගනී; කලින් backup overwrite නොකරයි.
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

    // Migration එකේ අසාර්ථක පියවරකදී වෙනස්කම් rollback කර දෝෂය තබයි.
    auto fail = [this](const QString &message) {
        lastError = message;
        db.rollback();
        return false;
    };

    // Migration සඳහා _4nf_ staging tables භාවිත කරයි; අලුත් database එකට සාමාන්‍ය නම් යොදයි.
    const QString prefix = migrating ? "_4nf_" : "";
    for (const QString &statement : schemaStatements(prefix)) {
        if (!query.exec(statement))
            return fail(query.lastError().text());
    }

    // Booking නැති පරණ students/workshops වුවත් අලුත් tables වෙත රැගෙන යයි.
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

    // Catalog එකේ workshops සහ ඒවායේ departments database එකට දමයි.
    for (const QString &category : workshopCategories()) {
        query.prepare("INSERT INTO " + prefix + "workshops(category, department) VALUES(?, ?) "
                      "ON CONFLICT(category) DO UPDATE SET department=excluded.department");
        query.addBindValue(category);
        query.addBindValue(workshopDepartment(category));
        if (!query.exec())
            return fail(query.lastError().text());
    }

    if (migrating) {
        // මකා දැමූ පරණ ID එකක් නැවත භාවිත නොවීමට AUTOINCREMENT අවසන් අගය මතක තබයි.
        qlonglong previousSequence = 0;
        if (query.exec("SELECT seq FROM sqlite_sequence WHERE name='records'") && query.next())
            previousSequence = query.value(0).toLongLong();
        query.finish();

        // පරණ column එක තිබේ නම් එහි value ගනී; නැතිනම් empty/NULL අගයක් දෙයි.
        auto oldValue = [&oldColumns](const QString &column) {
            // Optional notes වල NULL අගය හිස් text එකකට හරවන්නේ නැහැ.
            if (column == "description" || column == "admin_remarks")
                return oldColumns.contains(column) ? "r." + column : QString("NULL");
            return oldColumns.contains(column) ? "COALESCE(r." + column + ", '')" : QString("''");
        };
        const bool hasStudents = studentColumns.contains("index_no") && oldColumns.contains("index_no");
        // පරණ booking profile හි value හිස් නම් student table එකේ value ගනී.
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

        // පරණ normalized slot rows record ID අනුව සටහන් කරයි.
        QMap<int, QStringList> existingSlots;
        const QSet<QString> slotColumns = tableColumns(db, "booking_slots");
        if (slotColumns.contains("record_id") && slotColumns.contains("time_slot")) {
            if (!query.exec("SELECT record_id, time_slot FROM booking_slots ORDER BY record_id, time_slot"))
                return fail(query.lastError().text());
            while (query.next())
                existingSlots[query.value(0).toInt()].append(query.value(1).toString());
            query.finish();
        }

        // පරණ booking එක බැගින් අලුත් workshop/student/record/slot rows වලට මාරු කරයි.
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

            query.prepare("INSERT INTO _4nf_students(index_no, name, department, email) VALUES(?, ?, ?, ?) "
                          // එකම index එකේ profile එක update කරයි; migration හි වැඩිම ID එකේ profile අන්තිමට යෙදේ.
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

            // පරණ comma list එක තිබේ නම් එය ගනී; නැතිනම් පරණ slot rows ගනී.
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

        // Foreign keys රැකීමට child tables මුලින් සහ parent tables පසුව ඉවත් කරයි.
        for (const QString &table : {QString("booking_slots"), QString("records"),
                                     QString("students"), QString("workshops")}) {
            if (!query.exec("DROP TABLE IF EXISTS " + table))
                return fail(query.lastError().text());
        }
        // සාර්ථක staging tables සාමාන්‍ය නම් වලට මාරු කරයි; SQLite references ද ගළපයි.
        for (const QString &table : {QString("workshops"), QString("students"),
                                     QString("records"), QString("booking_slots")}) {
            if (!query.exec("ALTER TABLE _4nf_" + table + " RENAME TO " + table))
                return fail(query.lastError().text());
        }
        // පරණ ඉහළම ID සීමාව අලුත් records table එකටත් රැගෙන යයි.
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

    // Workshop/date අනුව සෙවීම වේගවත් කරන index එකයි.
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

// Workshop එක සහ student profile එක save කරයි; එකම index එකේ bookings එකම profile එක භාවිත කරයි.
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

// Booking එකේ පරණ slot rows මකා අලුත් list එකේ slot එකකට row එක බැගින් දමයි.
bool SqliteDatabaseManager::replaceSlots(int recordId, const QStringList &slotList)
{
    QSqlQuery query(db);
    // මේ booking එකට පමණක් අදාළ පරණ slots ඉවත් කරයි.
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


// නව booking එක insert කරයි; key එක සහ slots වෙනත් booking එකක භාවිතදැයිත් බලයි.
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

    // නව allocation key එක වෙනත් booking එකක තිබේ නම් save කිරීම නවතයි.
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

    // Reject booking එකක් slots අල්ලන්නේ නැහැ; අනෙක් statuses සඳහා availability බලයි.
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

    // Booking එකට අදාළ fields පමණක් records table එකේ save කරයි.
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
    // SQLite දුන් අලුත් ID එක slot rows සමඟ සම්බන්ධ කිරීමට ගනී.
    const int recordId = query.lastInsertId().toInt();
    query.finish();
    if (!replaceSlots(recordId, splitSlots(record.getTimeSlots()))) {
        db.rollback();
        return false;
    }
    return commitWrite();
}

// පවතින booking එක update කරයි. Key, created_at සහ admin_remarks මෙහි වෙනස් නොකරයි.
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

    // Edit කිරීමට පෙර booking ID එක තවම තිබේදැයි බලයි.
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

    // Booking details update කරයි; ඉන්පසු slot rows එම transaction එකේම මාරු කරයි.
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

// ID එකෙන් booking එක මකා දමයි; එහි slot rows foreign key cascade මගින් මැකේ.
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
    // එක් booking row එකක්වත් වෙනස් නොවුණොත් booking එක නැති බව දක්වයි.
    if (query.numRowsAffected() != 1) {
        lastError = "The allocation no longer exists. Refresh the allocation list.";
        return false;
    }
    return true;
}

// Status එක පමණක් වෙනස් කරයි; Reject නොවන status එකකට යද්දී slots නැවත පරීක්ෂා කරයි.
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

    // Status මාරු කිරීමට පෙර එම booking එකේ date/category/slots කියවා පරීක්ෂා කරයි.
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

// Booking එකේ admin remark එක පමණක් save කරයි.
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


// Workshop එකක තෝරාගත් දිනයේ bookings අලුත්ම ID එක මුලින් ලබාදෙයි.
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

// Reject නොකළ bookings ඇති දින ලබාදෙයි.
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

// ඉල්ලූ slots සියල්ල හිස් නම් true දෙයි; edit කරන booking එක excludeRecordId වලින් හැර දමයි.
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

// එම workshop/date එකේ වෙන්කර ඇති slots ලබාදෙයි; Reject bookings ඉවත් කරයි.
QStringList SqliteDatabaseManager::getAllocatedTimeSlotsStringList(const QString &date,
                                                                const QString &category,
                                                                int excludeRecordId)
{
    QStringList slotList;
    if (!ensureOpen())
        return slotList;

    QSqlQuery query(db);
    // Reject නැති bookings හි slots පමණක් ගනී; edit කරන ID එක query එකෙන් හැර දමයි.
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

// Workshop එකේ සියලු bookings අලුත් දිනය මුලින් ලැබෙන ලෙස ගනී.
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

// අවසන් database වැඩේ අසාර්ථක වූ හේතුව ලබාදෙයි.
QString SqliteDatabaseManager::getLastError() const
{
    return lastError;
}
