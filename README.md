# SYNCLAB — Qt 6 workshop bookings

A native C++17 / Qt 6 Widgets application using SQLite through Qt SQL.

## Open in Qt Creator

1. Open **StudentWorkshopApp.pro**.
2. Choose a Qt 6 desktop kit with Widgets, SQL, PrintSupport and the SQLite driver.
3. Build and run.

## Edit the interface in Qt Designer

The C++ controller creates the main window and central page stack directly.
The nine `.ui` files contain only the five pages and four dialogs.

Open `welcome.ui`, `selection.ui`, `calander.ui`, `form.ui`, or `admin.ui`
in `ui/forms`, or one of the four dialog forms, in Qt Creator's **Design** view.
Their controls use explicit geometry without
layouts, so you can drag, resize and restyle them directly. The 10 workshop
cards, their pictures and labels are stored in `selection.ui` as two rows of five,
covering all four departments without scrolling.

The selection page starts at **1180 × 650**, and the other pages at
**1180 × 780**. The application opens maximized
and expands page controls proportionally to fill larger windows. Smaller windows
scroll the original canvas to keep controls readable.
Designer fonts, static text and custom styles are retained at runtime. Dark
mode translates the supplied palette colors and switching back restores the
original styles. Booking values, calendar counts and table rows remain dynamic.
Keep existing control names and workshop `category` properties when editing.
Each page's form class matches its root name (`welcomeWidget`, `selectionWidget`,
`calanderWidget`, `formWidget`, or `adminWidget`), so
saving a moved control in Designer keeps the generated class compatible with C++.



## Booking and administration

- Choose a workshop, a date and one or more time slots, then enter student details.
- Save the allocation key shown after booking. The confirmation text can be selected and copied.
- Use **Edit allocation** with that key. Changes submitted by a student return to **Pending**.
- Rejected bookings release their time slots. Reactivation and workshop transfers check availability again when saving.
- The administrator dashboard provides date filtering, editable remarks/status and printing.
- Email buttons open a draft in the configured email application.
- Settings includes the workshop list, light/dark mode, wallpaper and configurable administrator/master passcodes.

Default master passcode: **0000**. Default workshop administrator passcode: **0000000**.
Use **Settings → Administrator passcodes** to change these; choose **System administrator** for the master passcode.

## Database and existing data

The database is normalized to **4NF**, with four tables:

| Table | Stores |
|---|---|
| `workshops` | The 10 available workshop categories and their owning departments |
| `students` | One current profile per student index number |
| `records` | Booking details and references to a student and workshop |
| `booking_slots` | One row per selected slot, keyed by booking ID and slot |

Student department and workshop department are separate facts. Student profiles
are shared by index number, so changing a student's details updates what is
displayed on all their bookings. Slot lists are assembled for the UI; the
database stores individual slot rows.
for the schema, dependency reasoning and migration policy.

The normal database path is **data/student_data.db** beside the source files.
Existing databases in the run working directory, beside the executable or in
Qt's local application data folder are reused when the source-folder database
does not exist. A new installation falls back to local application data if the
source data folder cannot be created.

On opening an older or partially normalized database, the program creates a
backup named **student_data.db.pre-4nf.bak** beside that database, with a suffix
if necessary, then migrates within a transaction. Booking IDs and keys are
retained. If old profiles disagree, the highest booking ID for that index
number supplies the current student details, with existing student-table
values filling missing fields. A missing student department stays empty until
edited. Existing duplicate allocation keys are preserved; new bookings check
key uniqueness in the application.

## Database regression tests

Open **tests/database_tests.pro** in Qt Creator with the same Qt 6 desktop kit,
then build and run it. The Qt Test module is required only for tests. Tests use
temporary databases, keeping saved bookings separate from test fixtures.

## Designer interface regression tests

Open **tests/designer_ui_tests.pro** in Qt Creator with the same Qt 6 desktop
kit, then build and run. The Qt Test and XML modules are required. These tests
check authored geometry, fonts, text and styles across theme changes, all 10
workshop cards, scrolling, dialog actions and allocation saving. They use
temporary databases and settings.

## Reference SQL

The assignment report is **DATABASE_ASSIGNMENT.md** and
**output/pdf/SYNCLAB_Database_Assignment.pdf**. It includes an ER diagram,
entities and attributes, relationship cardinalities, primary/foreign keys,
a short normalization note, and executable SQL examples. Diagram exports are
in **output/diagrams/** (PNG, SVG and editable Mermaid).

For a small demonstration, run **sql/create_tables.sql**, followed by
**sql/sample_data_small.sql**, in a new empty database. This example contains
two students, three bookings and four separate slot rows.

Use **sql/schema.sql**, then **sql/sample_data.sql**, in a separate empty
demonstration database. **sql/queries.sql** contains five example reports.
The application creates its own tables and handles migration automatically.
