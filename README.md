# SYNCLAB - සිංහල මාර්ගෝපදේශය

මෙය workshops සහ laboratories සඳහා දිනයක් හා time slots වෙන්කරගන්නා
C++17 / Qt 6 program එකකි. Student ට booking එකක් save/edit/delete කළ හැක.
Administrator ට bookings, status, remarks සහ email drafts පාලනය කළ හැක.

Code එකේ function හෝ වැදගත් කොටසට ඉහළින් කෙටි සිංහල comment එකක් ඇත.
එම comment එකෙන් එහි වැඩේ සහ වෙනස් කළ යුතු අගයන් හඳුනාගත හැක.

## Qt Creator එකෙන් run කරන ආකාරය

1. Qt Creator තුළ `StudentWorkshopApp.pro` විවෘත කරන්න.
2. Qt 6 Desktop kit එකක් තෝරන්න. Widgets, SQL සහ SQLite driver තිබිය යුතුයි.
3. Project එක build කර Run කරන්න. Program එක maximized window එකක විවෘත වේ.

Qt modules ලෙස `core`, `gui`, `widgets`, `sql` භාවිත වේ.
Tables program එකෙන්ම සාදන නිසා වෙනම SQL script එකක් run කිරීම අවශ්‍ය නැහැ.

සිංහල comments සහිත files **UTF-8** ලෙස save කරන්න. අකුරු නිවැරදිව නොපෙනේ නම්
editor font එකට Sinhala සහාය ඇති `Nirmala UI` හෝ `Iskoola Pota` භාවිත කරන්න.
මෙය code editor එකේ font එකයි; program UI එකේ font එක වෙනම සකසයි.

## Program එක ගමන් කරන පිළිවෙළ

1. `main.cpp` Qt application එක සහ `MainWindow` සාදයි.
2. `MainWindow` පිටු පහ සාදා database එක විවෘත කරයි.
3. Student workshop එක තෝරා දිනය සහ available slots තෝරයි.
4. Form දත්ත `readFormRecord()` මගින් `StudentRecord` එකකට ගනී.
5. `AllocationRules::validate()` දත්ත පරීක්ෂා කරයි.
6. `SqliteDatabaseManager::addRecord()` booking සහ slot rows save කරයි.
7. Save වූ allocation key එක student ට පෙන්වයි; admin dashboard එකේ status වෙනස් කළ හැක.

## Files වල වැඩ

| File / folder | කරන වැඩේ |
|---|---|
| `StudentWorkshopApp.pro` | Build කරන C++ files, headers, UI සහ resources ලැයිස්තුගත කරයි. |
| `main.cpp` | Program ආරම්භය, මුල් font, application නම, icon සහ window පෙන්වීම. |
| `ui/mainwindow.cpp` | Page navigation, buttons, form, calendar, admin table සහ email වැඩ. |
| `ui/mainwindow.h` | ඉහත functions වල නම් සහ MainWindow මතකයේ තබන දත්ත. |
| `ui/designertheme.cpp` / `.h` | Designer styles light/dark mode එකට ගළපයි. |
| `ui/forms/` | Qt Designer මගින් වෙනස් කළ හැකි පිටු සහ dialogs. |
| `ui/dialogs/customalertdialog.cpp` / `.h` | Messages, warnings, confirmation සහ highlighted allocation key. |
| `ui/dialogs/custominputdialog.cpp` / `.h` | Passcode/index වැනි එක් text input එකක් ඉල්ලයි. |
| `ui/dialogs/adminsettingsdialog.cpp` / `.h` | අනිවාර්ය admin emails දෙක පරීක්ෂා කර save කරයි. |
| `ui/dialogs/systemsettingsdialog.cpp` / `.h` | Theme, wallpaper සහ admin/master passcodes වෙනස් කරයි. |
| `ui/dialogs/dialogtheme.cpp` / `.h` | Save කළ theme එක dialogs වලට යොදයි. |
| `ui/dialogs/customdialogs.h` | Dialog headers හතර එක තැනින් include කරයි. |
| `ui/widgets/customcalendarwidget.cpp` / `.h` | Calendar දිනවල A/P/R booking ගණන් අඳියි. |
| `models/studentrecord.h` | එක් booking එකක දත්ත සහ ඒවා කියවන/වෙනස් කරන getters/setters. |
| `logic/allocationrules.cpp` / `.h` | Form validation, දෝෂ messages, new key සහ edit status නීති. |
| `database/idatabasemanager.h` | Database class එකට අවශ්‍ය පොදු functions ප්‍රකාශ කරයි. |
| `database/databasemanager.cpp` / `.h` | SQLite tables, queries, save/edit/delete සහ පරණ දත්ත migration. |
| `utilities/workshopcatalog.cpp` / `.h` | Workshops දහයේ නම්, departments සහ images. |
| `utilities/filestorage.cpp` / `.h` | භාවිත කරන database file path එක තෝරයි. |
| `resources/images.qrc` | Image files executable එකට ඇතුළත් කර resource aliases ලබාදෙයි. |
| `resources/logo/logo.png` | Application icon සහ welcome logo. |
| `resources/workshops and laboratory/` | Workshop images දහය. |

`.h` file එකක function නම සහ arguments සඳහන් කරයි.
එම function එකේ වැඩ සාමාන්‍යයෙන් අදාළ `.cpp` file එකේ ලියා ඇත.
`StudentRecord` හි කුඩා getters/setters පමණක් header එකේම ලියා ඇත.

## UI පිටු සහ dialogs

| Page / file | භාවිතය |
|---|---|
| 0 - `welcome.ui` | Logo, title, Allocate now, Edit allocation සහ Settings. |
| 1 - `selection.ui` | Workshops දහය rows දෙකක, row එකකට cards පහ බැගින්. |
| 2 - `calander.ui` | Workshop විස්තර, calendar සහ time buttons හතර. |
| 3 - `form.ui` | Student, supervisor, project සහ description inputs. |
| 4 - `admin.ui` | Booking table, date filter, edit/delete, status, remarks සහ email actions. |
| `customalertdialog.ui` | පණිවිඩ සහ Confirm/Cancel dialog පෙනුම. |
| `custominputdialog.ui` | එක් input එකක් ඉල්ලන dialog පෙනුම. |
| `adminsettingsdialog.ui` | Lab/Workshop In-charge සහ Department/head email inputs. |
| `systemsettingsdialog.ui` | Theme, wallpaper සහ administrator passcode controls. |

Qt Creator හි **Design** view එකෙන් `.ui` file එක විවෘත කර controls drag/resize කළ හැක.
Text, font, stylesheet සහ geometry properties වෙනස් කරන්න.
සිංහල XML comments කියවීමට එම file එකේ code/XML view භාවිත කරන්න.

`geometry` හි `x`, `y` යනු control එකේ ස්ථානයයි; `width`, `height` යනු ප්‍රමාණයයි.
විශාල window එකකට controls ගළපන්නේ `DesignerPageScroll::resizeEvent()` මගිනි.
කුඩා window එකකදී මුල් canvas එක scroll කිරීමට දෙයි.

## වෙනස් කරන්න අවශ්‍ය දේ අනුව බලන්න

| අවශ්‍ය වෙනස්කම | වෙනස් කළ යුතු තැන |
|---|---|
| Program නම / මුල් font | `main.cpp`; window title සඳහා `MainWindow` constructor එකත් බලන්න. |
| Page text, buttons, input ස්ථාන | අදාළ `ui/forms/*.ui` file එකේ properties. |
| Button එක ක්ලික් කළ විට කරන වැඩේ | `ui/mainwindow.cpp` හි `connect(...)` සහ එයට අදාළ function එක. |
| Workshop නම් සහ අනුපිළිවෙළ | `workshopCategories()` සහ `selection.ui` හි card/category/text. |
| Workshop department | `workshopDepartment()` සහ selection card එකේ department label. |
| Workshop image / logo path | `resources/images.qrc` හි `<file>` ඇතුළත path. |
| Time slots | `setupDynamicUI()` හි `timeSlotValues` සහ `calander.ui` time button text. |
| Input උපරිම දිග | `setupDynamicUI()` හි අදාළ `setMaxLength(...)`. |
| අනිවාර්ය fields / validation | `AllocationRules::validate()`; database validation සඳහා `validateRecord()`. |
| Form දෝෂ message | `AllocationRules::validationMessage()`. |
| Allocation key අංක ගණන | `prepareNewAllocation()` හි random range සහ `rightJustified(...)`. |
| Admin table column පළල | `setupDynamicUI()` හි `widths` list එක. |
| Admin table data columns | `loadAdminData()` හි `values` list සහ `admin.ui` column headings. |
| Calendar badge පාට / size | `CustomCalendarWidget::paintCell()` සහ constructor එක. |
| Light/dark colours | `.ui` stylesheets සහ `ui/designertheme.cpp` හි `replacements` map එක. |
| Message dialog / key badge පෙනුම | `customalertdialog.ui`, `highlightText()` සහ `fitMessage()`. |
| Email subject/body | `composeStatusEmail()` හි `subject` සහ `body` values. |
| Admin emails save/validate කිරීම | `ui/dialogs/adminsettingsdialog.cpp`. |
| Passcode settings වැඩ | `ui/dialogs/systemsettingsdialog.cpp`. |
| Database folder/file නම | `utilities/filestorage.cpp` හි `dataFolder` සහ `dataPath`. |
| Database columns / queries | `schemaStatements()`, `recordColumns`, `readRecord()` සහ write functions. |

### Images වෙනස් කිරීම

Logo එක `resources/logo/logo.png` හිත් workshop images
`resources/workshops and laboratory/` හිත් තබා ඇත.
File එක වෙනත් folder එකකට ගෙන ගියොත් `images.qrc` හි එහි path එක වෙනස් කරන්න.

```xml
<file alias="logo.png">logo/logo.png</file>
```

මෙහි `logo/logo.png` සැබෑ file path එකයි; `.qrc` file එක ඇති folder එකට සාපේක්ෂ වේ.
`alias="logo.png"` නිසා code එකේ `:/branding/logo.png` යන නමෙන් එය භාවිත වේ.
Workshop images සඳහා `:/workshops/<category>.png` භාවිත වේ.
Alias එකත් වෙනස් කළොත් C++ සහ `.ui` image references ඒ නමට ගළපන්න.

### Buttons සහ object names

`.ui` file එකේ `name` යනු C++ එකේ `ui->name` ලෙස භාවිත කරන control නමයි.
එය වෙනස් කළොත් එම නම භාවිත කරන C++ references ද වෙනස් කරන්න.
Button එකේ user ට පෙන්වන වචන වෙනස් කිරීමට `text` property එක වෙනස් කරන්න.

```cpp
// Submit button එක ක්ලික් කළ විට save function එක ක්‍රියා කරයි.
connect(formUi->submitBtn, &QPushButton::clicked,
        this, &MainWindow::submitData);
```

`ui_*.h`, `moc_*.cpp`, `qrc_*.cpp` යනු build එකේදී Qt සාදන files වේ.
UI වෙනස් කිරීමට මුල් `.ui` file එක භාවිත කරන්න; generated file නැවත build වෙද්දී සෑදේ.
අලුත් source/header/UI/resource file එකක් එක් කළොත් `.pro` හි අදාළ ලැයිස්තුවටත් එක් කරන්න.

## Database දත්ත

සාමාන්‍ය path එක project එකේ `data/student_data.db` වේ.
එම file එක නැති විට working folder, executable folder හෝ local app data folder එකේ
කලින් database එකක් තිබේදැයි සොයයි. කිසිවක් නැත්නම් අලුත් database එකක් සාදයි.

| Table | තබන දත්ත |
|---|---|
| `workshops` | Workshop category එක සහ එහි department එක. |
| `students` | එක් student index එකකට profile එකක්. |
| `records` | Booking එකේ date, project, supervisor, status සහ අනෙක් විස්තර. |
| `booking_slots` | එක් තෝරාගත් slot එකකට row එකක්; key එක `(record_id, time_slot)`. |

Student සහ workshop departments වෙනස් දත්ත දෙකකි.
Student profile එක වෙනස් කළොත් එම index එකේ සියලු bookings එකම අලුත් profile එක පෙන්වයි.
Slot values database එකේ වෙන්ව ගබඩා කර UI එකට පමණක් text list එකක් හදයි.

`Pending` සහ `Approve` bookings slots වෙන්කර තබයි.
`Reject` booking එකේ history රැඳුණත් එහි slots නැවත වෙන්කරගත හැක.
Student edit එකක් save කළ විට status එක නැවත `Pending` වේ.
Student ට දිනක් වෙන්කිරීමේදී අද හෝ ඉදිරි දිනයක් සහ අවම වශයෙන් slot එකක් අවශ්‍යයි.

Tables දැනට schema version 4 භාවිත කරයි. Column වෙනස් කිරීමට `schemaStatements()`
පමණක් වෙනස් නොකර, `createTable()` හි version checks/migration සහ queries ද ගළපන්න.
පැරණි database එකක් migrate කරන විට `.pre-4nf.bak` backup එකක් සාදයි.

## Administrator settings

Master admin සහ workshops සියල්ලේ මුල් admin passcode එක `0000` වේ.
Save කර ඇති passcode එකක් තිබේ නම් program එක එය භාවිත කරයි.
Settings dialog එකෙන් workshop හෝ System administrator තෝරා නව passcode එක save කරන්න.

Theme, wallpaper, passcodes සහ admin emails `QSettings` මගින් save වේ.
Windows හි සාමාන්‍ය native settings registry එකේ තබයි; booking database එකේ ඒවා නැහැ.
`dean_email` යනු පැරණි settings key නමයි; දැන් එහි **Lab / Workshop In-charge Email** තබයි.
අනෙක් email එක `dep_head_email` යන key එකේ තබයි. Emails දෙකම valid සහ අනිවාර්යයි.

Email buttons status එක save කර configured mail application එකේ draft එකක් විවෘත කරයි.
Email එක යැවීම user විසින් mail application එකේදී කරයි.

## වෙනස්කමක් කළ පසු

1. File එක UTF-8 ලෙස save කර project එක නැවත build කරන්න.
2. වෙනස් කළ page/button එක run කර පරීක්ෂා කරන්න.
3. Validation වෙනස් කළා නම් නිවැරදි input එකක් සහ වැරදි input එකක් දෙකම බලන්න.
4. Database වැඩ වෙනස් කළා නම් save/edit/delete සහ slot availability පරීක්ෂා කරන්න.

Comments කියවීම සඳහා පමණි; ඒවා executable code එකේ කොටසක් නොවේ.
