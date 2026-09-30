-- Example SQLite reports. These SELECT statements do not change saved data.
-- Example dates and index numbers match sample_data.sql; change them as needed.

-- 1. All allocations, newest allocation date first.
SELECT passcode, name, index_no, category, record_date, time_slot, status
FROM records
ORDER BY record_date DESC, id DESC;

-- 2. Active allocations for one day; rejected allocations do not reserve time slots.
SELECT category, passcode, name, time_slot, status
FROM records
WHERE record_date = '2030-01-12'
  AND lower(trim(COALESCE(status, 'Pending'))) NOT IN ('reject', 'rejected')
ORDER BY category, id;

-- 3. Allocations waiting for an administrator's decision.
SELECT passcode, category, name, supervisor_name, project_name, record_date, time_slot, description
FROM records
WHERE lower(trim(COALESCE(status, 'Pending'))) = 'pending'
ORDER BY record_date, id;

-- 4. Number of allocations and each decision status, grouped by laboratory.
-- An allocation can contain several time slots; these are allocation counts, not hours.
SELECT category,
       COUNT(*) AS total_allocations,
       SUM(CASE WHEN lower(trim(COALESCE(status, 'Pending'))) = 'pending'
                THEN 1 ELSE 0 END) AS pending_allocations,
       SUM(CASE WHEN lower(trim(status)) IN ('approve', 'approved')
                THEN 1 ELSE 0 END) AS approved_allocations,
       SUM(CASE WHEN lower(trim(status)) IN ('reject', 'rejected')
                THEN 1 ELSE 0 END) AS rejected_allocations
FROM records
GROUP BY category
ORDER BY total_allocations DESC, category;

-- 5. One student's allocation history, including the administrator's remarks.
SELECT passcode, category, record_date, time_slot, status, admin_remarks
FROM records
WHERE index_no = 'DEMO-003'
ORDER BY record_date DESC, id DESC;
