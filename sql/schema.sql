-- SQLite 4NF schema used by SqliteDatabaseManager.
-- Run in a NEW demonstration database before sample_data.sql.
-- The application creates this schema and migrates old databases automatically.
-- IF NOT EXISTS does not migrate an old records table; use the application for that.

PRAGMA foreign_keys = ON;
BEGIN IMMEDIATE;

CREATE TABLE IF NOT EXISTS workshops (
    category TEXT NOT NULL PRIMARY KEY,
    department TEXT NOT NULL
);

CREATE TABLE IF NOT EXISTS students (
    index_no TEXT NOT NULL PRIMARY KEY,
    name TEXT NOT NULL,
    department TEXT NOT NULL,
    email TEXT NOT NULL
);

CREATE TABLE IF NOT EXISTS records (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    category TEXT NOT NULL REFERENCES workshops(category),
    index_no TEXT NOT NULL REFERENCES students(index_no),
    supervisor_name TEXT NOT NULL,
    project_name TEXT NOT NULL,
    record_date TEXT NOT NULL,
    description TEXT,
    passcode TEXT NOT NULL,
    created_at TEXT NOT NULL,
    status TEXT NOT NULL DEFAULT 'Pending'
        CHECK (status IN ('Pending', 'Approve', 'Reject')),
    admin_remarks TEXT
);

CREATE TABLE IF NOT EXISTS booking_slots (
    record_id INTEGER NOT NULL REFERENCES records(id) ON DELETE CASCADE,
    time_slot TEXT NOT NULL
        CHECK (length(trim(time_slot)) > 0 AND instr(time_slot, ',') = 0),
    PRIMARY KEY (record_id, time_slot)
);

CREATE INDEX IF NOT EXISTS records_category_date
    ON records(category, record_date);

-- The catalog matches utilities/workshopcatalog.cpp.
-- A workshop's department is distinct from the student's department.
INSERT INTO workshops (category, department) VALUES
('Biochemistry', 'Department of Nano Science Technology'),
('Electronics', 'Department of Nano Science Technology'),
('Computer', 'Department of Nano Science Technology'),
('Advanced Material', 'Department of Nano Science Technology'),
('Geotechnical', 'Department of Construction Technology'),
('Surveying', 'Department of Construction Technology'),
('Concrete', 'Department of Construction Technology'),
('Environment and Water Resources Laboratory', 'Department of Construction Technology'),
('Highway Laboratory', 'Department of Construction Technology'),
('Physics', 'Department of Mechanical and Manufacturing Technology'),
('Drawing Office', 'Department of Mechanical and Manufacturing Technology'),
('Manufacturing', 'Department of Mechanical and Manufacturing Technology'),
('Structures Laboratory', 'Department of Mechanical and Manufacturing Technology'),
('Electrical Machines & Communications Laboratory', 'Department of Electrotechnology'),
('Measurement Laboratory', 'Department of Electrotechnology')
ON CONFLICT(category) DO UPDATE SET department = excluded.department;

PRAGMA user_version = 4;
COMMIT;

-- Dates use ISO text: YYYY-MM-DD. Each selected slot is a booking_slots row.
-- Allocation-key uniqueness, required form fields and active-slot overlap checks
-- are enforced by the application inside its write transaction.
-- passcode is deliberately not UNIQUE: migration preserves old duplicate keys.
