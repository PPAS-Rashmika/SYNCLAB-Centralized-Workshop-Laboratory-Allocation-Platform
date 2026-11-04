# SYNCLAB

SYNCLAB is a Qt Widgets desktop application for managing student workshop and laboratory allocations. It uses a local SQLite database to store student details, bookings, selected time slots, and allocation decisions.

## Features

- Browse ten workshops and laboratories.
- Submit and update allocation requests.
- Check date and time-slot availability before saving a request.
- Review requests and set their status to Pending, Approve, or Reject.
- Add administrative remarks to a request.
- Choose the light or dark theme and configure the application appearance.

## Requirements

- Qt 6 or newer, including the **Core**, **Gui**, **Widgets**, and **Sql** modules.
- A C++17-capable compiler supported by your Qt Desktop kit.
- The Qt SQLite SQL driver (`QSQLITE`).

The project uses qmake and is configured by `StudentWorkshopApp.pro`.

## Build and run with Qt Creator

1. Open `StudentWorkshopApp.pro` in Qt Creator.
2. Select a Qt 6 Desktop kit for your compiler.
3. Configure the project, then build and run it from Qt Creator.

For a MinGW command-line build on Windows, open a Qt-enabled terminal in the project folder and run:

```powershell
New-Item -ItemType Directory -Force build
Set-Location build
qmake ..\StudentWorkshopApp.pro
mingw32-make
```

Use the matching build command for your selected compiler (for example, `nmake` with an MSVC kit). The executable is produced in the build output directory selected by the kit.

## Portable Windows build

A ready-to-run 64-bit Windows package is in `dist/SYNCLAB`. Start `StudentWorkshopApp.exe` and keep all files and folders in the package together. The package has its own `data/student_data.db`; a fresh database is initialized automatically on first launch.

## Data and settings

The application uses `data/student_data.db` as its SQLite database when that project data folder is available. Keep this file if you need to preserve its records, and make a backup before replacing or editing it. If the project data folder cannot be used, the application falls back to a writable user-data location.

In the portable Windows package, the application uses `data/student_data.db` beside the executable. Keep the executable, its Qt runtime files, and the `data` folder together when copying the package.

Application preferences are stored through Qt's user-scoped settings, separately from the source files.

## Folder layout

```text
.
├── StudentWorkshopApp.pro       # qmake project and source-file lists
├── main.cpp                     # application entry point
├── data/                        # SQLite application database
├── database/                    # SQLite implementation and database interface
├── logic/                       # allocation validation and business rules
├── models/                      # student/allocation data model
├── resources/                   # Qt resource file, logo, wallpapers, workshop images
├── ui/
│   ├── dialogs/                 # dialogs and dialog themes
│   ├── forms/                   # Qt Designer .ui files
│   └── widgets/                 # custom UI widgets
└── utilities/                   # workshop catalog and file-storage helpers
```

## Adding source files

The qmake project lists source files explicitly. When adding a file, update the corresponding `SOURCES`, `HEADERS`, or `FORMS` section in `StudentWorkshopApp.pro`. Add new images or other bundled assets to `resources/images.qrc` as well.
