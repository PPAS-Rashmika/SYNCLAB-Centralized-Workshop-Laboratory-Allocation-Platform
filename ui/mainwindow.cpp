// පිටු පහේ වැඩ මෙහි පාලනය කරයි. Buttons, bookings, calendar සහ admin වැඩ පහත functions වල ඇත.
// #include වලින් මේ file එකට අවශ්‍ය Qt සහ project declarations ලබාගනී.
#include "mainwindow.h"
#include "designertheme.h"
#include <QStackedWidget>
#include "ui_welcome.h"
#include "ui_selection.h"
#include "ui_calander.h"
#include "ui_form.h"
#include "ui_admin.h"
#include "customdialogs.h"
#include "workshopcatalog.h"
#include "filestorage.h"
#include "allocationrules.h"
#include <QComboBox>
#include <QDesktopServices>
#include <QDir>
#include <QFileInfo>
#include <QHeaderView>
#include <QFont>
#include <QIcon>
#include <QLabel>
#include <QLineEdit>
#include <QMap>
#include <QPalette>
#include <QPixmap>
#include <QPushButton>
#include <QScrollArea>
#include <QSettings>
#include <QSignalBlocker>
#include <QStatusBar>
#include <QTableWidget>
#include <QTextCharFormat>
#include <QTimer>
#include <QUrl>
#include <QUrlQuery>
#include <QStyle>
#include <QResizeEvent>

// නම් නැති namespace එකේ helpers මේ file එක තුළට පමණක් සීමා වේ.
namespace {
class DesignerPageScroll final : public QScrollArea
{
// අනෙක් code වලින් භාවිත කළ හැකි functions.
public:
    // Designer පිටුව scroll area එකකට දමා මුල් ප්‍රමාණය මතක තබයි.
    DesignerPageScroll(QWidget *parent, QWidget *canvas)
        : QScrollArea(parent), baseSize(canvas->size())
    {
        capture(canvas);
        // Scroll area border ඉවත් කර Designer canvas එකම පෙන්වයි.
        setFrameShape(QFrame::NoFrame);
        setWidgetResizable(false);
        setAlignment(Qt::AlignLeft | Qt::AlignTop);
        setWidget(canvas);
    }

// මෙම class එකට සහ එයින් ලබාගත් classes වලට භාවිත කළ හැකි functions.
protected:
    // කවුළුවේ ප්‍රමාණය වෙනස් වූ විට ඇතුළත controls නැවත ගළපයි.
    void resizeEvent(QResizeEvent *event) override
    {
        // කවුළුවේ ප්‍රමාණය වෙනස් වූ විට ඇතුළත controls නැවත ගළපයි.
        QScrollArea::resizeEvent(event);
        const QSize target = maximumViewportSize().expandedTo(baseSize);
        // මුල් width/height සහ අලුත් ප්‍රමාණයේ අනුපාතයෙන් controls resize කරයි.
        const double sx = double(target.width()) / baseSize.width();
        const double sy = double(target.height()) / baseSize.height();
        widget()->resize(target);
        for (const auto &item : items) {
            const QRect &r = item.geometry;
            item.widget->setGeometry(qRound(r.x() * sx), qRound(r.y() * sy),
                                     qRound(r.width() * sx), qRound(r.height() * sy));
        }
    }

// මෙම class එක තුළ පමණක් භාවිත කරන functions සහ දත්ත.
private:
    // එක් control එක සහ එහි මුල් ස්ථානය/ප්‍රමාණය එකම entry එකක් ලෙස තබයි.
    struct Item { QWidget *widget; QRect geometry; };
    QSize baseSize;
    QList<Item> items;

    // පිටුවේ controls වල මුල් ස්ථාන සහ ප්‍රමාණ සටහන් කරයි; resize වෙද්දී ඒවා භාවිත කරයි.
    void capture(QWidget *container)
    {
        for (QWidget *child : container->findChildren<QWidget *>(QString(), Qt::FindDirectChildrenOnly)) {
            items.append({child, child->geometry()});
            // ඇතුළත scroll area එකක් තිබේ නම් එහි controls ද මතක තබයි.
            if (auto *scroll = qobject_cast<QScrollArea *>(child)) {
                if (scroll->widget()) {
                    items.append({scroll->widget(), scroll->widget()->geometry()});
                    capture(scroll->widget());
                }
            } else if (child->metaObject()->className() == QByteArray("QWidget")
                       || child->metaObject()->className() == QByteArray("QFrame")) {
                capture(child);
            }
        }
    }
};

// පිටුවක් scroll wrapper එක සමඟ stack එකට එක් කරයි. index එකෙන් පිටුව හඳුනාගනී.
void addDesignerPage(QStackedWidget *stack, QWidget *canvas, int index)
{
    auto *scroll = new DesignerPageScroll(stack, canvas);
    scroll->setObjectName("designerPageScroll" + QString::number(index));
    stack->addWidget(scroll);
}
}


// පිටු පහ සාදා database එක විවෘත කර buttons සහ ඒවායේ functions සම්බන්ධ කරයි.
MainWindow::MainWindow(QWidget *parent, const QString &databasePath)
    : QMainWindow(parent), welcomeUi(new Ui::welcomeWidget), selectionUi(new Ui::selectionWidget),
      calanderUi(new Ui::calanderWidget), formUi(new Ui::formWidget), adminUi(new Ui::adminWidget)
{
    setObjectName("MainWindow");
    // Window එකේ මුල් ප්‍රමාණයයි; Designer page ප්‍රමාණයත් සමඟ ගළපන්න.
    resize(1180, 780);
    setWindowTitle(tr("SYNCLAB | Centralized Workshop & Laboratory Allocation Platform"));
    setWindowIcon(QIcon(":/branding/logo.png"));
    setFont(QFont("Segoe UI", 10));
    // පොදු UI පාට සහ borders මෙහි ඇත; # colour codes සහ button rules වෙනස් කළ හැක.
    setStyleSheet(QStringLiteral(R"CSS(
QWidget { color:#17243B; }
QMainWindow, QStackedWidget, QWidget#welcomeWidget, QWidget#selectionWidget, QWidget#calanderWidget, QWidget#formWidget, QWidget#adminWidget { background:#F4F7FB; }
QScrollArea { border:none; background:transparent; }
 QScrollArea > QWidget > QWidget { background:#F4F7FB; }
QScrollArea#cardsScrollArea > QWidget, QWidget#cardsScrollContents { background:transparent; }
QLabel { background:transparent; }
QLabel#welcomeTitle { color:#52647D; }
QLabel#syncLabTitle { }
QLabel#syncLabSubtitle { }
QLabel#welcomeDesc, QLabel#selectedDescLabel2, QLabel#adminWsDescLbl { color:#52647D; }
QLabel#selectTitleLabel, QLabel#selectedNameLabel2, QLabel#confirmTitleLbl, QLabel#adminWsTitleLbl { }
QLineEdit, QTextEdit, QComboBox { background:#FFFFFF; border:1px solid #D4DEEB; border-radius:7px; padding:8px; selection-background-color:#2563EB; selection-color:white; }
QLineEdit:focus, QTextEdit:focus, QComboBox:focus { border:1px solid #2563EB; }
QComboBox QAbstractItemView { background:#FFFFFF; color:#17243B; selection-background-color:#2563EB; selection-color:white; }
QPushButton { background:#FFFFFF; color:#17243B; border:1px solid #D4DEEB; border-radius:8px; padding:9px 14px; }
QPushButton:hover, QPushButton:focus { background:#EAF2FF; border-color:#2563EB; }
QPushButton:pressed { border:2px solid #2563EB; }
QPushButton#allocateNowBtn, QPushButton#nextBtn, QPushButton#nextTo3Btn, QPushButton#submitBtn, QPushButton#updateBtn { background:#2563EB; color:white; border:1px solid #2563EB; }
QPushButton#allocateNowBtn:hover, QPushButton#nextBtn:hover, QPushButton#nextTo3Btn:hover, QPushButton#submitBtn:hover, QPushButton#updateBtn:hover { background:#1D4ED8; }
QPushButton#deleteBtn, QPushButton#adminDelBtn { color:#BE123C; }
QPushButton[timeSlot=true] { padding:10px 8px; }
QPushButton:checked { background:#2563EB; color:white; border-color:#2563EB; }
QPushButton:disabled { background:#F4F7FB; color:#52647D; border:1px solid #D4DEEB; }
QPushButton#allocateNowBtn:disabled, QPushButton#nextBtn:disabled, QPushButton#nextTo3Btn:disabled, QPushButton#submitBtn:disabled, QPushButton#updateBtn:disabled, QPushButton#deleteBtn:disabled, QPushButton#adminDelBtn:disabled { background:#F4F7FB; color:#52647D; border:1px solid #D4DEEB; }
QFrame#summaryBox { background:#EAF2FF; border:1px solid #D4DEEB; border-radius:10px; }
QLabel#summaryDateLbl { color:#2563EB; }
QTableWidget { background:#FFFFFF; alternate-background-color:#F4F7FB; border:1px solid #D4DEEB; gridline-color:#D4DEEB; selection-background-color:#EAF2FF; selection-color:#17243B; }
QHeaderView::section { background:#EAF2FF; color:#17243B; border:none; padding:10px; }
QTableWidget QLineEdit, QTableWidget QComboBox { padding:4px; border-radius:4px; }
QCalendarWidget { background:#FFFFFF; border:1px solid #D4DEEB; border-radius:8px; }
QCalendarWidget QWidget#qt_calendar_navigationbar { background:#2563EB; }
QCalendarWidget QToolButton { color:white; background:transparent; padding:6px; }
QCalendarWidget QMenu, QCalendarWidget QSpinBox { background:#FFFFFF; color:#17243B; }
QCalendarWidget QAbstractItemView { background:#FFFFFF; color:#17243B; selection-background-color:#2563EB; selection-color:white; outline:0; }
QToolTip { background:#FFFFFF; color:#17243B; border:1px solid #D4DEEB; padding:6px; }
)CSS"));
    // එක් වරකට එක් පිටුවක් පෙන්වන page stack එකයි.
    stackedWidget = new QStackedWidget(this);
    stackedWidget->setObjectName("stackedWidget");
    setCentralWidget(stackedWidget);
    // Page අංක: 0 welcome, 1 selection, 2 calander, 3 form, 4 admin.
    welcomeWidget = new QWidget();
    welcomeUi->setupUi(welcomeWidget);
    addDesignerPage(stackedWidget, welcomeWidget, 0);
    selectionWidget = new QWidget();
    selectionUi->setupUi(selectionWidget);
    addDesignerPage(stackedWidget, selectionWidget, 1);
    calanderWidget = new QWidget();
    calanderUi->setupUi(calanderWidget);
    addDesignerPage(stackedWidget, calanderWidget, 2);
    formWidget = new QWidget();
    formUi->setupUi(formWidget);
    addDesignerPage(stackedWidget, formWidget, 3);
    adminWidget = new QWidget();
    adminUi->setupUi(adminWidget);
    addDesignerPage(stackedWidget, adminWidget, 4);

    // Test එකෙන් path එකක් නොදුන්නොත් filestorage.cpp තෝරන database එක භාවිත කරයි.
    const QString path = databasePath.isEmpty() ? defaultDatabasePath() : databasePath;
    dbManager = new SqliteDatabaseManager(path);
    databaseReady = dbManager->createTable();

    // Designer දී තැබූ text මතක තබයි; edit වැඩේ අවසන් කළ විට එය නැවත යොදයි.
    calanderUi->backTo1Btn->setProperty("_designerBackText", calanderUi->backTo1Btn->text());
    formUi->confirmTitleLbl->setProperty("_designerNewTitle", formUi->confirmTitleLbl->text());
    bindWorkshopCards();
    setupDynamicUI();
    applyTheme(QSettings(QSettings::defaultFormat(), QSettings::UserScope, "StudentWorkshopApp",
                         "ThemeSettings")
                   .value("darkMode", false)
                   .toBool());
    stackedWidget->setCurrentIndex(0);

    // Allocate now button එක new booking function එකට සම්බන්ධ කරයි.
    connect(welcomeUi->allocateNowBtn, &QPushButton::clicked, this, &MainWindow::startNewAllocation);
    // Edit allocation button එකෙන් allocation key ඉල්ලයි.
    connect(welcomeUi->editAllocationBtn, &QPushButton::clicked, this, &MainWindow::startEditAllocationFlow);
    // Settings button එකෙන් master passcode පරීක්ෂාව ආරම්භ කරයි.
    connect(welcomeUi->settingsBtn0, &QPushButton::clicked, this, &MainWindow::openSystemSettings);
    // Selection Back button එකෙන් save නොකළ වෙනස්කම් ගැන අසා ආපසු යයි.
    connect(selectionUi->backTo0Btn, &QPushButton::clicked, this, &MainWindow::cancelCurrentFlow);
    // Selection Next button එක calendar පිටුවට ගෙන යයි.
    connect(selectionUi->nextBtn, &QPushButton::clicked, this, &MainWindow::onNextClicked);
    // Calendar Back button එක අදාළ පෙර පිටුවට ගෙන යයි.
    connect(calanderUi->backTo1Btn, &QPushButton::clicked, this, &MainWindow::goBackTo1);
    // Calendar Next button එකෙන් දිනය/slots form summary එකට ගනී.
    connect(calanderUi->nextTo3Btn, &QPushButton::clicked, this, &MainWindow::onNextTo3Clicked);
    // Form Back button එක calendar පිටුවට ගෙන යයි.
    connect(formUi->backTo2Btn, &QPushButton::clicked, this, &MainWindow::goBackTo2);
    // දිනය මාරු වූ විට available slots නැවත ලබාගනී.
    connect(calanderUi->calendar, &QCalendarWidget::selectionChanged, this,
            &MainWindow::onCalendarSelectionChanged);
    // Submit button එකෙන් අලුත් booking එක save කරයි.
    connect(formUi->submitBtn, &QPushButton::clicked, this, &MainWindow::submitData);
    // Update button එකෙන් පවතින booking වෙනස්කම් save කරයි.
    connect(formUi->updateBtn, &QPushButton::clicked, this, &MainWindow::updateData);
    // Delete button එකෙන් තහවුරු කිරීමෙන් පසු booking එක මකයි.
    connect(formUi->deleteBtn, &QPushButton::clicked, this, &MainWindow::deleteData);
    // Administrator button එකෙන් workshop admin passcode ඉල්ලයි.
    connect(calanderUi->adminBtn2, &QPushButton::clicked, this, &MainWindow::showRecordsWindow);
    // Admin Back button එක welcome පිටුවට ගෙන යයි.
    connect(adminUi->adminBackBtn, &QPushButton::clicked, this, &MainWindow::goBackFromAdmin);
    // Admin Settings button එකෙන් emails දෙක වෙනස් කරයි.
    connect(adminUi->adminSettingsBtn, &QPushButton::clicked, this,
            &MainWindow::openAdminEmailSettings);
    // Show all button එකෙන් date filter ඉවත් කරයි.
    connect(adminUi->adminShowAllBtn, &QPushButton::clicked, this, &MainWindow::adminClearFilter);
    // Admin calendar date selection වෙනස් වුණොත් table එක filter කරයි.
    connect(adminUi->adminCalendar, &QCalendarWidget::selectionChanged, this,
            [this] { adminFilterByDate(adminUi->adminCalendar->selectedDate()); });
    // එම date එක click කළත් admin filter එක යොදයි.
    connect(adminUi->adminCalendar, &QCalendarWidget::clicked, this, &MainWindow::adminFilterByDate);
    // Table selection වෙනස් වෙද්දී admin action buttons enable/disable කරයි.
    connect(adminUi->mainTable, &QTableWidget::itemSelectionChanged, this,
            &MainWindow::refreshAdminActions);

    // තෝරාගත් row එකේ hidden ID එකෙන් booking එක සොයා edit form එකට ගෙන යයි.
    connect(adminUi->adminEditBtn, &QPushButton::clicked, this, [this] {
        const int row = adminUi->mainTable->currentRow();
        if (row < 0)
            return;
        const int id = adminUi->mainTable->item(row, 0)->data(Qt::UserRole).toInt();
        const auto records = dbManager->getAllRecordsForCategory(currentCategory);
        if (!dbManager->getLastError().isEmpty()) {
            showDatabaseError();
            return;
        }
        for (const auto &record : records)
            if (record.getId() == id) {
                loadRecordForEdit(record);
                return;
            }
        loadAdminData();
    });

    // තෝරාගත් booking එක මකන්න user ගෙන් තහවුරු කර නැවත table/calendar සකසයි.
    connect(adminUi->adminDelBtn, &QPushButton::clicked, this, [this] {
        const int row = adminUi->mainTable->currentRow();
        if (row < 0)
            return;
        const int id = adminUi->mainTable->item(row, 0)->data(Qt::UserRole).toInt();
        if (CustomAlertDialog("Delete allocation", "Permanently delete the selected allocation?",
                              CustomAlertDialog::Question, this)
                .exec() != QDialog::Accepted)
            return;
        if (!dbManager->deleteRecord(id)) {
            showDatabaseError();
            return;
        }
        loadAdminData();
        updateCalendarHighlights();
    });

    // Pending status එක save කර email draft එක විවෘත කරයි.
    connect(adminUi->pendingEmailBtn, &QPushButton::clicked, this,
            [this] { composeStatusEmail("Pending"); });
    // Reject status එක save කර email draft එක විවෘත කරයි.
    connect(adminUi->rejectEmailBtn, &QPushButton::clicked, this,
            [this] { composeStatusEmail("Reject"); });

    // Database එක අසාර්ථක නම් booking buttons අක්‍රිය කර දෝෂය පෙන්වයි.
    if (!databaseReady) {
        welcomeUi->allocateNowBtn->setEnabled(false);
        welcomeUi->editAllocationBtn->setEnabled(false);
        QTimer::singleShot(0, this, &MainWindow::showDatabaseError);
    }
}

// Window එක වසන විට database එක සහ new වලින් හදපු UI helpers නිදහස් කරයි.
MainWindow::~MainWindow()
{
    delete dbManager;
    delete welcomeUi;
    delete selectionUi;
    delete calanderUi;
    delete formUi;
    delete adminUi;
}

// Time slots, table column පළල සහ input සීමා මෙහි සකසයි. ඒ අගයන් වෙනස් කිරීමට පහත ලැයිස්තු බලන්න.
void MainWindow::setupDynamicUI()
{
    // Cards අතර හිස් ප්‍රදේශයෙන් page wallpaper එක පෙනීමට ඉඩ දෙයි.
    selectionUi->cardsScrollArea->viewport()->setAutoFillBackground(false);
    selectionUi->cardsScrollContents->setAutoFillBackground(false);

    // Slots වෙනස් කිරීමට මෙයත් calander.ui හි time buttons වල text එකත් ගළපන්න.
    timeSlotValues = {"8.30 am - 10.30 am", "10.30 am - 12.30 pm", "1.30 pm - 3.30 pm",
                      "3.30 pm - 5.30 pm"};
    // ඉහත values සහ buttons එකම අනුපිළිවෙළේ තිබිය යුතුයි.
    timeSlotButtons = {calanderUi->timeBtn1, calanderUi->timeBtn2, calanderUi->timeBtn3, calanderUi->timeBtn4};
    for (int i = 0; i < timeSlotButtons.size(); ++i) {
        // CSS හි timeSlot=true rules මෙම buttons වලට යොදයි.
        timeSlotButtons[i]->setProperty("timeSlot", true);
        connect(timeSlotButtons[i], &QPushButton::toggled, this, &MainWindow::onTimeSlotClicked);
    }

    // නව bookings වලට අද හෝ ඉදිරි දිනයක් පමණක් තෝරාගැනීමට දෙයි.
    calanderUi->calendar->setMinimumDate(QDate::currentDate());
    calanderUi->calendar->setVerticalHeaderFormat(QCalendarWidget::NoVerticalHeader);
    adminUi->adminCalendar->setVerticalHeaderFormat(QCalendarWidget::NoVerticalHeader);

    adminUi->mainTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    // Admin table columns 11 හි පළලයි; අගයන් column අනුපිළිවෙළින් වෙනස් කරන්න.
    const QList<int> widths = {155, 150, 100, 150, 200, 200, 140, 210, 200, 190, 130};
    for (int i = 0; i < widths.size(); ++i)
        adminUi->mainTable->setColumnWidth(i, widths[i]);
    adminUi->mainTable->horizontalHeader()->setStretchLastSection(true);
    adminUi->mainTable->verticalHeader()->hide();
    adminUi->mainTable->verticalHeader()->setDefaultSectionSize(44);
    adminUi->mainTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    adminUi->mainTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    adminUi->mainTable->setSelectionMode(QAbstractItemView::SingleSelection);
    adminUi->mainTable->setAlternatingRowColors(true);
    adminUi->mainTable->setWordWrap(false);
    adminUi->mainTable->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);

    formUi->deptInput->setSizeAdjustPolicy(QComboBox::AdjustToMinimumContentsLengthWithIcon);
    formUi->deptInput->setMinimumContentsLength(16);
    // Input වල උපරිම text දිග මෙහි සකසයි; අවශ්‍ය field එකේ අගය වෙනස් කරන්න.
    formUi->nameInput->setMaxLength(160);
    formUi->indexInput->setMaxLength(80);
    formUi->emailInput->setMaxLength(254);
    formUi->supervisorInput->setMaxLength(160);
    formUi->projectInput->setMaxLength(200);
    formUi->descriptionInput->setAcceptRichText(false);
    formUi->summaryDateLbl->setWordWrap(true);
    formUi->summaryTimeLbl->setWordWrap(true);
    setFormButtonsMode(false);
    refreshAdminActions();
}

// Database වැඩේ අසාර්ථක වූ හේතුව user ට පෙන්වයි.
void MainWindow::showDatabaseError()
{
    const QString error = dbManager->getLastError();
    // Title/message සහ OK/Cancel සකසයි; message වර්ගයට ගැළපෙන පාට තෝරයි.
    CustomAlertDialog("Could not save or load allocation",
                      error.isEmpty() ? "The database is unavailable. Please try again." : error,
                      CustomAlertDialog::Error, this)
        .exec();
}


// තෝරාගත් theme සහ wallpaper පිටු පහට යොදයි; calendar සහ card පාටත් නැවත සකසයි.
void MainWindow::applyTheme(bool darkMode)
{
    isDarkTheme = darkMode;
    applyDesignerTheme(this, darkMode);

    // Save කළ wallpaper path එක කියවා file එක තිබේ නම් පමණක් පිටුවල පෙන්වයි.
    QString wallpaper = QSettings(QSettings::defaultFormat(), QSettings::UserScope,
                                  "StudentWorkshopApp", "ThemeSettings")
                            .value("wallpaper")
                            .toString();
    if (QFileInfo::exists(wallpaper)) {
        wallpaper = QDir::fromNativeSeparators(QFileInfo(wallpaper).absoluteFilePath());
        wallpaper.replace('"', "\\\"");
        for (QWidget *page : {welcomeWidget, selectionWidget, calanderWidget, formWidget, adminWidget})
            page->setStyleSheet(
                QString("QWidget#%1 { border-image:url(\"%2\") 0 0 0 0 stretch stretch; }")
                    .arg(page->objectName(), wallpaper) + page->styleSheet());
    }

    selectCard(selectedCategory);

    updateCalendarHighlights();
    updateAdminCalendarHighlights();
}

// selection.ui හි card category එක කියවා එහි button එක selectCard() වෙත සම්බන්ධ කරයි.
void MainWindow::bindWorkshopCards()
{
    for (QFrame *card : selectionUi->cardsScrollContents->findChildren<QFrame *>()) {
        // Card එකේ category property එක workshopCategories() හි නමට ගළපන්න.
        const QString category = card->property("category").toString();
        if (category.isEmpty())
            continue;
        cardFrames.append(card);
        auto *button = card->findChild<QPushButton *>();
        if (button)
            connect(button, &QPushButton::clicked, this, [this, category] { selectCard(category); });
    }
}

// තෝරාගත් card එකට selected ලකුණ දී Next button එක ක්‍රියාත්මක කරයි.
void MainWindow::selectCard(const QString &category)
{
    selectedCategory = category;
    for (QFrame *card : cardFrames) {
        const bool selected = card->property("category").toString() == category;
        card->setProperty("selected", selected);
        // Selected property මාරු වූ පසු card CSS එක නැවත යොදයි.
        card->style()->unpolish(card);
        card->style()->polish(card);
        card->update();
        if (auto *button = card->findChild<QPushButton *>())
            button->setAccessibleDescription(selected ? "Selected workshop" : "Select workshop");
    }
    selectionUi->nextBtn->setEnabled(!category.isEmpty());
}


// දැනට කරන booking වැඩේ සහ form values මුල් තත්ත්වයට ගෙන එයි.
void MainWindow::resetFlow()
{
    currentRecord = StudentRecord();
    currentCategory.clear();
    currentWorkshopId.clear();
    isPasscodeBypassed = false;
    isAdminEditMode = false;
    isStudentEditFlow = false;
    adminFilterDate = QDate();
    calanderUi->calendar->setMinimumDate(QDate::currentDate());
    calanderUi->calendar->setSelectedDate(QDate::currentDate());
    clearUI();
    selectCard(QString());
    calanderUi->adminBtn2->show();
    calanderUi->backTo1Btn->setText(calanderUi->backTo1Btn->property("_designerBackText").toString());
    setFormButtonsMode(false);
}

// නව booking එකක් සඳහා form එක හිස් කර workshop selection පිටුව පෙන්වයි.
void MainWindow::startNewAllocation()
{
    if (!databaseReady)
        return;
    resetFlow();
    stackedWidget->setCurrentIndex(1);
}

// Allocation key එකෙන් booking එක සොයා student ට edit කිරීමට විවෘත කරයි.
void MainWindow::startEditAllocationFlow()
{
    if (!databaseReady)
        return;
    CustomInputDialog dialog("Edit allocation", "Enter your allocation key:", this);
    if (dialog.exec() != QDialog::Accepted)
        return;
    const QString entered = dialog.getInput();

    // Allocation key එක සියලු workshops වල සොයයි.
    QList<StudentRecord> matches;
    for (const QString &category : workshopCategories()) {
        const auto records = dbManager->getAllRecordsForCategory(category);
        if (!dbManager->getLastError().isEmpty()) {
            showDatabaseError();
            return;
        }
        for (const auto &record : records)
            if (record.getPasscode().compare(entered, Qt::CaseInsensitive) == 0)
                matches.append(record);
    }

    // පරණ දත්තවල එකම key එක කිහිපයක තිබේ නම් student index එකෙන් තෝරයි.
    if (matches.size() > 1) {
        CustomInputDialog identity(
            "Confirm allocation",
            "This allocation key matches multiple allocations.\nEnter your student index:", this);
        if (identity.exec() != QDialog::Accepted)
            return;
        const QString identifier = identity.getInput();
        for (auto it = matches.begin(); it != matches.end();)
            if (it->getIndexNo() != identifier)
                it = matches.erase(it);
            else
                ++it;
    }
    if (matches.size() != 1) {
        // Title/message සහ OK/Cancel සකසයි; message වර්ගයට ගැළපෙන පාට තෝරයි.
        CustomAlertDialog("Allocation not found",
                          "The details do not identify a single allocation. Check your allocation key "
                          "or contact the administrator.",
                          CustomAlertDialog::Error, this)
            .exec();
        return;
    }

    resetFlow();
    currentRecord = matches.first();
    currentCategory = currentRecord.getCategory();
    currentWorkshopId = currentCategory;
    isStudentEditFlow = true;
    isPasscodeBypassed = true;
    calanderUi->adminBtn2->hide();
    loadRecordIntoUI(currentRecord);
    selectCard(currentCategory);
    const QDate savedDate = QDate::fromString(currentRecord.getRecordDate(), Qt::ISODate);
    // පරණ දිනයක් ඇති booking එකක් වුවත් form එකට ගොස් බලන්න/මකන්න දෙයි.
    stackedWidget->setCurrentIndex(savedDate < QDate::currentDate() ? 3 : 1);
}

// Save නොකළ දත්ත තිබේ නම් තහවුරු කිරීමක් ඉල්ලා welcome පිටුවට යයි.
void MainWindow::cancelCurrentFlow()
{
    // Save නොකළ inputs තිබේදැයි පරීක්ෂා කර discard ප්‍රශ්නය පෙන්වයි.
    const bool hasDraft =
        isStudentEditFlow || isAdminEditMode || !formUi->nameInput->text().isEmpty() ||
        !formUi->indexInput->text().isEmpty() || !formUi->emailInput->text().isEmpty() ||
        !formUi->supervisorInput->text().isEmpty() || !formUi->projectInput->text().isEmpty() ||
        !formUi->descriptionInput->toPlainText().isEmpty();
    if (hasDraft &&
        // Title/message සහ OK/Cancel සකසයි; message වර්ගයට ගැළපෙන පාට තෝරයි.
        CustomAlertDialog("Discard changes", "Leave this allocation and discard unsaved changes?",
                          CustomAlertDialog::Question, this)
                .exec() != QDialog::Accepted)
        return;
    resetFlow();
    stackedWidget->setCurrentIndex(0);
}

// තෝරාගත් workshop එකට calendar පිටුව පෙන්වයි; workshop මාරු වුණොත් slots ඉවත් කරයි.
void MainWindow::onNextClicked()
{
    if (selectedCategory.isEmpty())
        return;
    const bool changedCategory = currentCategory != selectedCategory;
    openSecondInterface(selectedCategory);
    if (changedCategory)
        setTimeSlotsInUI(QString());
    setFormButtonsMode(isStudentEditFlow || isAdminEditMode);
}

// Workshop නම, department සහ image calendar/admin පිටුවලට දමා calendar පිටුව විවෘත කරයි.
void MainWindow::openSecondInterface(const QString &category)
{
    if (currentCategory != category)
        adminFilterDate = QDate();
    currentCategory = category;
    currentWorkshopId = category;
    calanderUi->selectedNameLabel2->setText(workshopTitle(category));
    calanderUi->selectedDescLabel2->setText(
        workshopDepartment(category) +
        "\n\nChoose a date and one or more available time slots.\nGrey slots are unavailable.");
    adminUi->adminWsTitleLbl->setText(workshopTitle(category));
    // තෝරාගත් image එකේ උපරිම size එක 160x100; label එක කුඩා නම් ඊටත් ගළපයි.
    calanderUi->selectedImgLabel2->setPixmap(
        workshopImage(category, QSize(160, 100).boundedTo(calanderUi->selectedImgLabel2->size())));
    adminUi->adminImgLabel->setPixmap(
        workshopImage(category, QSize(160, 100).boundedTo(adminUi->adminImgLabel->size())));
    updateCalendarHighlights();
    onCalendarSelectionChanged();
    stackedWidget->setCurrentIndex(2);
}

// Slots නැවත පරීක්ෂා කර දිනය/වෙලාව summary එකට දමා form පිටුව පෙන්වයි.
void MainWindow::onNextTo3Clicked()
{
    onCalendarSelectionChanged();
    const QStringList selectedSlots = getSelectedTimeSlotsList();
    if (selectedSlots.isEmpty())
        return;
    formUi->summaryDateLbl->setText(workshopTitle(currentCategory) + "  |  " +
                                 calanderUi->calendar->selectedDate().toString("dddd, d MMMM yyyy"));
    formUi->summaryTimeLbl->setText(selectedSlots.join("  |  "));
    stackedWidget->setCurrentIndex(3);
    formUi->nameInput->setFocus();
}

// Student නම් selection පිටුවටත් admin edit එකක් නම් dashboard එකටත් ආපසු යයි.
void MainWindow::goBackTo1()
{
    // Admin edit වැඩක් නම් dashboard එකට ආපසු යාම සඳහා වෙනම තත්ත්වයක් භාවිත කරයි.
    if (isAdminEditMode) {
        isAdminEditMode = false;
        currentRecord = StudentRecord();
        clearUI();
        loadAdminData();
        stackedWidget->setCurrentIndex(4);
    } else {
        stackedWidget->setCurrentIndex(1);
    }
}

// Form පිටුවෙන් calendar පිටුවට ආපසු යයි.
void MainWindow::goBackTo2()
{
    stackedWidget->setCurrentIndex(2);
}

// Admin වැඩේ අවසන් කර welcome පිටුව පෙන්වයි.
void MainWindow::goBackFromAdmin()
{
    resetFlow();
    stackedWidget->setCurrentIndex(0);
}


// Enable වී tick කළ time buttons වල values ලැයිස්තුවක් ලබාදෙයි.
QStringList MainWindow::getSelectedTimeSlotsList()
{
    QStringList selectedSlots;
    for (int i = 0; i < timeSlotButtons.size(); ++i) {
        if (timeSlotButtons[i]->isChecked() && timeSlotButtons[i]->isEnabled())
            selectedSlots.append(timeSlotValues[i]);
    }
    return selectedSlots;
}

// Save කළ slot list එකට ගැළපෙන available time buttons tick කරයි.
void MainWindow::setTimeSlotsInUI(const QString &timeString)
{
    QStringList selectedSlots = timeString.split(',', Qt::SkipEmptyParts);
    for (QString &slot : selectedSlots)
        slot = slot.trimmed();
    for (int i = 0; i < timeSlotButtons.size(); ++i)
        timeSlotButtons[i]->setChecked(timeSlotButtons[i]->isEnabled() &&
                                       selectedSlots.contains(timeSlotValues[i]));
    onTimeSlotClicked();
}

// අවම වශයෙන් slot එකක් තෝරා ඇත්නම් පමණක් Next ක්‍රියාත්මක කරයි.
void MainWindow::onTimeSlotClicked()
{
    calanderUi->nextTo3Btn->setEnabled(!getSelectedTimeSlotsList().isEmpty());
}

// තෝරාගත් දිනට පුරවා ඇති slots සොයා unavailable buttons අක්‍රිය කරයි.
void MainWindow::onCalendarSelectionChanged()
{
    if (currentCategory.isEmpty()) {
        calanderUi->nextTo3Btn->setEnabled(false);
        return;
    }
    const QDate date = calanderUi->calendar->selectedDate();
    // Edit කරන booking ID එක හැර දමන නිසා තමන්ගේ slots තමන්ට block නොවේ.
    const auto allocated = dbManager->getAllocatedTimeSlotsStringList(
        date.toString(Qt::ISODate), currentCategory, currentRecord.getId());
    const bool queryOk = dbManager->getLastError().isEmpty();
    for (int i = 0; i < timeSlotButtons.size(); ++i) {
        QPushButton *button = timeSlotButtons[i];
        // Database query හරි, දිනය පරණ නැති සහ slot එක නොපිරුණු විට පමණක් button එක enable වේ.
        const bool available = queryOk && date >= QDate::currentDate() &&
                               !allocated.contains(timeSlotValues[i]);
        button->setEnabled(available);
        if (!available)
            button->setChecked(false);
        button->setToolTip(
            !queryOk ? "Availability could not be loaded"
                     : (available ? "Select this time slot" : "This time slot is unavailable"));
    }
    onTimeSlotClicked();
}

// Booking status අනුව දිනකට ඇති ගණන් සොයා student calendar එකට දෙයි.
void MainWindow::updateCalendarHighlights()
{
    if (!databaseReady || currentCategory.isEmpty())
        return;
    calanderUi->calendar->setDateTextFormat(QDate(), QTextCharFormat());
    QMap<QDate, QMap<QString, int>> counts;
    for (const auto &record : dbManager->getAllRecordsForCategory(currentCategory)) {
        const QDate date = QDate::fromString(record.getRecordDate(), Qt::ISODate);
        if (date.isValid())
            // මෙම දිනයේ අදාළ status එකේ booking ගණන එකකින් වැඩි කරයි.
            counts[date][record.getStatus()]++;
    }
    calanderUi->calendar->setStatusCounts(counts);
}

// එම booking ගණන් admin calendar එකටත් යොදයි.
void MainWindow::updateAdminCalendarHighlights()
{
    if (!databaseReady || currentCategory.isEmpty())
        return;
    adminUi->adminCalendar->setDateTextFormat(QDate(), QTextCharFormat());
    QMap<QDate, QMap<QString, int>> counts;
    for (const auto &record : dbManager->getAllRecordsForCategory(currentCategory)) {
        const QDate date = QDate::fromString(record.getRecordDate(), Qt::ISODate);
        if (date.isValid())
            counts[date][record.getStatus()]++;
    }
    adminUi->adminCalendar->setStatusCounts(counts);
}


// Student form inputs සහ තෝරාගත් slots හිස් කරයි.
void MainWindow::clearUI()
{
    formUi->nameInput->clear();
    formUi->indexInput->clear();
    formUi->deptInput->setCurrentIndex(0);
    formUi->emailInput->clear();
    formUi->supervisorInput->clear();
    formUi->projectInput->clear();
    formUi->descriptionInput->clear();
    setTimeSlotsInUI(QString());
}

// නව booking එකට Submit; edit එකට Update/Delete පෙන්වයි.
void MainWindow::setFormButtonsMode(bool edit)
{
    formUi->submitBtn->setVisible(!edit);
    formUi->updateBtn->setVisible(edit);
    formUi->deleteBtn->setVisible(edit);
    formUi->confirmTitleLbl->setText(edit ? "Edit your allocation" : formUi->confirmTitleLbl->property("_designerNewTitle").toString());
}

// Save කළ booking දත්ත input fields සහ date/time controls වලට දමයි.
void MainWindow::loadRecordIntoUI(const StudentRecord &record)
{
    currentRecord = record;
    const QDate date = QDate::fromString(record.getRecordDate(), Qt::ISODate);
    calanderUi->calendar->setMinimumDate(
        // Save කළ පරණ booking දිනයත් calendar එකේ තෝරා පෙන්වීමට ඉඩ දෙයි.
        qMin(date.isValid() ? date : QDate::currentDate(), QDate::currentDate()));
    calanderUi->calendar->setSelectedDate(date);
    onCalendarSelectionChanged();
    setTimeSlotsInUI(record.getTimeSlots());
    formUi->nameInput->setText(record.getName());
    formUi->indexInput->setText(record.getIndexNo());
    formUi->deptInput->setCurrentIndex(0);
    formUi->deptInput->setCurrentText(record.getDepartment());
    formUi->emailInput->setText(record.getEmail());
    formUi->supervisorInput->setText(record.getSupervisorName());
    formUi->projectInput->setText(record.getProjectName());
    formUi->descriptionInput->setPlainText(record.getDescription());
    formUi->summaryDateLbl->setText(workshopTitle(currentCategory) + "  |  " +
                                 date.toString("dddd, d MMMM yyyy"));
    formUi->summaryTimeLbl->setText(record.getTimeSlots());
    setFormButtonsMode(true);
}

// Admin තෝරාගත් booking එක form පිටුවට ගෙන edit mode එක සකසයි.
void MainWindow::loadRecordForEdit(const StudentRecord &record)
{
    isAdminEditMode = true;
    isStudentEditFlow = false;
    isPasscodeBypassed = true;
    currentRecord = record;
    openSecondInterface(record.getCategory());
    loadRecordIntoUI(record);
    calanderUi->backTo1Btn->setText("Back to dashboard");
    calanderUi->adminBtn2->hide();
    stackedWidget->setCurrentIndex(3);
}

// Screen එකේ input values කියවා StudentRecord එකකට දමයි.
StudentRecord MainWindow::readFormRecord()
{
    StudentRecord record = currentRecord;
    // Inputs වල මුල/අග spaces ඉවත් කර දත්ත object එකට දමයි.
    record.setName(formUi->nameInput->text().trimmed());
    record.setIndexNo(formUi->indexInput->text().trimmed());
    record.setDepartment(formUi->deptInput->currentText());
    record.setEmail(formUi->emailInput->text().trimmed());
    record.setSupervisorName(formUi->supervisorInput->text().trimmed());
    record.setProjectName(formUi->projectInput->text().trimmed());
    record.setCategory(currentCategory);
    record.setWorkshopId(currentWorkshopId);
    record.setRecordDate(calanderUi->calendar->selectedDate().toString(Qt::ISODate));
    record.setTimeSlots(getSelectedTimeSlotsList().join(", "));
    record.setDescription(formUi->descriptionInput->toPlainText().trimmed());
    return record;
}

// Form එක පරීක්ෂා කර වැරදි field එකට cursor එක ගෙන යයි.
bool MainWindow::validateForm()
{
    const AllocationRules::Field field =
        // Form එකේ පළමු වැරදි field එක ලබාදෙයි; සියල්ල හරි නම් Field::None දෙයි.
        AllocationRules::validate(readFormRecord(), formUi->deptInput->currentIndex() > 0);
    if (field == AllocationRules::Field::None) {
        return true;
    }
    // Title/message සහ OK/Cancel සකසයි; message වර්ගයට ගැළපෙන පාට තෝරයි.
    CustomAlertDialog("Check allocation details", AllocationRules::validationMessage(field),
                      CustomAlertDialog::Warning, this)
        .exec();
    // වැරදි field එකට focus දීමට case එක වෙනස් කරන්න; Schedule නම් calendar පිටුව පෙන්වයි.
    switch (field) {
    case AllocationRules::Field::Name:
        formUi->nameInput->setFocus();
        break;
    case AllocationRules::Field::Index:
        formUi->indexInput->setFocus();
        break;
    case AllocationRules::Field::Department:
        formUi->deptInput->setFocus();
        break;
    case AllocationRules::Field::Email:
        formUi->emailInput->setFocus();
        break;
    case AllocationRules::Field::Supervisor:
        formUi->supervisorInput->setFocus();
        break;
    case AllocationRules::Field::Project:
        formUi->projectInput->setFocus();
        break;
    case AllocationRules::Field::Schedule:
        stackedWidget->setCurrentIndex(2);
        break;
    case AllocationRules::Field::None:
        break;
    }
    return false;
}

// නව booking එක පරීක්ෂා කර database එකට save කර allocation key එක පෙන්වයි.
void MainWindow::submitData()
{
    if (currentRecord.getId() >= 0 || !validateForm())
        return;
    StudentRecord record = readFormRecord();
    // නව booking එකට ඉලක්කම් හතරේ key, created_at සහ Pending status සකසයි.
    AllocationRules::prepareNewAllocation(record);
    if (!dbManager->addRecord(record)) {
        showDatabaseError();
        return;
    }
    CustomAlertDialog saved("Allocation saved",
                            "Your allocation is pending approval.\n\nAllocation key: " +
                                record.getPasscode() +
                                "\n\nKeep your allocation key to edit this allocation.\n" +
                                workshopTitle(currentCategory) + "\n" + record.getRecordDate() +
                                "\n" + record.getTimeSlots(),
                            CustomAlertDialog::Info, this);
    // Save වූ key එක user ට ලේසියෙන් කියවීමට ලොකු badge එකක් කරයි.
    saved.highlightText(record.getPasscode());
    saved.exec();
    resetFlow();
    stackedWidget->setCurrentIndex(0);
}

// පවතින booking එකේ වෙනස්කම් save කර අදාළ පිටුවට ආපසු යයි.
void MainWindow::updateData()
{
    if (currentRecord.getId() < 0 || (!isPasscodeBypassed && !checkPasscode()) || !validateForm())
        return;
    StudentRecord updated = readFormRecord();
    // Student කළ edit එකකට නැවත Pending යොදයි; admin edit එකේ status එක තබයි.
    AllocationRules::prepareUpdatedAllocation(updated, isStudentEditFlow);
    if (!dbManager->updateRecord(updated)) {
        showDatabaseError();
        return;
    }
    // Title/message සහ OK/Cancel සකසයි; message වර්ගයට ගැළපෙන පාට තෝරයි.
    CustomAlertDialog("Allocation updated",
                      isStudentEditFlow ? "Your changes have been saved and are pending approval."
                                        : "Allocation changes saved.",
                      CustomAlertDialog::Info, this)
        .exec();
    if (isAdminEditMode) {
        currentRecord = StudentRecord();
        isAdminEditMode = false;
        clearUI();
        loadAdminData();
        stackedWidget->setCurrentIndex(4);
    } else {
        resetFlow();
        stackedWidget->setCurrentIndex(0);
    }
    updateCalendarHighlights();
}

// User තහවුරු කළ පසු දැනට edit කරන booking එක මකා දමයි.
void MainWindow::deleteData()
{
    if (currentRecord.getId() < 0 || (!isPasscodeBypassed && !checkPasscode()))
        return;
    if (CustomAlertDialog("Delete allocation", "Permanently delete this allocation?",
                          CustomAlertDialog::Question, this)
            .exec() != QDialog::Accepted)
        return;
    if (!dbManager->deleteRecord(currentRecord.getId())) {
        showDatabaseError();
        return;
    }
    if (isAdminEditMode) {
        currentRecord = StudentRecord();
        isAdminEditMode = false;
        clearUI();
        loadAdminData();
        stackedWidget->setCurrentIndex(4);
    } else {
        resetFlow();
        stackedWidget->setCurrentIndex(0);
    }
    updateCalendarHighlights();
}

// ඇතුළත් කළ allocation key එක මෙම booking එකේ key එකදැයි බලයි.
bool MainWindow::checkPasscode()
{
    CustomInputDialog dialog("Authentication", "Enter your allocation key:", this);
    return dialog.exec() == QDialog::Accepted &&
           dialog.getInput().compare(currentRecord.getPasscode(), Qt::CaseInsensitive) == 0;
}


// Workshop admin passcode එක පරීක්ෂා කර dashboard එක විවෘත කරයි.
void MainWindow::showRecordsWindow()
{
    CustomInputDialog authentication(
        "Administrator sign in", "Enter the passcode for " + workshopTitle(currentCategory) + ":",
        this);
    if (authentication.exec() != QDialog::Accepted)
        return;
    if (authentication.getInput() != QSettings(QSettings::defaultFormat(), QSettings::UserScope,
                                               "StudentWorkshopApp", "Passcodes")
                                         // Save කළ admin passcode නැත්නම් මුල් passcode එක 0000000 වේ.
                                         .value(currentCategory + "_passcode", "0000000")
                                         .toString()) {
        // Title/message සහ OK/Cancel සකසයි; message වර්ගයට ගැළපෙන පාට තෝරයි.
        CustomAlertDialog("Access denied", "Incorrect administrator passcode.",
                          CustomAlertDialog::Error, this)
            .exec();
        return;
    }
    adminFilterDate = QDate();
    loadAdminData();
    stackedWidget->setCurrentIndex(4);
}

// Admin තෝරාගත් දිනයේ bookings පමණක් table එකට ගනී.
void MainWindow::adminFilterByDate(const QDate &date)
{
    if (adminFilterDate == date)
        return;
    adminFilterDate = date;
    loadAdminData();
}

// Date filter එක ඉවත් කර සියලු bookings පෙන්වයි.
void MainWindow::adminClearFilter()
{
    adminFilterDate = QDate();
    const QSignalBlocker blocker(adminUi->adminCalendar);
    adminUi->adminCalendar->setSelectedDate(QDate::currentDate());
    loadAdminData();
}

// Master passcode එක පරීක්ෂා කර system settings dialog එක විවෘත කරයි.
void MainWindow::openSystemSettings()
{
    CustomInputDialog authentication("System settings", "Enter the master passcode:", this);
    if (authentication.exec() != QDialog::Accepted)
        return;
    if (authentication.getInput() != QSettings(QSettings::defaultFormat(), QSettings::UserScope,
                                               "StudentWorkshopApp", "Passcodes")
                                         // Save කළ master passcode නැත්නම් මුල් passcode එක 0000 වේ.
                                         .value("master_passcode", "0000")
                                         .toString()) {
        // Title/message සහ OK/Cancel සකසයි; message වර්ගයට ගැළපෙන පාට තෝරයි.
        CustomAlertDialog("Access denied", "Incorrect master passcode.", CustomAlertDialog::Error,
                          this)
            .exec();
        return;
    }
    SystemSettingsDialog dialog(this);
    // Settings dialog එකේ theme signal ලැබුණොත් pages නැවත පාට කරයි.
    connect(&dialog, &SystemSettingsDialog::themeChanged, this, [this] {
        applyTheme(QSettings(QSettings::defaultFormat(), QSettings::UserScope, "StudentWorkshopApp",
                             "ThemeSettings")
                       .value("darkMode", false)
                       .toBool());
    });
    // Wallpaper signal එකේ path save කර theme සමඟ නැවත යොදයි.
    connect(&dialog, &SystemSettingsDialog::wallpaperChanged, this, [this](const QString &path) {
        QSettings settings(QSettings::defaultFormat(), QSettings::UserScope, "StudentWorkshopApp",
                           "ThemeSettings");
        settings.setValue("wallpaper", path);
        settings.sync();
        if (settings.status() != QSettings::NoError) {
            // Title/message සහ OK/Cancel සකසයි; message වර්ගයට ගැළපෙන පාට තෝරයි.
            CustomAlertDialog("Settings not saved", "Unable to save the background setting.",
                              CustomAlertDialog::Error, this)
                .exec();
            return;
        }
        applyTheme(settings.value("darkMode", false).toBool());
    });
    dialog.exec();
}

// Admin email settings dialog එක විවෘත කරයි.
void MainWindow::openAdminEmailSettings()
{
    AdminSettingsDialog dialog(this);
    dialog.exec();
}

// Table row එකක් තෝරා ඇති විට පමණක් Edit/Delete/Email buttons ක්‍රියාත්මක කරයි.
void MainWindow::refreshAdminActions()
{
    const bool selected =
        adminUi->mainTable->currentRow() >= 0 && !adminUi->mainTable->selectedItems().isEmpty();
    for (QPushButton *button :
         {adminUi->adminEditBtn, adminUi->adminDelBtn, adminUi->pendingEmailBtn, adminUi->rejectEmailBtn})
        button->setEnabled(selected);
}

// Database bookings table rows වලට දමා editable remarks සහ status controls සකසයි.
void MainWindow::loadAdminData()
{
    const QList<StudentRecord> records =
        adminFilterDate.isValid() ? dbManager->getRecordsByDateAndCategory(
                                        adminFilterDate.toString(Qt::ISODate), currentCategory)
                                  : dbManager->getAllRecordsForCategory(currentCategory);
    if (!dbManager->getLastError().isEmpty()) {
        showDatabaseError();
        return;
    }
    // Table එක පුරවන අතර selection signals තාවකාලිකව නවතයි.
    const QSignalBlocker tableBlocker(adminUi->mainTable);
    adminUi->mainTable->setRowCount(0);
    for (int row = 0; row < records.size(); ++row) {
        const StudentRecord record = records[row];
        adminUi->mainTable->insertRow(row);
        // මෙම values අනුපිළිවෙළ admin.ui හි columns සහ widths ලැයිස්තුවට ගළපන්න.
        const QStringList values = {record.getPasscode(),   record.getCreatedAt(),
                                    record.getIndexNo(),    record.getName(),
                                    record.getDepartment(), record.getEmail(),
                                    record.getRecordDate(), record.getTimeSlots(),
                                    record.getDescription(), record.getAdminRemarks(),
                                    record.getStatus()};
        for (int column = 0; column < values.size(); ++column) {
            auto *item = new QTableWidgetItem(values[column]);
            item->setToolTip(values[column]);
            adminUi->mainTable->setItem(row, column, item);
        }
        // පළමු cell එකේ නොපෙනෙන UserRole දත්ත ලෙස සැබෑ database ID එක තබයි.
        adminUi->mainTable->item(row, 0)->setData(Qt::UserRole, record.getId());

        // Remarks cell එකේ typing අවසන් කළ විට නව remark එක save කරයි.
        auto *remarks = new QLineEdit(record.getAdminRemarks());
        remarks->setMaxLength(1000);
        remarks->setPlaceholderText("Add a remark");
        remarks->setProperty("savedText", record.getAdminRemarks());
        connect(remarks, &QLineEdit::editingFinished, this, [this, remarks, id = record.getId()] {
            if (remarks->text() == remarks->property("savedText").toString())
                return;
            if (!dbManager->updateAdminRemarks(id, remarks->text())) {
                remarks->setText(remarks->property("savedText").toString());
                showDatabaseError();
            } else {
                remarks->setProperty("savedText", remarks->text());
            }
        });
        adminUi->mainTable->setCellWidget(row, 9, remarks);

        // පෙන්වන status label සහ database status value වෙන වෙනම මෙහි ඇත.
        auto *status = new QComboBox();
        status->addItem("Pending", "Pending");
        status->addItem("Approved", "Approve");
        status->addItem("Rejected", "Reject");
        // Save කළ status එක තෝරා error එකකදී ආපසු යෙදීමට savedIndex මතක තබයි.
        status->setCurrentIndex(qMax(0, status->findData(record.getStatus())));
        status->setProperty("savedIndex", status->currentIndex());
        connect(status, &QComboBox::currentIndexChanged, this,
                [this, status, id = record.getId()](int) {
                    if (!dbManager->updateRecordStatus(id, status->currentData().toString())) {
                        const QSignalBlocker blocker(status);
                        status->setCurrentIndex(status->property("savedIndex").toInt());
                        showDatabaseError();
                        return;
                    }
                    status->setProperty("savedIndex", status->currentIndex());
                    updateCalendarHighlights();
                    updateAdminCalendarHighlights();
                });
        adminUi->mainTable->setCellWidget(row, 10, status);
    }
    adminUi->adminWsDescLbl->setText(
        workshopDepartment(currentCategory) + "\n" + QString::number(records.size()) +
        (records.size() == 1 ? " allocation" : " allocations") +
        (adminFilterDate.isValid() ? " on " + adminFilterDate.toString("d MMM yyyy")
                                   : " · All dates"));
    adminUi->adminShowAllBtn->setEnabled(adminFilterDate.isValid());
    refreshAdminActions();
    updateAdminCalendarHighlights();
}

// Status එක save කර student email draft එක විවෘත කරයි. Subject/body පහත වෙනස් කරන්න.
void MainWindow::composeStatusEmail(const QString &status)
{
    const int row = adminUi->mainTable->currentRow();
    if (row < 0)
        return;
    const int id = adminUi->mainTable->item(row, 0)->data(Qt::UserRole).toInt();
    const auto records = dbManager->getAllRecordsForCategory(currentCategory);
    if (!dbManager->getLastError().isEmpty()) {
        showDatabaseError();
        return;
    }
    for (const auto &record : records) {
        if (record.getId() != id)
            continue;
        if (!dbManager->updateRecordStatus(id, status)) {
            showDatabaseError();
            return;
        }
        // Email එක auto-send නොකර mail application එකේ draft එකක් පමණක් විවෘත කරයි.
        QUrl url("mailto:" + record.getEmail());
        QUrlQuery query;
        const bool pending = status == "Pending";
        // Email subject text එක මෙහි වෙනස් කරන්න.
        query.addQueryItem("subject", "Workshop allocation " +
                                          QString(pending ? "pending" : "rejected") + ": " +
                                          workshopTitle(currentCategory));
        // Student ට යන email body එක මෙහි වෙනස් කරන්න; booking values අගට එක් වේ.
        query.addQueryItem("body",
                           "Dear " + record.getName() + ",\n\nYour allocation for " +
                               workshopTitle(currentCategory) + " on " + record.getRecordDate() +
                               " is " + (pending ? "pending approval." : "rejected.") + "\nTime: " +
                               record.getTimeSlots() + "\nAllocation key: " + record.getPasscode() +
                               "\nRemarks: " + record.getAdminRemarks());
        if (pending) {
            QSettings settings(QSettings::defaultFormat(), QSettings::UserScope,
                               "StudentWorkshopApp", "AdminSettings");
            QStringList recipients;
            // dean_email යනු පැරණි save key එකයි; දැන් එහි Lab/Workshop In-charge email එක තබයි.
            for (const QString &key : {QString("dean_email"), QString("dep_head_email")}) {
                const QString email = settings.value(key).toString();
                if (!email.isEmpty())
                    recipients.append(email);
            }
            if (!recipients.isEmpty())
                query.addQueryItem("cc", recipients.join(','));
        }
        url.setQuery(query);
        loadAdminData();
        updateCalendarHighlights();
        if (!QDesktopServices::openUrl(url))
            // Title/message සහ OK/Cancel සකසයි; message වර්ගයට ගැළපෙන පාට තෝරයි.
            CustomAlertDialog(
                "Email draft unavailable",
                "The allocation status was saved, but no email application could open the draft.",
                CustomAlertDialog::Warning, this)
                .exec();
        else
            statusBar()->showMessage(
                "Email draft opened. Review and send it in your email application.", 10000);
        return;
    }
}

