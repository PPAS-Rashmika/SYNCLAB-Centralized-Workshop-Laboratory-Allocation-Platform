-- Fictional demonstration records only; no real student data is included.
-- Run once after schema.sql in an empty demonstration database.
-- SQLite assigns the numeric id; each example has a distinct 4-digit allocation key.
-- Demo allocation keys below are examples for these fictional records only.

BEGIN IMMEDIATE;

INSERT INTO records (
    workshop_id, category, name, index_no, department,
    email, record_date, time_slot, description, passcode,
    created_at, status, admin_remarks
) VALUES
(
    'Biochemistry', 'Biochemistry',
    'Demo Student One', 'DEMO-001', 'Department of Nano Science Technology',
    'student.one@example.org', '2030-01-10', '8.30 am - 10.30 am',
    'Demonstration: basic laboratory observation.', '1001',
    '2030-01-05T09:00:00', 'Pending', ''
),
(
    'Computer', 'Computer',
    'Demo Student Two', 'DEMO-002', 'Department of Nano Science Technology',
    'student.two@example.org', '2030-01-10', '10.30 am - 12.30 pm',
    'Demonstration: programming practical.', '1002',
    '2030-01-05T09:15:00', 'Approve', 'Demonstration approval.'
),
(
    'Environment and Water Resources Laboratory',
    'Environment and Water Resources Laboratory',
    'Demo Student Three', 'DEMO-003', 'Department of Construction Technology',
    'student.three@example.org', '2030-01-11', '8.30 am - 10.30 am',
    'Demonstration: water sample observation.', '1003',
    '2030-01-05T09:30:00', 'Pending', ''
),
(
    'Highway Laboratory', 'Highway Laboratory',
    'Demo Student Four', 'DEMO-004', 'Department of Construction Technology',
    'student.four@example.org', '2030-01-11', '10.30 am - 12.30 pm',
    'Demonstration: highway materials practical.', '1004',
    '2030-01-05T09:45:00', 'Reject', 'Demonstration rejection; the slot is available.'
),
(
    'Structures Laboratory', 'Structures Laboratory',
    'Demo Student Five', 'DEMO-005', 'Department of Construction Technology',
    'student.five@example.org', '2030-01-12',
    '8.30 am - 10.30 am, 10.30 am - 12.30 pm',
    'Demonstration: structures practical with two time slots.', '1005',
    '2030-01-05T10:00:00', 'Approve', 'Demonstration approval.'
),
(
    'Electrical Machines & Communications Laboratory',
    'Electrical Machines & Communications Laboratory',
    'Demo Student Six', 'DEMO-006', 'Department of Electrotechnology',
    'student.six@example.org', '2030-01-12', '1.30 pm - 3.30 pm',
    'Demonstration: electrical machines practical.', '1006',
    '2030-01-05T10:15:00', 'Pending', ''
),
(
    'Measurement Laboratory', 'Measurement Laboratory',
    'Demo Student Seven', 'DEMO-007', 'Department of Electrotechnology',
    'student.seven@example.org', '2030-01-13', '3.30 pm - 5.30 pm',
    'Demonstration: measurement instruments practical.', '1007',
    '2030-01-05T10:30:00', 'Approve', 'Demonstration approval.'
);

COMMIT;
