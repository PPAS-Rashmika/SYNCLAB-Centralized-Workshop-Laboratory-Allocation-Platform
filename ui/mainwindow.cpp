// Creates the window, connects buttons, and sets up shared widgets.
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "ui_page0.h"
#include "ui_page1.h"
#include "ui_page2.h"
#include "ui_page3.h"
#include "ui_page4.h"
#include "customdialogs.h"
#include "workshopcatalog.h"
#include "apphelpers.h"
#include <QDesktopServices>
#include <QFileDialog>
#include <QFileInfo>
#include <QHeaderView>
#include <QSettings>
#include <QTimer>
#include <QUrl>

MainWindow::MainWindow(QWidget *parent, const QString &databasePath)
    : QMainWindow(parent), ui(new Ui::MainWindow), ui0(new Ui::Page0), ui1(new Ui::Page1),
      ui2(new Ui::Page2), ui3(new Ui::Page3), ui4(new Ui::Page4), isPasscodeBypassed(false),
      isAdminEditMode(false), isStudentEditFlow(false)
{
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
    const QString path = databasePath.isEmpty() ? defaultDatabasePath() : databasePath;
    m_dataDirectory = QFileInfo(path).absolutePath();
    dbManager = new SqliteDatabaseManager(path);
    m_databaseReady = dbManager->createTable();
    setupDynamicUI();
    applyTheme(QSettings(QSettings::defaultFormat(), QSettings::UserScope, "StudentWorkshopApp",
                         "ThemeSettings")
                   .value("darkMode", false)
                   .toBool());
    ui->stackedWidget->setCurrentIndex(0);
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
    connect(ui3->browsePdfBtn, &QPushButton::clicked, this, [this] {
        const QString path = QFileDialog::getOpenFileName(this, "Attach a PDF document", {},
                                                          "PDF documents (*.pdf)");
        if (!path.isEmpty())
            ui3->pdfInput->setText(path);
    });
    auto *removePdf = new QPushButton("Remove", page3Widget);
    removePdf->setObjectName("removePdfBtn");
    removePdf->setToolTip("Remove this booking's attachment");
    ui3->pdfLayout->addWidget(removePdf);
    connect(removePdf, &QPushButton::clicked, ui3->pdfInput, &QLineEdit::clear);
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
    if (!m_databaseReady) {
        ui0->allocateNowBtn->setEnabled(false);
        ui0->editBookingBtn->setEnabled(false);
        QTimer::singleShot(0, this, &MainWindow::showDatabaseError);
    }
}

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

void MainWindow::setupDynamicUI()
{
    ui1->cardsLayout->setAlignment(Qt::AlignTop | Qt::AlignHCenter);
    const QStringList timeValues = {"8.30 am - 10.30 am", "10.30 am - 12.30 pm",
                                    "1.30 pm - 3.30 pm", "3.30 pm - 5.30 pm"};
    const QList<QPushButton *> buttons = {ui2->timeBtn1, ui2->timeBtn2, ui2->timeBtn3,
                                          ui2->timeBtn4};
    for (int i = 0; i < buttons.size(); ++i) {
        buttons[i]->setProperty("timeVal", timeValues[i]);
        buttons[i]->setProperty("timeSlot", true);
        buttons[i]->setText(timeValues[i].toUpper());
        connect(buttons[i], &QPushButton::toggled, this, &MainWindow::onTimeSlotClicked);
    }
    ui2->calendar->setMinimumDate(QDate::currentDate());
    ui2->calendar->setVerticalHeaderFormat(QCalendarWidget::NoVerticalHeader);
    ui4->adminCalendar->setVerticalHeaderFormat(QCalendarWidget::NoVerticalHeader);
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

void MainWindow::showDatabaseError()
{
    const QString error = dbManager->getLastError();
    CustomAlertDialog("Could not save or load booking",
                      error.isEmpty() ? "The database is unavailable. Please try again." : error,
                      CustomAlertDialog::Error, this)
        .exec();
}
