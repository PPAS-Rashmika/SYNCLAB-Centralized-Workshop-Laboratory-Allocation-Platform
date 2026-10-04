// MainWindow is the controller of the program.
// Everything the window does lives in this one file, grouped into sections:
//   1. Starting the program  (constructor, destructor, first setup)
//   2. Appearance            (theme and workshop card state)
//   3. Moving between pages
//   4. Date and time page
//   5. Allocation form page
//   6. Administrator page    (table, email drafts, printing, settings)
#include "mainwindow.h"
#include "designertheme.h"
#include "ui_mainwindow.h"
#include "ui_page0.h"
#include "ui_page1.h"
#include "ui_page2.h"
#include "ui_page3.h"
#include "ui_page4.h"
#include "customdialogs.h"
#include "workshopcatalog.h"
#include "filestorage.h"
#include "allocationrules.h"
#include <QComboBox>
#include <QDesktopServices>
#include <QDir>
#include <QFileInfo>
#include <QHeaderView>
#include <QLabel>
#include <QLineEdit>
#include <QMap>
#include <QPalette>
#include <QPixmap>
#include <QPrintDialog>
#include <QPrinter>
#include <QPushButton>
#include <QScrollArea>
#include <QSettings>
#include <QSignalBlocker>
#include <QStatusBar>
#include <QTableWidget>
#include <QTextCharFormat>
#include <QTextDocument>
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
    : QMainWindow(parent), ui(new Ui::MainWindow), ui0(new Ui::page0Widget), ui1(new Ui::page1Widget),
      ui2(new Ui::page2Widget), ui3(new Ui::page3Widget), ui4(new Ui::page4Widget)
{
    // Build the outer window first, then add the five pages to its stack.
    ui->setupUi(this);
    page0Widget = new QWidget();
    ui0->setupUi(page0Widget);
    addDesignerPage(ui->stackedWidget, page0Widget, 0);
    page1Widget = new QWidget();
    ui1->setupUi(page1Widget);
    addDesignerPage(ui->stackedWidget, page1Widget, 1);
    page2Widget = new QWidget();
    ui2->setupUi(page2Widget);
    addDesignerPage(ui->stackedWidget, page2Widget, 2);
    page3Widget = new QWidget();
    ui3->setupUi(page3Widget);
    addDesignerPage(ui->stackedWidget, page3Widget, 3);
    page4Widget = new QWidget();
    ui4->setupUi(page4Widget);
    addDesignerPage(ui->stackedWidget, page4Widget, 4);

    // Open the SQLite database and make sure the table exists.
    const QString path = databasePath.isEmpty() ? defaultDatabasePath() : databasePath;
    dbManager = new SqliteDatabaseManager(path);
    databaseReady = dbManager->createTable();

    ui2->backTo1Btn->setProperty("_designerBackText", ui2->backTo1Btn->text());
    ui3->confirmTitleLbl->setProperty("_designerNewTitle", ui3->confirmTitleLbl->text());
    bindWorkshopCards();
    setupDynamicUI();
    applyTheme(QSettings(QSettings::defaultFormat(), QSettings::UserScope, "StudentWorkshopApp",
                         "ThemeSettings")
                   .value("darkMode", false)
                   .toBool());
    ui->stackedWidget->setCurrentIndex(0);

    // Connect every button to the function that should run when it is clicked.
    connect(ui0->allocateNowBtn, &QPushButton::clicked, this, &MainWindow::startNewAllocation);
    connect(ui0->editAllocationBtn, &QPushButton::clicked, this, &MainWindow::startEditAllocationFlow);
    connect(ui0->settingsBtn0, &QPushButton::clicked, this, &MainWindow::openSystemSettings);
    connect(ui1->backTo0Btn, &QPushButton::clicked, this, &MainWindow::cancelCurrentFlow);
    connect(ui1->nextBtn, &QPushButton::clicked, this, &MainWindow::onNextClicked);
    connect(ui2->backTo1Btn, &QPushButton::clicked, this, &MainWindow::goBackTo1);
    connect(ui2->nextTo3Btn, &QPushButton::clicked, this, &MainWindow::onNextTo3Clicked);
    connect(ui3->backTo2Btn, &QPushButton::clicked, this, &MainWindow::goBackTo2);
    connect(ui2->calendar, &QCalendarWidget::selectionChanged, this,
            &MainWindow::onCalendarSelectionChanged);
    connect(ui3->submitBtn, &QPushButton::clicked, this, &MainWindow::submitData);
    connect(ui3->updateBtn, &QPushButton::clicked, this, &MainWindow::updateData);
    connect(ui3->deleteBtn, &QPushButton::clicked, this, &MainWindow::deleteData);
    connect(ui2->adminBtn2, &QPushButton::clicked, this, &MainWindow::showRecordsWindow);
    connect(ui4->adminBackBtn, &QPushButton::clicked, this, &MainWindow::goBackFromAdmin);
    connect(ui4->adminSettingsBtn, &QPushButton::clicked, this,
            &MainWindow::openAdminEmailSettings);
    connect(ui4->adminShowAllBtn, &QPushButton::clicked, this, &MainWindow::adminClearFilter);
    connect(ui4->adminCalendar, &QCalendarWidget::selectionChanged, this,
            [this] { adminFilterByDate(ui4->adminCalendar->selectedDate()); });
    connect(ui4->adminCalendar, &QCalendarWidget::clicked, this, &MainWindow::adminFilterByDate);
    connect(ui4->mainTable, &QTableWidget::itemSelectionChanged, this,
            &MainWindow::refreshAdminActions);

    // Administrator: open the selected allocation in the form.
    connect(ui4->adminEditBtn, &QPushButton::clicked, this, [this] {
        const int row = ui4->mainTable->currentRow();
        if (row < 0)
            return;
        const int id = ui4->mainTable->item(row, 0)->data(Qt::UserRole).toInt();
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
    connect(ui4->adminDelBtn, &QPushButton::clicked, this, [this] {
        const int row = ui4->mainTable->currentRow();
        if (row < 0)
            return;
        const int id = ui4->mainTable->item(row, 0)->data(Qt::UserRole).toInt();
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

    connect(ui4->pendingEmailBtn, &QPushButton::clicked, this,
            [this] { composeStatusEmail("Pending"); });
    connect(ui4->rejectEmailBtn, &QPushButton::clicked, this,
            [this] { composeStatusEmail("Reject"); });

    connect(ui4->printRecordsBtn, &QPushButton::clicked, this, &MainWindow::printAllRecords);

    // If the database could not be opened, allocation must be switched off.
    if (!databaseReady) {
        ui0->allocateNowBtn->setEnabled(false);
        ui0->editAllocationBtn->setEnabled(false);
        QTimer::singleShot(0, this, &MainWindow::showDatabaseError);
    }
}

// Destructor
MainWindow::~MainWindow()
{
    delete dbManager;
    delete ui;
    delete ui0;
    delete ui1;
    delete ui2;
    delete ui3;
    delete ui4;
}

// Connects dynamic behavior and configures input limits. Static text, fonts,
// pictures, widget sizes and positions are stored in the Designer forms.
void MainWindow::setupDynamicUI()
{
    // Let the page wallpaper show through the space around the workshop cards.
    ui1->cardsScrollArea->viewport()->setAutoFillBackground(false);
    ui1->cardsScrollContents->setAutoFillBackground(false);

    // Remember the booking values associated with the four authored buttons.
    timeSlotValues = {"8.30 am - 10.30 am", "10.30 am - 12.30 pm", "1.30 pm - 3.30 pm",
                      "3.30 pm - 5.30 pm"};
    timeSlotButtons = {ui2->timeBtn1, ui2->timeBtn2, ui2->timeBtn3, ui2->timeBtn4};
    for (int i = 0; i < timeSlotButtons.size(); ++i) {
        // The stylesheet uses [timeSlot=true] to recognise these buttons.
        timeSlotButtons[i]->setProperty("timeSlot", true);
        connect(timeSlotButtons[i], &QPushButton::toggled, this, &MainWindow::onTimeSlotClicked);
    }

    ui2->calendar->setMinimumDate(QDate::currentDate());
    ui2->calendar->setVerticalHeaderFormat(QCalendarWidget::NoVerticalHeader);
    ui4->adminCalendar->setVerticalHeaderFormat(QCalendarWidget::NoVerticalHeader);

    // The administrator table has eleven columns.
    ui4->mainTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    const QList<int> widths = {155, 150, 100, 150, 200, 200, 140, 210, 200, 190, 130};
    for (int i = 0; i < widths.size(); ++i)
        ui4->mainTable->setColumnWidth(i, widths[i]);
    ui4->mainTable->horizontalHeader()->setStretchLastSection(true);
    ui4->mainTable->verticalHeader()->hide();
    ui4->mainTable->verticalHeader()->setDefaultSectionSize(44);
    ui4->mainTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui4->mainTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui4->mainTable->setSelectionMode(QAbstractItemView::SingleSelection);
    ui4->mainTable->setAlternatingRowColors(true);
    ui4->mainTable->setWordWrap(false);
    ui4->mainTable->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);

    ui3->deptInput->setSizeAdjustPolicy(QComboBox::AdjustToMinimumContentsLengthWithIcon);
    ui3->deptInput->setMinimumContentsLength(16);
    ui3->nameInput->setMaxLength(160);
    ui3->indexInput->setMaxLength(80);
    ui3->emailInput->setMaxLength(254);
    ui3->supervisorInput->setMaxLength(160);
    ui3->projectInput->setMaxLength(200);
    ui3->descriptionInput->setAcceptRichText(false);
    ui3->summaryDateLbl->setWordWrap(true);
    ui3->summaryTimeLbl->setWordWrap(true);
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
    // Designer owns every widget's normal style, font, geometry and static text.
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
        for (QWidget *page : {page0Widget, page1Widget, page2Widget, page3Widget, page4Widget})
            page->setStyleSheet(
                QString("QWidget#%1 { border-image:url(\"%2\") 0 0 0 0 stretch stretch; }")
                    .arg(page->objectName(), wallpaper) + page->styleSheet());
    }

    selectCard(selectedCategory);

    updateCalendarHighlights();
    updateAdminCalendarHighlights();
}

// Connect the cards stored in page1.ui. Their position, size, pictures and
// labels are authored in Designer and are never reconstructed at runtime.
void MainWindow::bindWorkshopCards()
{
    for (QFrame *card : ui1->cardsScrollContents->findChildren<QFrame *>()) {
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
    ui1->nextBtn->setEnabled(!category.isEmpty());
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
    ui2->calendar->setMinimumDate(QDate::currentDate());
    ui2->calendar->setSelectedDate(QDate::currentDate());
    clearUI();
    selectCard(QString());
    ui2->adminBtn2->show();
    ui2->backTo1Btn->setText(ui2->backTo1Btn->property("_designerBackText").toString());
    setFormButtonsMode(false);
}

// "Allocate now" on the welcome page.
void MainWindow::startNewAllocation()
{
    if (!databaseReady)
        return;
    resetFlow();
    ui->stackedWidget->setCurrentIndex(1);
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
    ui2->adminBtn2->hide();
    loadRecordIntoUI(currentRecord);
    selectCard(currentCategory);
    const QDate savedDate = QDate::fromString(currentRecord.getRecordDate(), Qt::ISODate);
    // Historical allocations must still expose the delete action and editable details.
    ui->stackedWidget->setCurrentIndex(savedDate < QDate::currentDate() ? 3 : 1);
}

// Leaving a allocation that has not been saved yet.
void MainWindow::cancelCurrentFlow()
{
    const bool hasDraft =
        isStudentEditFlow || isAdminEditMode || !ui3->nameInput->text().isEmpty() ||
        !ui3->indexInput->text().isEmpty() || !ui3->emailInput->text().isEmpty() ||
        !ui3->supervisorInput->text().isEmpty() || !ui3->projectInput->text().isEmpty() ||
        !ui3->descriptionInput->toPlainText().isEmpty();
    if (hasDraft &&
        CustomAlertDialog("Discard changes", "Leave this allocation and discard unsaved changes?",
                          CustomAlertDialog::Question, this)
                .exec() != QDialog::Accepted)
        return;
    resetFlow();
    ui->stackedWidget->setCurrentIndex(0);
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
    ui2->selectedNameLabel2->setText(workshopTitle(category));
    ui2->selectedDescLabel2->setText(
        workshopDepartment(category) +
        "\n\nChoose a date and one or more available time slots.\nGrey slots are unavailable.");
    ui4->adminWsTitleLbl->setText(workshopTitle(category));
    // Fit each selected-workshop picture inside its Designer-authored label.
    ui2->selectedImgLabel2->setPixmap(
        workshopImage(category, QSize(160, 100).boundedTo(ui2->selectedImgLabel2->size())));
    ui4->adminImgLabel->setPixmap(
        workshopImage(category, QSize(160, 100).boundedTo(ui4->adminImgLabel->size())));
    updateCalendarHighlights();
    onCalendarSelectionChanged();
    ui->stackedWidget->setCurrentIndex(2);
}

// "Next" on the date and time page.
void MainWindow::onNextTo3Clicked()
{
    onCalendarSelectionChanged();
    const QStringList selectedSlots = getSelectedTimeSlotsList();
    if (selectedSlots.isEmpty())
        return;
    ui3->summaryDateLbl->setText(workshopTitle(currentCategory) + "  |  " +
                                 ui2->calendar->selectedDate().toString("dddd, d MMMM yyyy"));
    ui3->summaryTimeLbl->setText(selectedSlots.join("  |  "));
    ui->stackedWidget->setCurrentIndex(3);
    ui3->nameInput->setFocus();
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
        ui->stackedWidget->setCurrentIndex(4);
    } else {
        ui->stackedWidget->setCurrentIndex(1);
    }
}

// "Back" on the allocation form page.
void MainWindow::goBackTo2()
{
    ui->stackedWidget->setCurrentIndex(2);
}

// "Back" on the administrator page.
void MainWindow::goBackFromAdmin()
{
    resetFlow();
    ui->stackedWidget->setCurrentIndex(0);
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
    ui2->nextTo3Btn->setEnabled(!getSelectedTimeSlotsList().isEmpty());
}

// Runs every time the student picks a different date.
// Grey slots are the ones already allocated by somebody else.
void MainWindow::onCalendarSelectionChanged()
{
    if (currentCategory.isEmpty()) {
        ui2->nextTo3Btn->setEnabled(false);
        return;
    }
    const QDate date = ui2->calendar->selectedDate();
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
    ui2->calendar->setDateTextFormat(QDate(), QTextCharFormat());
    // Count how many approved, pending and rejected allocations fall on each date.
    QMap<QDate, QMap<QString, int>> counts;
    for (const auto &record : dbManager->getAllRecordsForCategory(currentCategory)) {
        const QDate date = QDate::fromString(record.getRecordDate(), Qt::ISODate);
        if (date.isValid())
            counts[date][record.getStatus()]++;
    }
    ui2->calendar->setStatusCounts(counts);
}

// The same badges, but on the administrator's calendar.
void MainWindow::updateAdminCalendarHighlights()
{
    if (!databaseReady || currentCategory.isEmpty())
        return;
    ui4->adminCalendar->setDateTextFormat(QDate(), QTextCharFormat());
    QMap<QDate, QMap<QString, int>> counts;
    for (const auto &record : dbManager->getAllRecordsForCategory(currentCategory)) {
        const QDate date = QDate::fromString(record.getRecordDate(), Qt::ISODate);
        if (date.isValid())
            counts[date][record.getStatus()]++;
    }
    ui4->adminCalendar->setStatusCounts(counts);
}

// =====================================================================
// 5. ALLOCATION FORM PAGE
// =====================================================================

// Empties every field on the form.
void MainWindow::clearUI()
{
    ui3->nameInput->clear();
    ui3->indexInput->clear();
    ui3->deptInput->setCurrentIndex(0);
    ui3->emailInput->clear();
    ui3->supervisorInput->clear();
    ui3->projectInput->clear();
    ui3->descriptionInput->clear();
    setTimeSlotsInUI(QString());
}

// Shows either "Submit" (new allocation) or "Update" and "Delete" (existing one).
void MainWindow::setFormButtonsMode(bool edit)
{
    ui3->submitBtn->setVisible(!edit);
    ui3->updateBtn->setVisible(edit);
    ui3->deleteBtn->setVisible(edit);
    ui3->confirmTitleLbl->setText(edit ? "Edit your allocation" : ui3->confirmTitleLbl->property("_designerNewTitle").toString());
}

// Puts a saved allocation back onto the pages so it can be read or changed.
void MainWindow::loadRecordIntoUI(const StudentRecord &record)
{
    currentRecord = record;
    const QDate date = QDate::fromString(record.getRecordDate(), Qt::ISODate);
    // Old allocations remain viewable/deletable, but resubmission requires a current date.
    ui2->calendar->setMinimumDate(
        qMin(date.isValid() ? date : QDate::currentDate(), QDate::currentDate()));
    ui2->calendar->setSelectedDate(date);
    onCalendarSelectionChanged();
    setTimeSlotsInUI(record.getTimeSlots());
    ui3->nameInput->setText(record.getName());
    ui3->indexInput->setText(record.getIndexNo());
    ui3->deptInput->setCurrentIndex(0);
    ui3->deptInput->setCurrentText(record.getDepartment());
    ui3->emailInput->setText(record.getEmail());
    ui3->supervisorInput->setText(record.getSupervisorName());
    ui3->projectInput->setText(record.getProjectName());
    ui3->descriptionInput->setPlainText(record.getDescription());
    ui3->summaryDateLbl->setText(workshopTitle(currentCategory) + "  |  " +
                                 date.toString("dddd, d MMMM yyyy"));
    ui3->summaryTimeLbl->setText(record.getTimeSlots());
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
    ui2->backTo1Btn->setText("Back to dashboard");
    ui2->adminBtn2->hide();
    ui->stackedWidget->setCurrentIndex(3);
}

// Reads what is on screen into a StudentRecord object.
StudentRecord MainWindow::readFormRecord()
{
    StudentRecord record = currentRecord;
    record.setName(ui3->nameInput->text().trimmed());
    record.setIndexNo(ui3->indexInput->text().trimmed());
    record.setDepartment(ui3->deptInput->currentText());
    record.setEmail(ui3->emailInput->text().trimmed());
    record.setSupervisorName(ui3->supervisorInput->text().trimmed());
    record.setProjectName(ui3->projectInput->text().trimmed());
    record.setCategory(currentCategory);
    record.setWorkshopId(currentWorkshopId);
    record.setRecordDate(ui2->calendar->selectedDate().toString(Qt::ISODate));
    record.setTimeSlots(getSelectedTimeSlotsList().join(", "));
    record.setDescription(ui3->descriptionInput->toPlainText().trimmed());
    return record;
}

// Checks the form. If something is wrong it shows a message and moves the
// cursor to the field that needs attention.
bool MainWindow::validateForm()
{
    const AllocationRules::Field field =
        AllocationRules::validate(readFormRecord(), ui3->deptInput->currentIndex() > 0);
    if (field == AllocationRules::Field::None) {
        return true;
    }
    CustomAlertDialog("Check allocation details", AllocationRules::validationMessage(field),
                      CustomAlertDialog::Warning, this)
        .exec();
    // Only the UI controller is responsible for keyboard focus and page changes.
    switch (field) {
    case AllocationRules::Field::Name:
        ui3->nameInput->setFocus();
        break;
    case AllocationRules::Field::Index:
        ui3->indexInput->setFocus();
        break;
    case AllocationRules::Field::Department:
        ui3->deptInput->setFocus();
        break;
    case AllocationRules::Field::Email:
        ui3->emailInput->setFocus();
        break;
    case AllocationRules::Field::Supervisor:
        ui3->supervisorInput->setFocus();
        break;
    case AllocationRules::Field::Project:
        ui3->projectInput->setFocus();
        break;
    case AllocationRules::Field::Schedule:
        ui->stackedWidget->setCurrentIndex(2);
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
    ui->stackedWidget->setCurrentIndex(0);
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
        ui->stackedWidget->setCurrentIndex(4);
    } else {
        resetFlow();
        ui->stackedWidget->setCurrentIndex(0);
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
        ui->stackedWidget->setCurrentIndex(4);
    } else {
        resetFlow();
        ui->stackedWidget->setCurrentIndex(0);
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
    ui->stackedWidget->setCurrentIndex(4);
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
    const QSignalBlocker blocker(ui4->adminCalendar);
    ui4->adminCalendar->setSelectedDate(QDate::currentDate());
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
        ui4->mainTable->currentRow() >= 0 && !ui4->mainTable->selectedItems().isEmpty();
    for (QPushButton *button :
         {ui4->adminEditBtn, ui4->adminDelBtn, ui4->pendingEmailBtn, ui4->rejectEmailBtn})
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
    const QSignalBlocker tableBlocker(ui4->mainTable);
    ui4->mainTable->setRowCount(0);
    for (int row = 0; row < records.size(); ++row) {
        const StudentRecord record = records[row];
        ui4->mainTable->insertRow(row);
        const QStringList values = {record.getPasscode(),   record.getCreatedAt(),
                                    record.getIndexNo(),    record.getName(),
                                    record.getDepartment(), record.getEmail(),
                                    record.getRecordDate(), record.getTimeSlots(),
                                    record.getDescription(), record.getAdminRemarks(),
                                    record.getStatus()};
        for (int column = 0; column < values.size(); ++column) {
            auto *item = new QTableWidgetItem(values[column]);
            item->setToolTip(values[column]);
            ui4->mainTable->setItem(row, column, item);
        }
        // The real database id is hidden inside the first cell.
        ui4->mainTable->item(row, 0)->setData(Qt::UserRole, record.getId());

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
        ui4->mainTable->setCellWidget(row, 9, remarks);

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
        ui4->mainTable->setCellWidget(row, 10, status);
    }
    ui4->adminWsDescLbl->setText(
        workshopDepartment(currentCategory) + "\n" + QString::number(records.size()) +
        (records.size() == 1 ? " allocation" : " allocations") +
        (adminFilterDate.isValid() ? " on " + adminFilterDate.toString("d MMM yyyy")
                                   : " · All dates"));
    ui4->adminShowAllBtn->setEnabled(adminFilterDate.isValid());
    refreshAdminActions();
    updateAdminCalendarHighlights();
}

// Saves the new status and opens an email draft telling the student about it.
void MainWindow::composeStatusEmail(const QString &status)
{
    const int row = ui4->mainTable->currentRow();
    if (row < 0)
        return;
    const int id = ui4->mainTable->item(row, 0)->data(Qt::UserRole).toInt();
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
            // Copy in the dean and the head of department if their addresses
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

// Prints the allocations that are currently listed in the table.
void MainWindow::printAllRecords()
{
    const auto records = adminFilterDate.isValid()
                             ? dbManager->getRecordsByDateAndCategory(
                                   adminFilterDate.toString(Qt::ISODate), currentCategory)
                             : dbManager->getAllRecordsForCategory(currentCategory);
    if (!dbManager->getLastError().isEmpty()) {
        showDatabaseError();
        return;
    }
    // Build the report as simple HTML and hand it to the printer.
    QString html =
        "<h1>" + workshopTitle(currentCategory).toHtmlEscaped() + "</h1><p>" +
        (adminFilterDate.isValid() ? adminFilterDate.toString("d MMM yyyy") : "All dates") +
        "</p><table border='1' cellspacing='0' "
        "cellpadding='5'><tr><th>Date</th><th>Name</th><th>Index</th><th>Time "
        "slots</th><th>Status</th></tr>";
    for (const auto &record : records)
        html += "<tr><td>" + record.getRecordDate().toHtmlEscaped() + "</td><td>" +
                record.getName().toHtmlEscaped() + "</td><td>" +
                record.getIndexNo().toHtmlEscaped() + "</td><td>" +
                record.getTimeSlots().toHtmlEscaped() + "</td><td>" +
                record.getStatus().toHtmlEscaped() + "</td></tr>";
    html += "</table>";
    QPrinter printer(QPrinter::HighResolution);
    QPrintDialog dialog(&printer, this);
    if (dialog.exec() != QDialog::Accepted)
        return;
    QTextDocument document;
    document.setHtml(html);
    document.print(&printer);
}
