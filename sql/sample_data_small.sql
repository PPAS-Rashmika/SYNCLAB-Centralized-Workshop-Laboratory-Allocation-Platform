-- Fictional sample data. Run once after create_tables.sql in an empty demo DB.
-- Student DEMO-001 has two bookings; booking 1 has two separate slot rows.
PRAGMA foreign_keys = ON;
BEGIN IMMEDIATE;

INSERT INTO workshops (category, department) VALUES
('Computer', 'Department of Nano Science Technology'),
('Structures Laboratory', 'Department of Mechanical and Manufacturing Technology');

INSERT INTO students (index_no, name, department, email) VALUES
('DEMO-001', 'Demo Student One', 'Department of Construction Technology',
 'student.one@example.org'),
('DEMO-002', 'Demo Student Two', 'Department of Nano Science Technology',
 'student.two@example.org');

INSERT INTO records (
    id, category, index_no, supervisor_name, project_name, record_date,
    description, passcode, created_at, status, admin_remarks
) VALUES
(1, 'Structures Laboratory', 'DEMO-001', 'Demo Supervisor One', 'Beam Testing',
 '2030-01-12', 'Structures practical with two time slots.', '2001',
 '2030-01-05T10:00:00', 'Approve', 'Approved for the practical.'),
(2, 'Computer', 'DEMO-002', 'Demo Supervisor Two', 'Programming Practical',
 '2030-01-12', 'Programming laboratory session.', '2002',
 '2030-01-05T10:15:00', 'Pending', ''),
(3, 'Computer', 'DEMO-001', 'Demo Supervisor One', 'Data Analysis',
 '2030-01-13', 'Second booking by the same student.', '2003',
 '2030-01-05T10:30:00', 'Reject', 'Reschedule this session.');

INSERT INTO booking_slots (record_id, time_slot) VALUES
(1, '8.30 am - 10.30 am'),
(1, '10.30 am - 12.30 pm'),
(2, '1.30 pm - 3.30 pm'),
(3, '8.30 am - 10.30 am');

COMMIT;
