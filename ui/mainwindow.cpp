// MainWindow is the controller of the program.
// Everything the window does lives in this one file, grouped into sections:
//   1. Starting the program  (constructor, destructor, first setup)
//   2. Appearance            (theme, workshop cards, window resizing)
//   3. Moving between pages
//   4. Date and time page
//   5. Booking form page
//   6. Administrator page    (table, email drafts, printing, settings)
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "ui_page0.h"
#include "ui_page1.h"
#include "ui_page2.h"
#include "ui_page3.h"
#include "ui_page4.h"
#include "customdialogs.h"
#include "workshopcatalog.h"
#include "filestorage.h"
#include "bookingrules.h"
#include <QComboBox>
#include <QDesktopServices>
#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QHeaderView>
#include <QLabel>
#include <QLineEdit>
#include <QMap>
#include <QPalette>
#include <QPrintDialog>
#include <QPrinter>
#include <QPushButton>
#include <QResizeEvent>
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
#include <QVBoxLayout>

// =====================================================================
// 1. STARTING THE PROGRAM
// =====================================================================

// Constructor
MainWindow::MainWindow(QWidget *parent, const QString &databasePath)
    : QMainWindow(parent), ui(new Ui::MainWindow), ui0(new Ui::Page0), ui1(new Ui::Page1),
      ui2(new Ui::Page2), ui3(new Ui::Page3), ui4(new Ui::Page4)
{
    // Build the outer window first, then add the five pages to its stack.
    ui->setupUi(this);
    page0Widget = new QWidget();
    ui0->setupUi(page0Widget);
    ui->stackedWidget->addWidget(page0Widget);
    page1Widget = new QWidget();
    ui1->setupUi(page1Widget);
    ui->stackedWidget->addWidget(page1Widget);
    page2Widget = new QWidget();
    ui2->setupUi(page2Widget);
    ui->stackedWidget->addWidget(page2Widget);
    page3Widget = new QWidget();
    ui3->setupUi(page3Widget);
    ui->stackedWidget->addWidget(page3Widget);
    page4Widget = new QWidget();
    ui4->setupUi(page4Widget);
    ui->stackedWidget->addWidget(page4Widget);

    setWindowTitle("SYNCLAB | Workshop & Laboratory Bookings");
    setWindowIcon(QIcon(workshopImage("SYNCLAB", QSize(64, 64))));

    // Open the SQLite database and make sure the table exists.
    const QString path = databasePath.isEmpty() ? defaultDatabasePath() : databasePath;
    dataDirectory = QFileInfo(path).absolutePath();
    dbManager = new SqliteDatabaseManager(path);
    databaseReady = dbManager->createTable();

    setupDynamicUI();
    applyTheme(QSettings(QSettings::defaultFormat(), QSettings::UserScope, "StudentWorkshopApp",
                         "ThemeSettings")
                   .value("darkMode", false)
                   .toBool());
    ui->stackedWidget->setCurrentIndex(0);

    // Connect every button to the function that should run when it is clicked.
    connect(ui0->allocateNowBtn, &QPushButton::clicked, this, &MainWindow::startNewBooking);
    connect(ui0->editBookingBtn, &QPushButton::clicked, this, &MainWindow::startEditBookingFlow);
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

    // "Browse" lets the student choose a PDF to attach.
    connect(ui3->browsePdfBtn, &QPushButton::clicked, this, [this] {
        const QString path = QFileDialog::getOpenFileName(this, "Attach a PDF document", {},
                                                          "PDF documents (*.pdf)");
        if (!path.isEmpty())
            ui3->pdfInput->setText(path);
    });

    // This button does not exist in Designer, so it is created here.
    auto *removePdf = new QPushButton("Remove", page3Widget);
    removePdf->setObjectName("removePdfBtn");
    removePdf->setToolTip("Remove this booking's attachment");
    ui3->pdfLayout->addWidget(removePdf);
    connect(removePdf, &QPushButton::clicked, ui3->pdfInput, &QLineEdit::clear);

    // Administrator: open the selected booking in the form.
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

    // Administrator: delete the selected booking.
    connect(ui4->adminDelBtn, &QPushButton::clicked, this, [this] {
        const int row = ui4->mainTable->currentRow();
        if (row < 0)
            return;
        const int id = ui4->mainTable->item(row, 0)->data(Qt::UserRole).toInt();
        if (CustomAlertDialog("Delete booking", "Permanently delete the selected booking?",
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

    // Double-clicking the PDF column opens the attachment.
    connect(ui4->mainTable, &QTableWidget::cellDoubleClicked, this, [this](int row, int column) {
        if (column != 6)
            return;
        const QString path = ui4->mainTable->item(row, column)->data(Qt::UserRole).toString();
        if (path.isEmpty())
            return;
        if (!QFileInfo::exists(path) || !QDesktopServices::openUrl(QUrl::fromLocalFile(path)))
            CustomAlertDialog("Cannot open attachment",
                              "The PDF is missing or no PDF viewer is available.",
                              CustomAlertDialog::Error, this)
                .exec();
    });

    auto *printButton = new QPushButton("Print list", page4Widget);
    printButton->setObjectName("printRecordsBtn");
    ui4->tableActionsLayout->addWidget(printButton);
    connect(printButton, &QPushButton::clicked, this, &MainWindow::printAllRecords);

    // If the database could not be opened, booking must be switched off.
    if (!databaseReady) {
        ui0->allocateNowBtn->setEnabled(false);
        ui0->editBookingBtn->setEnabled(false);
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

// Sets up the parts of the interface that Designer cannot store: the time slot
// values, the table columns and the typing limits on the form.
void MainWindow::setupDynamicUI()
{
    ui1->cardsLayout->setAlignment(Qt::AlignTop | Qt::AlignHCenter);

    // Remember the four time slots and label the four buttons with them.
    timeSlotValues = {"8.30 am - 10.30 am", "10.30 am - 12.30 pm", "1.30 pm - 3.30 pm",
                      "3.30 pm - 5.30 pm"};
    timeSlotButtons = {ui2->timeBtn1, ui2->timeBtn2, ui2->timeBtn3, ui2->timeBtn4};
    for (int i = 0; i < timeSlotButtons.size(); ++i) {
        // The stylesheet uses [timeSlot=true] to recognise these buttons.
        timeSlotButtons[i]->setProperty("timeSlot", true);
        timeSlotButtons[i]->setText(timeSlotValues[i].toUpper());
        connect(timeSlotButtons[i], &QPushButton::toggled, this, &MainWindow::onTimeSlotClicked);
    }

    ui2->calendar->setMinimumDate(QDate::currentDate());
    ui2->calendar->setVerticalHeaderFormat(QCalendarWidget::NoVerticalHeader);
    ui4->adminCalendar->setVerticalHeaderFormat(QCalendarWidget::NoVerticalHeader);

    // The administrator table has twelve columns.
    ui4->mainTable->setColumnCount(12);
    ui4->mainTable->setHorizontalHeaderLabels({"ALLOCATION KEY", "CREATED", "INDEX", "NAME",
                                               "DEPARTMENT", "EMAIL", "PDF", "BOOKING DATE",
                                               "TIME SLOTS", "DESCRIPTION", "REMARKS", "STATUS"});
    ui4->mainTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    const QList<int> widths = {155, 150, 100, 150, 200, 200, 145, 120, 210, 200, 190, 130};
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
    CustomAlertDialog("Could not save or load booking",
                      error.isEmpty() ? "The database is unavailable. Please try again." : error,
                      CustomAlertDialog::Error, this)
        .exec();
}

// =====================================================================
// 2. APPEARANCE: THEME, WORKSHOP CARDS, WINDOW RESIZING
// =====================================================================

// Applies the light or dark colour scheme to the whole window.
void MainWindow::applyTheme(bool darkMode)
{
    isDarkTheme = darkMode;
    // Clear old stylesheets so the new colours are not mixed with the old ones.
    for (QWidget *widget : findChildren<QWidget *>())
        widget->setStyleSheet("");

    const QString bg = darkMode ? "#0F172A" : "#F4F7FB";
    const QString surface = darkMode ? "#1E293B" : "#FFFFFF";
    const QString text = darkMode ? "#F1F5F9" : "#17243B";
    const QString muted = darkMode ? "#AABAD0" : "#52647D";
    const QString border = darkMode ? "#43536B" : "#D4DEEB";
    const QString accent = darkMode ? "#8DB8FF" : "#2563EB";
    const QString soft = darkMode ? "#253C61" : "#EAF2FF";

    QPalette palette;
    palette.setColor(QPalette::Window, QColor(bg));
    palette.setColor(QPalette::WindowText, QColor(text));
    palette.setColor(QPalette::Base, QColor(surface));
    palette.setColor(QPalette::AlternateBase, QColor(bg));
    palette.setColor(QPalette::Text, QColor(text));
    palette.setColor(QPalette::Button, QColor(surface));
    palette.setColor(QPalette::ButtonText, QColor(text));
    palette.setColor(QPalette::Highlight, QColor("#2563EB"));
    palette.setColor(QPalette::HighlightedText, Qt::white);
    palette.setColor(QPalette::PlaceholderText, QColor(muted));
    palette.setColor(QPalette::Disabled, QPalette::Text, QColor(muted));
    palette.setColor(QPalette::Disabled, QPalette::ButtonText, QColor(muted));
    setPalette(palette);

    // One stylesheet for the whole window. %1 to %8 are filled in by arg().
    setStyleSheet(
        QString("QWidget { color:%1; font-family:'Segoe UI'; font-size:13px; }"
                "QMainWindow, QStackedWidget, QWidget#page0Widget, QWidget#page1Widget, "
                "QWidget#page2Widget, QWidget#page3Widget, QWidget#page4Widget { background:%2; }"
                "QScrollArea { border:none; background:transparent; } QScrollArea > QWidget > "
                "QWidget { background:%2; }"
                "QLabel { background:transparent; }"
                "QLabel#welcomeTitle { color:%4; font-size:16px; font-weight:600; }"
                "QLabel#syncLabTitle { font-size:48px; font-weight:800; }"
                "QLabel#welcomeDesc, QLabel#selectedDescLabel2, QLabel#adminWsDescLbl { color:%4; "
                "font-size:14px; }"
                "QLabel#selectTitleLabel, QLabel#selectedNameLabel2, QLabel#confirmTitleLbl, "
                "QLabel#adminWsTitleLbl { font-size:25px; font-weight:700; }"
                "QLineEdit, QTextEdit, QComboBox { background:%3; border:1px solid %5; "
                "border-radius:7px; padding:8px; selection-background-color:#2563EB; "
                "selection-color:white; }"
                "QLineEdit:focus, QTextEdit:focus, QComboBox:focus { border:1px solid %6; }"
                "QComboBox QAbstractItemView { background:%3; color:%1; "
                "selection-background-color:#2563EB; selection-color:white; }"
                "QPushButton { background:%3; color:%1; border:1px solid %5; border-radius:8px; "
                "padding:9px 14px; font-weight:600; }"
                "QPushButton:hover, QPushButton:focus { background:%7; border-color:%6; }"
                "QPushButton:pressed { border:2px solid %6; }"
                "QPushButton#allocateNowBtn, QPushButton#nextBtn, QPushButton#nextTo3Btn, "
                "QPushButton#submitBtn, QPushButton#updateBtn { background:#2563EB; color:white; "
                "border:1px solid #2563EB; }"
                "QPushButton#allocateNowBtn:hover, QPushButton#nextBtn:hover, "
                "QPushButton#nextTo3Btn:hover, QPushButton#submitBtn:hover, "
                "QPushButton#updateBtn:hover { background:#1D4ED8; }"
                "QPushButton#deleteBtn, QPushButton#adminDelBtn { color:%8; }"
                "QPushButton[timeSlot=true] { padding:10px 8px; }"
                "QPushButton:checked { background:#2563EB; color:white; border-color:#2563EB; }"
                "QPushButton:disabled { background:%2; color:%4; border:1px solid %5; }"
                "QPushButton#allocateNowBtn:disabled, QPushButton#nextBtn:disabled, "
                "QPushButton#nextTo3Btn:disabled, QPushButton#submitBtn:disabled, "
                "QPushButton#updateBtn:disabled, QPushButton#deleteBtn:disabled, "
                "QPushButton#adminDelBtn:disabled { background:%2; color:%4; border:1px solid %5; }"
                "QFrame#summaryBox { background:%7; border:1px solid %5; border-radius:10px; }"
                "QLabel#summaryDateLbl { color:%6; font-weight:700; }"
                "QTableWidget { background:%3; alternate-background-color:%2; border:1px solid %5; "
                "gridline-color:%5; selection-background-color:%7; selection-color:%1; }"
                "QHeaderView::section { background:%7; color:%1; border:none; padding:10px; "
                "font-weight:600; }"
                "QTableWidget QLineEdit, QTableWidget QComboBox { padding:4px; border-radius:4px; }"
                "QCalendarWidget { background:%3; border:1px solid %5; border-radius:8px; }"
                "QCalendarWidget QWidget#qt_calendar_navigationbar { background:#2563EB; }"
                "QCalendarWidget QToolButton { color:white; background:transparent; padding:6px; "
                "font-weight:600; }"
                "QCalendarWidget QMenu, QCalendarWidget QSpinBox { background:%3; color:%1; }"
                "QCalendarWidget QAbstractItemView { background:%3; color:%1; "
                "selection-background-color:#2563EB; selection-color:white; outline:0; }"
                "QToolTip { background:%3; color:%1; border:1px solid %5; padding:6px; }")
            .arg(text, bg, surface, muted, border, accent, soft,
                 darkMode ? "#FDA4AF" : "#BE123C"));

    ui0->welcomeImg->setPixmap(workshopImage("SYNCLAB", QSize(110, 110)));

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
                    .arg(page->objectName(), wallpaper));
    }

    // The cards are created the first time the theme is applied.
    if (cardFrames.isEmpty())
        drawCards();
    else
        selectCard(selectedCategory);

    updateCalendarHighlights();
    updateAdminCalendarHighlights();
}

// Builds one workshop card: picture, title, description and a click layer.
QWidget *MainWindow::createCard(const QString &id, const QString &title, const QString &subtitle)
{
    auto *card = new QFrame(page1Widget);
    card->setObjectName("cardFrame");
    card->setProperty("category", id);
    card->setFixedSize(212, 210);

    auto *layout = new QVBoxLayout(card);
    layout->setContentsMargins(14, 14, 14, 14);
    layout->setSpacing(8);

    auto *image = new QLabel(card);
    image->setPixmap(workshopImage(id, QSize(110, 80)));
    image->setAlignment(Qt::AlignCenter);

    auto *heading = new QLabel(title, card);
    heading->setAlignment(Qt::AlignCenter);
    heading->setWordWrap(true);
    QFont font = heading->font();
    font.setBold(true);
    heading->setFont(font);

    auto *description = new QLabel(subtitle, card);
    description->setAlignment(Qt::AlignCenter);
    description->setWordWrap(true);

    layout->addWidget(image);
    layout->addWidget(heading);
    layout->addWidget(description, 1);

    // A transparent button on top of the card catches the mouse click.
    auto *overlay = new QPushButton(card);
    overlay->setObjectName("workshopCard_" + QString(id).replace(' ', '_'));
    overlay->setAccessibleName(title);
    overlay->setToolTip(title + "\n" + subtitle);
    overlay->setGeometry(card->rect());
    overlay->setCursor(Qt::PointingHandCursor);
    overlay->raise();
    connect(overlay, &QPushButton::clicked, this, [this, id] { selectCard(id); });

    cardFrames.append(card);
    return card;
}

// Creates a card for every workshop in the catalogue.
void MainWindow::drawCards()
{
    for (const QString &category : workshopCategories())
        createCard(category, workshopTitle(category), workshopDepartment(category));
    cardColumns = 0; // force arrangeCards() to lay them out
    arrangeCards();
    selectCard(selectedCategory);
}

// Highlights the chosen card and enables or disables the Next button.
void MainWindow::selectCard(const QString &category)
{
    selectedCategory = category;
    for (QFrame *card : cardFrames) {
        const bool selected = card->property("category").toString() == category;
        card->setStyleSheet(
            QString("QFrame#cardFrame { background:%1; border:2px solid %2; border-radius:12px; }")
                .arg(selected ? (isDarkTheme ? "#253C61" : "#EAF2FF")
                              : (isDarkTheme ? "#1E293B" : "#FFFFFF"),
                     selected ? "#6099F5" : (isDarkTheme ? "#43536B" : "#D4DEEB")));
        auto *button = card->findChild<QPushButton *>();
        button->setStyleSheet("QPushButton { background:transparent; border:2px solid transparent; "
                              "border-radius:12px; } QPushButton:hover, QPushButton:focus { "
                              "border:2px solid #6099F5; background:rgba(80,140,240,18); }");
        button->setAccessibleDescription(selected ? "Selected workshop" : "Select workshop");
    }
    ui1->nextBtn->setEnabled(!category.isEmpty());
}

// Qt calls this whenever the window is resized.
void MainWindow::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);
    // Wait until the resize has finished before moving the cards around.
    QTimer::singleShot(0, this, &MainWindow::arrangeCards);
}

// Puts the cards into as many columns as the window width allows.
void MainWindow::arrangeCards()
{
    if (!page1Widget)
        return;
    const auto *area = page1Widget->findChild<QScrollArea *>("cardsScrollArea");
    const int available = area ? area->viewport()->width() : width() - 70;
    const int columns = qBound(1, (available - 12) / 228, 5);
    if (columns == cardColumns)
        return; // nothing changed, so do not rebuild the grid
    cardColumns = columns;

    while (auto *item = ui1->cardsLayout->takeAt(0))
        delete item;

    // Keep the additional laboratories below the original ten, even on narrower windows.
    constexpr int originalWorkshopCount = 10;
    const int additionalRow = (originalWorkshopCount + columns - 1) / columns;
    for (int i = 0; i < cardFrames.size(); ++i) {
        const int offset = i < originalWorkshopCount ? i : i - originalWorkshopCount;
        const int row =
            i < originalWorkshopCount ? offset / columns : additionalRow + offset / columns;
        ui1->cardsLayout->addWidget(cardFrames[i], row, offset % columns);
    }
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
    ui2->backTo1Btn->setText("Back to workshops");
    setFormButtonsMode(false);
}

// "Allocate now" on the welcome page.
void MainWindow::startNewBooking()
{
    if (!databaseReady)
        return;
    resetFlow();
    ui->stackedWidget->setCurrentIndex(1);
    arrangeCards();
}

// "Edit booking" on the welcome page. The student types the allocation key.
void MainWindow::startEditBookingFlow()
{
    if (!databaseReady)
        return;
    CustomInputDialog dialog("Edit booking", "Enter your allocation key:", this);
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
            "Confirm booking",
            "This allocation key matches multiple bookings.\nEnter your student index:", this);
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
        CustomAlertDialog("Booking not found",
                          "The details do not identify a single booking. Check your allocation key "
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
    // Historical bookings must still expose the delete action and editable details.
    ui->stackedWidget->setCurrentIndex(savedDate < QDate::currentDate() ? 3 : 1);
    arrangeCards();
}

// Leaving a booking that has not been saved yet.
void MainWindow::cancelCurrentFlow()
{
    const bool hasDraft =
        isStudentEditFlow || isAdminEditMode || !ui3->nameInput->text().isEmpty() ||
        !ui3->indexInput->text().isEmpty() || !ui3->emailInput->text().isEmpty() ||
        !ui3->descriptionInput->toPlainText().isEmpty() || !ui3->pdfInput->text().isEmpty();
    if (hasDraft &&
        CustomAlertDialog("Discard changes", "Leave this booking and discard unsaved changes?",
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
    const QPixmap pix = workshopImage(category, QSize(160, 100));
    ui2->selectedImgLabel2->setPixmap(pix);
    ui4->adminImgLabel->setPixmap(pix);
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
        arrangeCards();
    }
}

// "Back" on the booking form page.
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

// Ticks the buttons that belong to a saved booking.
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
// Grey slots are the ones already booked by somebody else.
void MainWindow::onCalendarSelectionChanged()
{
    if (currentCategory.isEmpty()) {
        ui2->nextTo3Btn->setEnabled(false);
        return;
    }
    const QDate date = ui2->calendar->selectedDate();
    // The student's own booking is excluded, so editing it does not block itself.
    const auto booked = dbManager->getBookedTimeSlotsStringList(
        date.toString(Qt::ISODate), currentCategory, currentRecord.getId());
    const bool queryOk = dbManager->getLastError().isEmpty();
    for (int i = 0; i < timeSlotButtons.size(); ++i) {
        QPushButton *button = timeSlotButtons[i];
        const bool available = queryOk && date >= QDate::currentDate() &&
                               !booked.contains(timeSlotValues[i]);
        button->setEnabled(available);
        if (!available)
            button->setChecked(false);
        button->setToolTip(
            !queryOk ? "Availability could not be loaded"
                     : (available ? "Select this time slot" : "This time slot is unavailable"));
    }
    onTimeSlotClicked();
}

// Draws the small A / P / R badges on the booking calendar.
void MainWindow::updateCalendarHighlights()
{
    if (!databaseReady || currentCategory.isEmpty())
        return;
    ui2->calendar->setDateTextFormat(QDate(), QTextCharFormat());
    // Count how many approved, pending and rejected bookings fall on each date.
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
// 5. BOOKING FORM PAGE
// =====================================================================

// Empties every field on the form.
void MainWindow::clearUI()
{
    ui3->nameInput->clear();
    ui3->indexInput->clear();
    ui3->deptInput->setCurrentIndex(0);
    ui3->emailInput->clear();
    ui3->pdfInput->clear();
    ui3->descriptionInput->clear();
    setTimeSlotsInUI(QString());
}

// Shows either "Submit" (new booking) or "Update" and "Delete" (existing one).
void MainWindow::setFormButtonsMode(bool edit)
{
    ui3->submitBtn->setVisible(!edit);
    ui3->updateBtn->setVisible(edit);
    ui3->deleteBtn->setVisible(edit);
    ui3->confirmTitleLbl->setText(edit ? "Edit your booking" : "Finalize your booking");
}

// Puts a saved booking back onto the pages so it can be read or changed.
void MainWindow::loadRecordIntoUI(const StudentRecord &record)
{
    currentRecord = record;
    const QDate date = QDate::fromString(record.getRecordDate(), Qt::ISODate);
    // Old bookings remain viewable/deletable, but resubmission requires a current date.
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
    ui3->pdfInput->setText(record.getPdfPath());
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
    const BookingRules::Field field =
        BookingRules::validate(readFormRecord(), ui3->deptInput->currentIndex() > 0);
    if (field == BookingRules::Field::None) {
        return true;
    }
    CustomAlertDialog("Check booking details", BookingRules::validationMessage(field),
                      CustomAlertDialog::Warning, this)
        .exec();
    // Only the UI controller is responsible for keyboard focus and page changes.
    switch (field) {
    case BookingRules::Field::Name:
        ui3->nameInput->setFocus();
        break;
    case BookingRules::Field::Index:
        ui3->indexInput->setFocus();
        break;
    case BookingRules::Field::Department:
        ui3->deptInput->setFocus();
        break;
    case BookingRules::Field::Email:
        ui3->emailInput->setFocus();
        break;
    case BookingRules::Field::Schedule:
        ui->stackedWidget->setCurrentIndex(2);
        break;
    case BookingRules::Field::None:
        break;
    }
    return false;
}

// Copies the chosen PDF into the program's own uploads folder.
QString MainWindow::savePdfFile(const QString &sourcePath, const QString &allocationKey)
{
    QString error;
    const QString savedPath =
        savePdfAttachment(sourcePath, allocationKey, dataDirectory, error);
    if (!error.isEmpty()) {
        CustomAlertDialog("Attachment unavailable", error, CustomAlertDialog::Error, this).exec();
    }
    return savedPath;
}

// "Submit" on the booking form: INSERT a new record.
void MainWindow::submitData()
{
    if (currentRecord.getId() >= 0 || !validateForm())
        return;
    StudentRecord record = readFormRecord();
    // Gives the booking its 4-digit allocation key, a "Pending" status and a
    // creation timestamp.
    BookingRules::prepareNewBooking(record);
    const QString source = ui3->pdfInput->text();
    const QString pdf = savePdfFile(source, record.getPasscode());
    if (!source.isEmpty() && pdf.isEmpty())
        return;
    record.setPdfPath(pdf);
    if (!dbManager->addRecord(record)) {
        // The insert failed, so remove the copied file to avoid orphans.
        if (!pdf.isEmpty() && pdf != source)
            QFile::remove(pdf);
        showDatabaseError();
        return;
    }
    CustomAlertDialog("Booking saved",
                      "Your booking is pending approval.\n\nAllocation key: " +
                          record.getPasscode() +
                          "\n\nKeep your allocation key to edit this booking.\n" +
                          workshopTitle(currentCategory) + "\n" + record.getRecordDate() + "\n" +
                          record.getTimeSlots(),
                      CustomAlertDialog::Info, this)
        .exec();
    resetFlow();
    ui->stackedWidget->setCurrentIndex(0);
}

// "Update" on the booking form: UPDATE the existing record.
void MainWindow::updateData()
{
    if (currentRecord.getId() < 0 || (!isPasscodeBypassed && !checkPasscode()) || !validateForm())
        return;
    StudentRecord updated = readFormRecord();
    BookingRules::prepareUpdatedBooking(updated, isStudentEditFlow);
    const QString source = ui3->pdfInput->text();
    const QString pdf = savePdfFile(source, updated.getPasscode());
    if (!source.isEmpty() && pdf.isEmpty())
        return;
    updated.setPdfPath(pdf);
    if (!dbManager->updateRecord(updated)) {
        if (!pdf.isEmpty() && pdf != source)
            QFile::remove(pdf);
        showDatabaseError();
        return;
    }
    CustomAlertDialog("Booking updated",
                      isStudentEditFlow ? "Your changes have been saved and are pending approval."
                                        : "Booking changes saved.",
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

// "Delete" on the booking form: DELETE the record after a yes/no question.
void MainWindow::deleteData()
{
    if (currentRecord.getId() < 0 || (!isPasscodeBypassed && !checkPasscode()))
        return;
    if (CustomAlertDialog("Delete booking", "Permanently delete this booking?",
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

// SELECTs the bookings and fills the table with them.
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
        const QStringList values = {record.getPasscode(),
                                    record.getCreatedAt(),
                                    record.getIndexNo(),
                                    record.getName(),
                                    record.getDepartment(),
                                    record.getEmail(),
                                    record.getPdfPath().isEmpty()
                                        ? "No attachment"
                                        : QFileInfo(record.getPdfPath()).fileName(),
                                    record.getRecordDate(),
                                    record.getTimeSlots(),
                                    record.getDescription(),
                                    record.getAdminRemarks(),
                                    record.getStatus()};
        for (int column = 0; column < values.size(); ++column) {
            auto *item = new QTableWidgetItem(values[column]);
            item->setToolTip(values[column]);
            ui4->mainTable->setItem(row, column, item);
        }
        // The real database id and the PDF path are hidden inside the cells.
        ui4->mainTable->item(row, 0)->setData(Qt::UserRole, record.getId());
        ui4->mainTable->item(row, 6)->setData(Qt::UserRole, record.getPdfPath());
        ui4->mainTable->item(row, 6)->setToolTip(
            record.getPdfPath().isEmpty() ? "No PDF attached" : "Double-click to open the PDF");

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
        ui4->mainTable->setCellWidget(row, 10, remarks);

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
        ui4->mainTable->setCellWidget(row, 11, status);
    }
    ui4->adminWsDescLbl->setText(
        workshopDepartment(currentCategory) + "\n\n" + QString::number(records.size()) +
        (records.size() == 1 ? " booking" : " bookings") +
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
        query.addQueryItem("subject", "Workshop booking " +
                                          QString(pending ? "pending" : "rejected") + ": " +
                                          workshopTitle(currentCategory));
        query.addQueryItem("body",
                           "Dear " + record.getName() + ",\n\nYour booking for " +
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
                "The booking status was saved, but no email application could open the draft.",
                CustomAlertDialog::Warning, this)
                .exec();
        else
            statusBar()->showMessage(
                "Email draft opened. Review and send it in your email application.", 10000);
        return;
    }
}

// Prints the bookings that are currently listed in the table.
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
