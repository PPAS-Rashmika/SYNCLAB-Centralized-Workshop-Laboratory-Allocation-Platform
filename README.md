# SYNCLAB — Qt 6 workshop bookings

A native C++17 / Qt 6 Widgets application using SQLite through Qt SQL.
No Python, JavaScript, QML, web service, or additional application framework is required.

## Open in Qt Creator

1. Open **StudentWorkshopApp.pro**.
2. Choose a Qt 6 desktop kit with Widgets, SQL, PrintSupport, and the SQLite driver.
3. Build and run. This project was verified with Qt 6.11.2 and MinGW 13.1, 64-bit.

The existing Qt Designer .ui files remain editable in Qt Creator.

Verification: Qt 6.11.2 release build succeeded; database tests and UI tests each
reported 14 passed, 0 failed (including setup/cleanup). UI previews were checked
at 1100×760 and 900×650, including dark mode.

## Booking and administration

- Choose a workshop, a date, and one or more time slots, then enter student details.
- Save the allocation key shown after booking. The confirmation text can be selected and copied.
- Use **Edit Booking** with that key. Changes submitted by a student return to **Pending**.
- Rejected bookings release their time slots. Reactivation and workshop transfers check availability again when saving.
- The administrator dashboard provides date filtering, editable remarks/status, PDF opening on double-click, and printing.
- Email buttons open a draft in the configured email application; the application does not send email automatically.
- Settings includes the actual workshop list, light/dark mode, wallpaper, and configurable administrator/master passcodes.

Existing defaults remain compatible: master passcode **0000**, workshop administrator passcode **0000000**.
Use **Settings → Administrator passcodes** to change these; choose **System administrator** for the master passcode.

## Data locations

An existing student_data.db in the run working directory or beside the executable is reused.
For a new installation, the database is created in Qt's writable local application data folder
(StudentWorkshopApp/SYNCLAB on Windows). PDFs are copied into uploads/pdfs beside the selected database.
Replacing an attachment never overwrites its previous file before the booking saves.
Previously stored attachment files are retained when a booking is changed or deleted.

This workspace's existing database is at
build/Desktop_Qt_6_11_2_MinGW_64_bit_Debug/debug/student_data.db.
To keep using that database from a different build, set Qt Creator's run working directory to that folder.
The existing database and its contents were not used by the automated tests.

## Regression tests

Open either test project in Qt Creator with the same Qt 6 kit:

- tests/database_tests.pro: SQLite migration, validation, availability, simultaneous booking attempts,
  rejected-slot release, status changes, transaction rollback, and workshop transfers.
- tests/ui_tests.pro: new/edit/back navigation, draft preservation, validation, attachments,
  ambiguous old allocation keys, administration, themes, historical bookings, and compact layouts.

Tests use temporary databases and temporary INI settings. The Qt Test module is required only for tests.
For an offscreen test run on Windows, set QT_QPA_PLATFORM=offscreen and
QT_QPA_FONTDIR=C:\Windows\Fonts. Set UI_SCREENSHOT_DIR to an output folder to capture UI previews.

## What was corrected

- Responsive workshop cards, scrollable forms, clear full time ranges, readable calendars, and consistent themes.
- Student edit navigation, backward navigation, preserved form drafts, and historical booking access.
- Allocation-key ambiguity, missing validation, silent database failures, and unsafe PDF replacement.
- Atomic booking writes, accurate rejected-booking availability, editable workshop/category, and status rollback.
- Administrator filter/selection state, matching settings categories, and functional print/PDF actions.
