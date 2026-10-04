// MainWindow is the controller of the program.
// Everything the window does lives in this one file, grouped into sections:
//   1. Starting the program  (constructor, destructor, first setup)
//   2. Appearance            (theme and workshop card state)
//   3. Moving between pages
//   4. Date and time page
//   5. Allocation form page
//   6. Administrator page    (table, email drafts, settings)
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

namespace {
// Preserve Designer coordinates as the baseline, expanding the authored
// containers and controls to use the available window space. Small windows
// retain readable control sizes and scroll the baseline canvas.
class DesignerPageScroll final : public QScrollArea
{
public:
    DesignerPageScroll(QWidget *parent, QWidget *canvas)
        : QScrollArea(parent), baseSize(canvas->size())
    {
        capture(canvas);
        setFrameShape(QFrame::NoFrame);
        setWidgetResizable(false);
        setAlignment(Qt::AlignLeft | Qt::AlignTop);
        setWidget(canvas);
    }

protected:
    void resizeEvent(QResizeEvent *event) override
    {
        QScrollArea::resizeEvent(event);
        const QSize target = maximumViewportSize().expandedTo(baseSize);
        const double sx = double(target.width()) / baseSize.width();
        const double sy = double(target.height()) / baseSize.height();
        widget()->resize(target);
        for (const auto &item : items) {
            const QRect &r = item.geometry;
            item.widget->setGeometry(qRound(r.x() * sx), qRound(r.y() * sy),
                                     qRound(r.width() * sx), qRound(r.height() * sy));
        }
    }

private:
    struct Item { QWidget *widget; QRect geometry; };
    QSize baseSize;
    QList<Item> items;

    void capture(QWidget *container)
    {
        for (QWidget *child : container->findChildren<QWidget *>(QString(), Qt::FindDirectChildrenOnly)) {
            items.append({child, child->geometry()});
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

void addDesignerPage(QStackedWidget *stack, QWidget *canvas, int index)
{
    auto *scroll = new DesignerPageScroll(stack, canvas);
    scroll->setObjectName("designerPageScroll" + QString::number(index));
    stack->addWidget(scroll);
}
}

// =====================================================================
// 1. STARTING THE PROGRAM
// =====================================================================

// Constructor
MainWindow::MainWindow(QWidget *parent, const QString &databasePath)
    : QMainWindow(parent), welcomeUi(new Ui::welcomeWidget), selectionUi(new Ui::selectionWidget),
      calanderUi(new Ui::calanderWidget), formUi(new Ui::formWidget), adminUi(new Ui::adminWidget)
{
    // Create the page host directly; only the actual pages need .ui files.
    setObjectName("MainWindow");
    resize(1180, 780);
    setWindowTitle(tr("SYNCLAB | Centralized Workshop & Laboratory Allocation Platform"));
    setWindowIcon(QIcon(":/branding/logo.png"));
    setFont(QFont("Segoe UI", 10));
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
    stackedWidget = new QStackedWidget(this);
    stackedWidget->setObjectName("stackedWidget");
    setCentralWidget(stackedWidget);
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

    // Open the SQLite database and make sure the table exists.
    const QString path = databasePath.isEmpty() ? defaultDatabasePath() : databasePath;
    dbManager = new SqliteDatabaseManager(path);
    databaseReady = dbManager->createTable();

    calanderUi->backTo1Btn->setProperty("_designerBackText", calanderUi->backTo1Btn->text());
    formUi->confirmTitleLbl->setProperty("_designerNewTitle", formUi->confirmTitleLbl->text());
    bindWorkshopCards();
    setupDynamicUI();
    applyTheme(QSettings(QSettings::defaultFormat(), QSettings::UserScope, "StudentWorkshopApp",
                         "ThemeSettings")
                   .value("darkMode", false)
                   .toBool());
    stackedWidget->setCurrentIndex(0);

    // Connect every button to the function that should run when it is clicked.
    connect(welcomeUi->allocateNowBtn, &QPushButton::clicked, this, &MainWindow::startNewAllocation);
    connect(welcomeUi->editAllocationBtn, &QPushButton::clicked, this, &MainWindow::startEditAllocationFlow);
    connect(welcomeUi->settingsBtn0, &QPushButton::clicked, this, &MainWindow::openSystemSettings);
    connect(selectionUi->backTo0Btn, &QPushButton::clicked, this, &MainWindow::cancelCurrentFlow);
    connect(selectionUi->nextBtn, &QPushButton::clicked, this, &MainWindow::onNextClicked);
    connect(calanderUi->backTo1Btn, &QPushButton::clicked, this, &MainWindow::goBackTo1);
    connect(calanderUi->nextTo3Btn, &QPushButton::clicked, this, &MainWindow::onNextTo3Clicked);
    connect(formUi->backTo2Btn, &QPushButton::clicked, this, &MainWindow::goBackTo2);
    connect(calanderUi->calendar, &QCalendarWidget::selectionChanged, this,
            &MainWindow::onCalendarSelectionChanged);
    connect(formUi->submitBtn, &QPushButton::clicked, this, &MainWindow::submitData);
    connect(formUi->updateBtn, &QPushButton::clicked, this, &MainWindow::updateData);
    connect(formUi->deleteBtn, &QPushButton::clicked, this, &MainWindow::deleteData);
    connect(calanderUi->adminBtn2, &QPushButton::clicked, this, &MainWindow::showRecordsWindow);
    connect(adminUi->adminBackBtn, &QPushButton::clicked, this, &MainWindow::goBackFromAdmin);
    connect(adminUi->adminSettingsBtn, &QPushButton::clicked, this,
            &MainWindow::openAdminEmailSettings);
    connect(adminUi->adminShowAllBtn, &QPushButton::clicked, this, &MainWindow::adminClearFilter);
    connect(adminUi->adminCalendar, &QCalendarWidget::selectionChanged, this,
            [this] { adminFilterByDate(adminUi->adminCalendar->selectedDate()); });
    connect(adminUi->adminCalendar, &QCalendarWidget::clicked, this, &MainWindow::adminFilterByDate);
    connect(adminUi->mainTable, &QTableWidget::itemSelectionChanged, this,
            &MainWindow::refreshAdminActions);

    // Administrator: open the selected allocation in the form.
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

    // Administrator: delete the selected allocation.
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

    connect(adminUi->pendingEmailBtn, &QPushButton::clicked, this,
            [this] { composeStatusEmail("Pending"); });
    connect(adminUi->rejectEmailBtn, &QPushButton::clicked, this,
            [this] { composeStatusEmail("Reject"); });

    // If the database could not be opened, allocation must be switched off.
    if (!databaseReady) {
        welcomeUi->allocateNowBtn->setEnabled(false);
        welcomeUi->editAllocationBtn->setEnabled(false);
        QTimer::singleShot(0, this, &MainWindow::showDatabaseError);
    }
}

// Destructor
MainWindow::~MainWindow()
{
    delete dbManager;
    delete welcomeUi;
    delete selectionUi;
    delete calanderUi;
    delete formUi;
    delete adminUi;
}

// Connects dynamic behavior and configures input limits. Static text, fonts,
// pictures, widget sizes and positions are stored in the Designer forms.
void MainWindow::setupDynamicUI()
{
    // Let the page wallpaper show through the space around the workshop cards.
    selectionUi->cardsScrollArea->viewport()->setAutoFillBackground(false);
    selectionUi->cardsScrollContents->setAutoFillBackground(false);

    // Remember the booking values associated with the four authored buttons.
    timeSlotValues = {"8.30 am - 10.30 am", "10.30 am - 12.30 pm", "1.30 pm - 3.30 pm",
                      "3.30 pm - 5.30 pm"};
    timeSlotButtons = {calanderUi->timeBtn1, calanderUi->timeBtn2, calanderUi->timeBtn3, calanderUi->timeBtn4};
    for (int i = 0; i < timeSlotButtons.size(); ++i) {
        // The stylesheet uses [timeSlot=true] to recognise these buttons.
        timeSlotButtons[i]->setProperty("timeSlot", true);
        connect(timeSlotButtons[i], &QPushButton::toggled, this, &MainWindow::onTimeSlotClicked);
    }

    calanderUi->calendar->setMinimumDate(QDate::currentDate());
    calanderUi->calendar->setVerticalHeaderFormat(QCalendarWidget::NoVerticalHeader);
    adminUi->adminCalendar->setVerticalHeaderFormat(QCalendarWidget::NoVerticalHeader);

    // The administrator table has eleven columns.
    adminUi->mainTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
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

// Shows the last database error message in a dialog box.
void MainWindow::showDatabaseError()
{
    const QString error = dbManager->getLastError();
    CustomAlertDialog("Could not save or load allocation",
                      error.isEmpty() ? "The database is unavailable. Please try again." : error,
                      CustomAlertDialog::Error, this)
        .exec();
}

// =====================================================================
// 2. APPEARANCE: THEME AND WORKSHOP CARD STATE
// =====================================================================

// Applies the light or dark colour scheme to the whole window.
void MainWindow::applyTheme(bool darkMode)
{
    isDarkTheme = darkMode;
    // Designer owns the page controls' normal style, font, geometry and static text.
    // The theme helper only changes recognised palette colours for dark mode.
    applyDesignerTheme(this, darkMode);

    // If the user chose a background picture, draw it behind each page.
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

// Connect the cards stored in selection.ui. Their position, size, pictures and
// labels are authored in Designer and are never reconstructed at runtime.
void MainWindow::bindWorkshopCards()
{
    for (QFrame *card : selectionUi->cardsScrollContents->findChildren<QFrame *>()) {
        const QString category = card->property("category").toString();
        if (category.isEmpty())
            continue;
        cardFrames.append(card);
        auto *button = card->findChild<QPushButton *>();
        if (button)
            connect(button, &QPushButton::clicked, this, [this, category] { selectCard(category); });
    }
}

// A dynamic state selects a Designer-authored CSS rule, without replacing it.
void MainWindow::selectCard(const QString &category)
{
    selectedCategory = category;
    for (QFrame *card : cardFrames) {
        const bool selected = card->property("category").toString() == category;
        card->setProperty("selected", selected);
        card->style()->unpolish(card);
        card->style()->polish(card);
        card->update();
        if (auto *button = card->findChild<QPushButton *>())
            button->setAccessibleDescription(selected ? "Selected workshop" : "Select workshop");
    }
    selectionUi->nextBtn->setEnabled(!category.isEmpty());
}

// =====================================================================
// 3. MOVING BETWEEN PAGES
// =====================================================================

// Puts every variable and widget back to its starting value.
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

// "Allocate now" on the welcome page.
void MainWindow::startNewAllocation()
{
    if (!databaseReady)
        return;
    resetFlow();
    stackedWidget->setCurrentIndex(1);
}

// "Edit allocation" on the welcome page. The student types the allocation key.
void MainWindow::startEditAllocationFlow()
{
    if (!databaseReady)
        return;
    CustomInputDialog dialog("Edit allocation", "Enter your allocation key:", this);
    if (dialog.exec() != QDialog::Accepted)
        return;
    const QString entered = dialog.getInput();

    // Search every workshop for that key.
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

    // The same key can exist in two workshops, so ask for the index number too.
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
    // Historical allocations must still expose the delete action and editable details.
    stackedWidget->setCurrentIndex(savedDate < QDate::currentDate() ? 3 : 1);
}

// Leaving a allocation that has not been saved yet.
void MainWindow::cancelCurrentFlow()
{
    const bool hasDraft =
        isStudentEditFlow || isAdminEditMode || !formUi->nameInput->text().isEmpty() ||
        !formUi->indexInput->text().isEmpty() || !formUi->emailInput->text().isEmpty() ||
        !formUi->supervisorInput->text().isEmpty() || !formUi->projectInput->text().isEmpty() ||
        !formUi->descriptionInput->toPlainText().isEmpty();
    if (hasDraft &&
        CustomAlertDialog("Discard changes", "Leave this allocation and discard unsaved changes?",
                          CustomAlertDialog::Question, this)
                .exec() != QDialog::Accepted)
        return;
    resetFlow();
    stackedWidget->setCurrentIndex(0);
}

// "Next" on the workshop selection page.
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

// Fills page 2 with the chosen workshop and shows it.
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
    // Fit each selected-workshop picture inside its Designer-authored label.
    calanderUi->selectedImgLabel2->setPixmap(
        workshopImage(category, QSize(160, 100).boundedTo(calanderUi->selectedImgLabel2->size())));
    adminUi->adminImgLabel->setPixmap(
        workshopImage(category, QSize(160, 100).boundedTo(adminUi->adminImgLabel->size())));
    updateCalendarHighlights();
    onCalendarSelectionChanged();
    stackedWidget->setCurrentIndex(2);
}

// "Next" on the date and time page.
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

// "Back" on the date and time page.
void MainWindow::goBackTo1()
{
    if (isAdminEditMode) {
        // The administrator came from the records table, so send them back there.
        isAdminEditMode = false;
        currentRecord = StudentRecord();
        clearUI();
        loadAdminData();
        stackedWidget->setCurrentIndex(4);
    } else {
        stackedWidget->setCurrentIndex(1);
    }
}

// "Back" on the allocation form page.
void MainWindow::goBackTo2()
{
    stackedWidget->setCurrentIndex(2);
}

// "Back" on the administrator page.
void MainWindow::goBackFromAdmin()
{
    resetFlow();
    stackedWidget->setCurrentIndex(0);
}

// =====================================================================
// 4. DATE AND TIME PAGE
// =====================================================================

// Returns the text of every time slot button that is ticked.
QStringList MainWindow::getSelectedTimeSlotsList()
{
    QStringList selectedSlots;
    for (int i = 0; i < timeSlotButtons.size(); ++i) {
        if (timeSlotButtons[i]->isChecked() && timeSlotButtons[i]->isEnabled())
            selectedSlots.append(timeSlotValues[i]);
    }
    return selectedSlots;
}

// Ticks the buttons that belong to a saved allocation.
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

// The Next button only works when at least one time slot is chosen.
void MainWindow::onTimeSlotClicked()
{
    calanderUi->nextTo3Btn->setEnabled(!getSelectedTimeSlotsList().isEmpty());
}

// Runs every time the student picks a different date.
// Grey slots are the ones already allocated by somebody else.
void MainWindow::onCalendarSelectionChanged()
{
    if (currentCategory.isEmpty()) {
        calanderUi->nextTo3Btn->setEnabled(false);
        return;
    }
    const QDate date = calanderUi->calendar->selectedDate();
    // The student's own allocation is excluded, so editing it does not block itself.
    const auto allocated = dbManager->getAllocatedTimeSlotsStringList(
        date.toString(Qt::ISODate), currentCategory, currentRecord.getId());
    const bool queryOk = dbManager->getLastError().isEmpty();
    for (int i = 0; i < timeSlotButtons.size(); ++i) {
        QPushButton *button = timeSlotButtons[i];
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

// Draws the small A / P / R badges on the allocation calendar.
void MainWindow::updateCalendarHighlights()
{
    if (!databaseReady || currentCategory.isEmpty())
        return;
    calanderUi->calendar->setDateTextFormat(QDate(), QTextCharFormat());
    // Count how many approved, pending and rejected allocations fall on each date.
    QMap<QDate, QMap<QString, int>> counts;
    for (const auto &record : dbManager->getAllRecordsForCategory(currentCategory)) {
        const QDate date = QDate::fromString(record.getRecordDate(), Qt::ISODate);
        if (date.isValid())
            counts[date][record.getStatus()]++;
    }
    calanderUi->calendar->setStatusCounts(counts);
}

// The same badges, but on the administrator's calendar.
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

// =====================================================================
// 5. ALLOCATION FORM PAGE
// =====================================================================

// Empties every field on the form.
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

// Shows either "Submit" (new allocation) or "Update" and "Delete" (existing one).
void MainWindow::setFormButtonsMode(bool edit)
{
    formUi->submitBtn->setVisible(!edit);
    formUi->updateBtn->setVisible(edit);
    formUi->deleteBtn->setVisible(edit);
    formUi->confirmTitleLbl->setText(edit ? "Edit your allocation" : formUi->confirmTitleLbl->property("_designerNewTitle").toString());
}

// Puts a saved allocation back onto the pages so it can be read or changed.
void MainWindow::loadRecordIntoUI(const StudentRecord &record)
{
    currentRecord = record;
    const QDate date = QDate::fromString(record.getRecordDate(), Qt::ISODate);
    // Old allocations remain viewable/deletable, but resubmission requires a current date.
    calanderUi->calendar->setMinimumDate(
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

// The administrator's "Edit" button uses the same form as the student.
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

// Reads what is on screen into a StudentRecord object.
StudentRecord MainWindow::readFormRecord()
{
    StudentRecord record = currentRecord;
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

// Checks the form. If something is wrong it shows a message and moves the
// cursor to the field that needs attention.
bool MainWindow::validateForm()
{
    const AllocationRules::Field field =
        AllocationRules::validate(readFormRecord(), formUi->deptInput->currentIndex() > 0);
    if (field == AllocationRules::Field::None) {
        return true;
    }
    CustomAlertDialog("Check allocation details", AllocationRules::validationMessage(field),
                      CustomAlertDialog::Warning, this)
        .exec();
    // Only the UI controller is responsible for keyboard focus and page changes.
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

// "Submit" on the allocation form: INSERT a new record.
void MainWindow::submitData()
{
    if (currentRecord.getId() >= 0 || !validateForm())
        return;
    StudentRecord record = readFormRecord();
    // Gives the allocation its 4-digit allocation key, a "Pending" status and a
    // creation timestamp.
    AllocationRules::prepareNewAllocation(record);
    if (!dbManager->addRecord(record)) {
        showDatabaseError();
        return;
    }
    // The allocation key is the one thing the student must remember, so the
    // dialog draws it as a highlighted badge instead of leaving it inside a
    // plain line of text.
    CustomAlertDialog saved("Allocation saved",
                            "Your allocation is pending approval.\n\nAllocation key: " +
                                record.getPasscode() +
                                "\n\nKeep your allocation key to edit this allocation.\n" +
                                workshopTitle(currentCategory) + "\n" + record.getRecordDate() +
                                "\n" + record.getTimeSlots(),
                            CustomAlertDialog::Info, this);
    saved.highlightText(record.getPasscode());
    saved.exec();
    resetFlow();
    stackedWidget->setCurrentIndex(0);
}

// "Update" on the allocation form: UPDATE the existing record.
void MainWindow::updateData()
{
    if (currentRecord.getId() < 0 || (!isPasscodeBypassed && !checkPasscode()) || !validateForm())
        return;
    StudentRecord updated = readFormRecord();
    AllocationRules::prepareUpdatedAllocation(updated, isStudentEditFlow);
    if (!dbManager->updateRecord(updated)) {
        showDatabaseError();
        return;
    }
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

// "Delete" on the allocation form: DELETE the record after a yes/no question.
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

// Asks for the allocation key before a change is allowed.
bool MainWindow::checkPasscode()
{
    CustomInputDialog dialog("Authentication", "Enter your allocation key:", this);
    return dialog.exec() == QDialog::Accepted &&
           dialog.getInput().compare(currentRecord.getPasscode(), Qt::CaseInsensitive) == 0;
}

// =====================================================================
// 6. ADMINISTRATOR PAGE
// =====================================================================

// "Administrator" button: check the passcode, then show the records page.
void MainWindow::showRecordsWindow()
{
    CustomInputDialog authentication(
        "Administrator sign in", "Enter the passcode for " + workshopTitle(currentCategory) + ":",
        this);
    if (authentication.exec() != QDialog::Accepted)
        return;
    if (authentication.getInput() != QSettings(QSettings::defaultFormat(), QSettings::UserScope,
                                               "StudentWorkshopApp", "Passcodes")
                                         .value(currentCategory + "_passcode", "0000000")
                                         .toString()) {
        CustomAlertDialog("Access denied", "Incorrect administrator passcode.",
                          CustomAlertDialog::Error, this)
            .exec();
        return;
    }
    adminFilterDate = QDate();
    loadAdminData();
    stackedWidget->setCurrentIndex(4);
}

// The administrator clicked a date on their calendar.
void MainWindow::adminFilterByDate(const QDate &date)
{
    if (adminFilterDate == date)
        return;
    adminFilterDate = date;
    loadAdminData();
}

// "Show all" removes the date filter.
void MainWindow::adminClearFilter()
{
    adminFilterDate = QDate();
    const QSignalBlocker blocker(adminUi->adminCalendar);
    adminUi->adminCalendar->setSelectedDate(QDate::currentDate());
    loadAdminData();
}

// The master passcode opens the theme and background settings.
void MainWindow::openSystemSettings()
{
    CustomInputDialog authentication("System settings", "Enter the master passcode:", this);
    if (authentication.exec() != QDialog::Accepted)
        return;
    if (authentication.getInput() != QSettings(QSettings::defaultFormat(), QSettings::UserScope,
                                               "StudentWorkshopApp", "Passcodes")
                                         .value("master_passcode", "0000")
                                         .toString()) {
        CustomAlertDialog("Access denied", "Incorrect master passcode.", CustomAlertDialog::Error,
                          this)
            .exec();
        return;
    }
    SystemSettingsDialog dialog(this);
    connect(&dialog, &SystemSettingsDialog::themeChanged, this, [this] {
        applyTheme(QSettings(QSettings::defaultFormat(), QSettings::UserScope, "StudentWorkshopApp",
                             "ThemeSettings")
                       .value("darkMode", false)
                       .toBool());
    });
    connect(&dialog, &SystemSettingsDialog::wallpaperChanged, this, [this](const QString &path) {
        QSettings settings(QSettings::defaultFormat(), QSettings::UserScope, "StudentWorkshopApp",
                           "ThemeSettings");
        settings.setValue("wallpaper", path);
        settings.sync();
        if (settings.status() != QSettings::NoError) {
            CustomAlertDialog("Settings not saved", "Unable to save the background setting.",
                              CustomAlertDialog::Error, this)
                .exec();
            return;
        }
        applyTheme(settings.value("darkMode", false).toBool());
    });
    dialog.exec();
}

// The administrator's email settings dialog.
void MainWindow::openAdminEmailSettings()
{
    AdminSettingsDialog dialog(this);
    dialog.exec();
}

// Edit, delete and email buttons only work when a row is selected.
void MainWindow::refreshAdminActions()
{
    const bool selected =
        adminUi->mainTable->currentRow() >= 0 && !adminUi->mainTable->selectedItems().isEmpty();
    for (QPushButton *button :
         {adminUi->adminEditBtn, adminUi->adminDelBtn, adminUi->pendingEmailBtn, adminUi->rejectEmailBtn})
        button->setEnabled(selected);
}

// SELECTs the allocations and fills the table with them.
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
    // Blocking signals stops the table telling us the selection changed while
    // we are still filling it in.
    const QSignalBlocker tableBlocker(adminUi->mainTable);
    adminUi->mainTable->setRowCount(0);
    for (int row = 0; row < records.size(); ++row) {
        const StudentRecord record = records[row];
        adminUi->mainTable->insertRow(row);
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
        // The real database id is hidden inside the first cell.
        adminUi->mainTable->item(row, 0)->setData(Qt::UserRole, record.getId());

        // Remarks can be typed straight into the table.
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

        // The status drop-down writes to the database as soon as it changes.
        auto *status = new QComboBox();
        status->addItem("Pending", "Pending");
        status->addItem("Approved", "Approve");
        status->addItem("Rejected", "Reject");
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

// Saves the new status and opens an email draft telling the student about it.
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
        // Build a mailto: link. The email program opens with the text ready.
        QUrl url("mailto:" + record.getEmail());
        QUrlQuery query;
        const bool pending = status == "Pending";
        query.addQueryItem("subject", "Workshop allocation " +
                                          QString(pending ? "pending" : "rejected") + ": " +
                                          workshopTitle(currentCategory));
        query.addQueryItem("body",
                           "Dear " + record.getName() + ",\n\nYour allocation for " +
                               workshopTitle(currentCategory) + " on " + record.getRecordDate() +
                               " is " + (pending ? "pending approval." : "rejected.") + "\nTime: " +
                               record.getTimeSlots() + "\nAllocation key: " + record.getPasscode() +
                               "\nRemarks: " + record.getAdminRemarks());
        if (pending) {
            // Copy in the lab/workshop in-charge and the head of department if their addresses
            // were saved in the administrator settings.
            QSettings settings(QSettings::defaultFormat(), QSettings::UserScope,
                               "StudentWorkshopApp", "AdminSettings");
            QStringList recipients;
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

