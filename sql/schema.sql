-- SQLite schema used by SqliteDatabaseManager::createTable().
-- Run this script in a new demonstration database, before sample_data.sql.
-- The application also creates this table automatically on first launch.
-- Existing installations are upgraded by createTable() in C++.

BEGIN IMMEDIATE;

CREATE TABLE IF NOT EXISTS records (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    workshop_id TEXT,
    category TEXT,
    name TEXT,
    index_no TEXT,
    department TEXT,
    email TEXT,
    supervisor_name TEXT,
    project_name TEXT,
    record_date TEXT,
    time_slot TEXT,
    description TEXT,
    passcode TEXT,
    created_at TEXT,
    status TEXT DEFAULT 'Pending',
    admin_remarks TEXT
);

CREATE INDEX IF NOT EXISTS records_category_date
    ON records(category, record_date);

COMMIT;

-- Dates use ISO text: YYYY-MM-DD. Multiple time slots use comma-separated text.
-- The existing application validates required fields and allocation keys in C++.
-- Its transaction checks prevent overlapping active bookings.
-- The schema is retained to preserve the application's current behavior.
