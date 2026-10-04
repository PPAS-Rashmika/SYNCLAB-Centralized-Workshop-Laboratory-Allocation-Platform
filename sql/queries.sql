-- Read-only SQLite reports for the normalized schema.
-- Example dates and index numbers match sample_data.sql.
-- GROUP_CONCAT builds display text; comma lists are never stored in records.

-- 1. All allocations, newest allocation date first, with both departments.
SELECT r.id, r.passcode, s.name, r.index_no, r.category,
       s.department AS student_department, w.department AS workshop_department,
       r.record_date,
       (SELECT GROUP_CONCAT(time_slot, ', ') FROM
          (SELECT time_slot FROM booking_slots
           WHERE record_id = r.id
           ORDER BY CASE time_slot
             WHEN '8.30 am - 10.30 am' THEN 0
             WHEN '10.30 am - 12.30 pm' THEN 1
             WHEN '1.30 pm - 3.30 pm' THEN 2
             WHEN '3.30 pm - 5.30 pm' THEN 3
             ELSE 4 END, time_slot)) AS time_slots,
       r.status
FROM records r
JOIN students s ON s.index_no = r.index_no
JOIN workshops w ON w.category = r.category
ORDER BY r.record_date DESC, r.id DESC;

-- 2. Active allocations for one day, one row per reserved slot.
-- Rejected allocations do not reserve slots.
SELECT r.category, r.passcode, s.name, bs.time_slot, r.status
FROM records r
JOIN students s ON s.index_no = r.index_no
JOIN booking_slots bs ON bs.record_id = r.id
WHERE r.record_date = '2030-01-12' AND r.status <> 'Reject'
ORDER BY r.category, r.id, CASE bs.time_slot
    WHEN '8.30 am - 10.30 am' THEN 0
    WHEN '10.30 am - 12.30 pm' THEN 1
    WHEN '1.30 pm - 3.30 pm' THEN 2
    WHEN '3.30 pm - 5.30 pm' THEN 3
    ELSE 4 END, bs.time_slot;

-- 3. Allocations waiting for an administrator's decision.
SELECT r.passcode, r.category, s.name, r.supervisor_name, r.project_name,
       r.record_date,
       (SELECT GROUP_CONCAT(time_slot, ', ') FROM
          (SELECT time_slot FROM booking_slots
           WHERE record_id = r.id
           ORDER BY CASE time_slot
             WHEN '8.30 am - 10.30 am' THEN 0
             WHEN '10.30 am - 12.30 pm' THEN 1
             WHEN '1.30 pm - 3.30 pm' THEN 2
             WHEN '3.30 pm - 5.30 pm' THEN 3
             ELSE 4 END, time_slot)) AS time_slots,
       r.description
FROM records r
JOIN students s ON s.index_no = r.index_no
WHERE r.status = 'Pending'
ORDER BY r.record_date, r.id;

-- 4. Allocation counts by laboratory, including laboratories with no bookings.
-- Do not join booking_slots: a booking with two slots is still one allocation.
SELECT w.category, COUNT(r.id) AS total_allocations,
       SUM(CASE WHEN r.status = 'Pending' THEN 1 ELSE 0 END) AS pending_allocations,
       SUM(CASE WHEN r.status = 'Approve' THEN 1 ELSE 0 END) AS approved_allocations,
       SUM(CASE WHEN r.status = 'Reject' THEN 1 ELSE 0 END) AS rejected_allocations
FROM workshops w
LEFT JOIN records r ON r.category = w.category
GROUP BY w.category
ORDER BY total_allocations DESC, w.category;

-- 5. One student's allocation history with current shared student details.
SELECT r.passcode, s.name, s.email, r.category, r.record_date,
       (SELECT GROUP_CONCAT(time_slot, ', ') FROM
          (SELECT time_slot FROM booking_slots
           WHERE record_id = r.id
           ORDER BY CASE time_slot
             WHEN '8.30 am - 10.30 am' THEN 0
             WHEN '10.30 am - 12.30 pm' THEN 1
             WHEN '1.30 pm - 3.30 pm' THEN 2
             WHEN '3.30 pm - 5.30 pm' THEN 3
             ELSE 4 END, time_slot)) AS time_slots,
       r.status, r.admin_remarks
FROM records r
JOIN students s ON s.index_no = r.index_no
WHERE r.index_no = 'DEMO-003'
ORDER BY r.record_date DESC, r.id DESC;
