-- SYNCLAB: four-table 4NF schema for a NEW demonstration database.
-- Run this file first, then sample_data_small.sql. The app migrates saved data.
PRAGMA foreign_keys = ON;
BEGIN IMMEDIATE;

CREATE TABLE workshops (
    category TEXT NOT NULL PRIMARY KEY,
    department TEXT NOT NULL
);

CREATE TABLE students (
    index_no TEXT NOT NULL PRIMARY KEY,
    name TEXT NOT NULL,
    department TEXT NOT NULL,
    email TEXT NOT NULL
);

CREATE TABLE records (
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

CREATE TABLE booking_slots (
    record_id INTEGER NOT NULL REFERENCES records(id) ON DELETE CASCADE,
    time_slot TEXT NOT NULL
        CHECK (length(trim(time_slot)) > 0 AND instr(time_slot, ',') = 0),
    PRIMARY KEY (record_id, time_slot)
);

CREATE INDEX records_category_date ON records(category, record_date);
PRAGMA user_version = 4;
COMMIT;
