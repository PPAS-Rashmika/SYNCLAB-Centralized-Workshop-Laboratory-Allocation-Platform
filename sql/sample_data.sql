-- Fictional demonstration data only. Run once after schema.sql in an empty
-- demonstration database. IDs 1-7 are explicit so slots reference booking IDs,
-- rather than assuming that allocation keys are unique in every legacy database.

PRAGMA foreign_keys = ON;
BEGIN IMMEDIATE;

INSERT INTO students (index_no, name, department, email) VALUES
('DEMO-001', 'Demo Student One', 'Department of Nano Science Technology', 'student.one@example.org'),
('DEMO-002', 'Demo Student Two', 'Department of Nano Science Technology', 'student.two@example.org'),
('DEMO-003', 'Demo Student Three', 'Department of Construction Technology', 'student.three@example.org'),
('DEMO-004', 'Demo Student Four', 'Department of Construction Technology', 'student.four@example.org'),
('DEMO-005', 'Demo Student Five', 'Department of Construction Technology', 'student.five@example.org'),
('DEMO-006', 'Demo Student Six', 'Department of Electrotechnology', 'student.six@example.org'),
('DEMO-007', 'Demo Student Seven', 'Department of Electrotechnology', 'student.seven@example.org');

INSERT INTO records (
    id, category, index_no, supervisor_name, project_name, record_date,
    description, passcode, created_at, status, admin_remarks
) VALUES
(1, 'Biochemistry', 'DEMO-001', 'Demo Supervisor One', 'Demo Project One',
 '2030-01-10', 'Demonstration: basic laboratory observation.', '1001',
 '2030-01-05T09:00:00', 'Pending', ''),
(2, 'Computer', 'DEMO-002', 'Demo Supervisor Two', 'Demo Project Two',
 '2030-01-10', 'Demonstration: programming practical.', '1002',
 '2030-01-05T09:15:00', 'Approve', 'Demonstration approval.'),
(3, 'Environment and Water Resources Laboratory', 'DEMO-003', 'Demo Supervisor Three', 'Demo Project Three',
 '2030-01-11', 'Demonstration: water sample observation.', '1003',
 '2030-01-05T09:30:00', 'Pending', ''),
(4, 'Highway Laboratory', 'DEMO-004', 'Demo Supervisor Four', 'Demo Project Four',
 '2030-01-11', 'Demonstration: highway materials practical.', '1004',
 '2030-01-05T09:45:00', 'Reject', 'Demonstration rejection; the slot is available.'),
(5, 'Structures Laboratory', 'DEMO-005', 'Demo Supervisor Five', 'Demo Project Five',
 '2030-01-12', 'Demonstration: structures practical with two time slots.', '1005',
 '2030-01-05T10:00:00', 'Approve', 'Demonstration approval.'),
(6, 'Electrical Machines & Communications Laboratory', 'DEMO-006', 'Demo Supervisor Six', 'Demo Project Six',
 '2030-01-12', 'Demonstration: electrical machines practical.', '1006',
 '2030-01-05T10:15:00', 'Pending', ''),
(7, 'Measurement Laboratory', 'DEMO-007', 'Demo Supervisor Seven', 'Demo Project Seven',
 '2030-01-13', 'Demonstration: measurement instruments practical.', '1007',
 '2030-01-05T10:30:00', 'Approve', 'Demonstration approval.');

INSERT INTO booking_slots (record_id, time_slot) VALUES
(1, '8.30 am - 10.30 am'),
(2, '10.30 am - 12.30 pm'),
(3, '8.30 am - 10.30 am'),
(4, '10.30 am - 12.30 pm'),
(5, '8.30 am - 10.30 am'),
(5, '10.30 am - 12.30 pm'),
(6, '1.30 pm - 3.30 pm'),
(7, '3.30 pm - 5.30 pm');

COMMIT;

-- DEMO-005 is a Construction Technology student using a workshop belonging to
-- Mechanical and Manufacturing Technology. These are two different facts.
-- Booking 5 contains two slot rows. Booking 4 retains its slot for history,
-- but Reject status means it does not reserve that slot.
