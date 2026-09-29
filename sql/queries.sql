-- Example SQLite reports. These SELECT statements do not change saved data.
-- Example dates and index numbers match sample_data.sql; change them as needed.

-- 1. All bookings, newest booking date first.
SELECT passcode, name, index_no, category, record_date, time_slot, status
FROM records
ORDER BY record_date DESC, id DESC;

-- 2. Active bookings for one day; rejected bookings do not reserve time slots.
SELECT category, passcode, name, time_slot, status
FROM records
WHERE record_date = '2030-01-12'
  AND lower(trim(COALESCE(status, 'Pending'))) NOT IN ('reject', 'rejected')
ORDER BY category, id;

-- 3. Bookings waiting for an administrator's decision.
SELECT passcode, category, name, record_date, time_slot, description
FROM records
WHERE lower(trim(COALESCE(status, 'Pending'))) = 'pending'
ORDER BY record_date, id;

-- 4. Number of bookings and each decision status, grouped by laboratory.
-- A booking can contain several time slots; these are booking counts, not hours.
SELECT category,
       COUNT(*) AS total_bookings,
       SUM(CASE WHEN lower(trim(COALESCE(status, 'Pending'))) = 'pending'
                THEN 1 ELSE 0 END) AS pending_bookings,
       SUM(CASE WHEN lower(trim(status)) IN ('approve', 'approved')
                THEN 1 ELSE 0 END) AS approved_bookings,
       SUM(CASE WHEN lower(trim(status)) IN ('reject', 'rejected')
                THEN 1 ELSE 0 END) AS rejected_bookings
FROM records
GROUP BY category
ORDER BY total_bookings DESC, category;

-- 5. One student's booking history, including the administrator's remarks.
SELECT passcode, category, record_date, time_slot, status, admin_remarks
FROM records
WHERE index_no = 'DEMO-003'
ORDER BY record_date DESC, id DESC;
