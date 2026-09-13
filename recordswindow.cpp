#include "recordswindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QHeaderView>
#include <QMessageBox>
#include <QWidget>
#include <QDesktopServices>
#include <QUrl>
#include <QScrollArea>
#include <QGroupBox>
#include <QComboBox>
#include <QMap>
#include <QLabel>
#include <QStringList>
#include <QInputDialog>
#include <QLineEdit>
#include <QSettings> 
#include <QFormLayout> 
#include <QDialogButtonBox>
#include <QPrinter>       
#include <QPrintDialog>
#include <QTextDocument>
#include <QTextCharFormat>

RecordsWindow::RecordsWindow(IDatabaseManager *db, const QString &category, QWidget *parent)
    : QDialog(parent), dbManager(db), currentCategory(category)
{
    setWindowTitle(category.toUpper() + " - ALL BOOKINGS");
    setWindowFlags(Qt::Window | Qt::WindowMinimizeButtonHint | Qt::WindowMaximizeButtonHint | Qt::WindowCloseButtonHint);
    resize(1500, 800); 
    
    setupUI();
    loadData();
}

RecordsWindow::~RecordsWindow() {}

void RecordsWindow::setupUI()
{
    bool isDark = QSettings("StudentWorkshopApp", "ThemeSettings").value("darkMode", false).toBool();
    QString bg = isDark ? "#0B1120" : "#F4F7FC";
    QString text = isDark ? "#F8FAFC" : "#1E293B";
    QString cardBg = isDark ? "#1E293B" : "#FFFFFF";
    QString border = isDark ? "#334155" : "#CBD5E1";
    QString subText = isDark ? "#94A3B8" : "#475569";

    this->setStyleSheet(QString("QDialog { background-color: %1; }").arg(bg));

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    
    QHBoxLayout *topBar = new QHBoxLayout();
    QLabel *titleLabel = new QLabel("Booking Management: " + currentCategory.toUpper(), this);
    titleLabel->setStyleSheet(QString("font-size: 20px; font-weight: bold; color: %1;").arg(text));
    
    QPushButton *printAllBtn = new QPushButton("🖨 Print All", this);
    printAllBtn->setStyleSheet("background-color: #0F172A; color: white; font-weight: bold; border-radius: 6px; padding: 10px 20px; font-size: 14px;");
    connect(printAllBtn, &QPushButton::clicked, this, &RecordsWindow::printAllRecords);
    
    QPushButton *settingsBtn = new QPushButton("⚙ Settings", this);
    settingsBtn->setStyleSheet("background-color: #334155; color: white; font-weight: bold; border-radius: 6px; padding: 10px 20px; font-size: 14px;");
    connect(settingsBtn, &QPushButton::clicked, this, &RecordsWindow::openSettingsDialog);
    
    topBar->addWidget(titleLabel);
    topBar->addStretch();
    topBar->addWidget(printAllBtn);
    topBar->addWidget(settingsBtn);
    
    mainLayout->addLayout(topBar);

    QFrame *calFrame = new QFrame(this);
    calFrame->setStyleSheet(QString("QFrame { background-color: %1; border-radius: 10px; border: 1px solid %2; margin-bottom: 10px; }").arg(cardBg, border));
    
    QVBoxLayout *calLayout = new QVBoxLayout(calFrame);
    calLayout->setContentsMargins(15, 15, 15, 15);
    calLayout->setSpacing(15);

    QHBoxLayout *calTopLayout = new QHBoxLayout();
    
    QVBoxLayout *calTextLayout = new QVBoxLayout();
    QLabel *calTitle = new QLabel("Filter by Booked Date", calFrame);
    calTitle->setStyleSheet(QString("font-size: 16px; font-weight: bold; color: %1; border: none;").arg(text));
    QLabel *calDesc = new QLabel("Select a date on the calendar to view bookings. Numbers represent Approved (Green), Pending (Orange), and Rejected (Red).", calFrame);
    calDesc->setStyleSheet(QString("font-size: 13px; color: %1; border: none;").arg(subText));
    
    calTextLayout->addWidget(calTitle);
    calTextLayout->addWidget(calDesc);

    QPushButton *showAllBtn = new QPushButton("Show All Bookings", calFrame);
    showAllBtn->setStyleSheet("background-color: #3B82F6; color: white; font-weight: bold; border-radius: 4px; padding: 10px 20px; font-size: 13px;");

    calTopLayout->addLayout(calTextLayout);
    calTopLayout->addStretch();
    calTopLayout->addWidget(showAllBtn);

    calendar = new CustomCalendarWidget(calFrame);
    calendar->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred); 
    calendar->setMinimumHeight(250); 
    
    // --- දෝෂය නිවැරදි කර අකුරු පැහැදිලිව පෙනෙන පරිදි සකසන ලද Calendar Stylesheet එක ---
    QString calStyle = isDark ? 
        "CustomCalendarWidget { background-color: #0B1120; color: #F8FAFC; }"
        "CustomCalendarWidget QWidget#qt_calendar_navigationbar { background-color: #1E293B; }"
        "CustomCalendarWidget QToolButton { color: #F8FAFC; font-weight: bold; background-color: transparent; }"
        "CustomCalendarWidget QToolButton::hover { background-color: #334155; border-radius: 4px; }"
        "CustomCalendarWidget QMenu { background-color: #1E293B; color: #F8FAFC; }"
        "CustomCalendarWidget QSpinBox { background-color: #1E293B; color: #F8FAFC; selection-background-color: #38BDF8; }"
        "CustomCalendarWidget QAbstractItemView { selection-background-color: #38BDF8; selection-color: #0B1120; background-color: #0B1120; color: #F8FAFC; border: 1px solid #334155; alternate-background-color: #1E293B; font-size: 13px; }"
        :
        "CustomCalendarWidget { background-color: #FFFFFF; color: #0F172A; }"
        "CustomCalendarWidget QWidget#qt_calendar_navigationbar { background-color: #F1F5F9; }"
        "CustomCalendarWidget QToolButton { color: #0F172A; font-weight: bold; background-color: transparent; }"
        "CustomCalendarWidget QToolButton::hover { background-color: #E2E8F0; border-radius: 4px; }"
        "CustomCalendarWidget QMenu { background-color: #FFFFFF; color: #0F172A; }"
        "CustomCalendarWidget QSpinBox { background-color: #FFFFFF; color: #0F172A; selection-background-color: #3B82F6; }"
        "CustomCalendarWidget QAbstractItemView { selection-background-color: #3B82F6; selection-color: #FFFFFF; background-color: #FFFFFF; color: #0F172A; border: 1px solid #CBD5E1; alternate-background-color: #F8FAFC; font-size: 13px; }";
    
    calendar->setStyleSheet(calStyle);

    calLayout->addLayout(calTopLayout);
    calLayout->addWidget(calendar);

    mainLayout->addWidget(calFrame);

    connect(calendar, &QCalendarWidget::clicked, this, &RecordsWindow::filterByDate);
    connect(showAllBtn, &QPushButton::clicked, this, &RecordsWindow::clearFilter);

    QScrollArea *scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setStyleSheet("QScrollArea { border: none; background-color: transparent; }");
    
    QWidget *scrollWidget = new QWidget(scrollArea);
    scrollWidget->setStyleSheet("background-color: transparent;");
    
    scrollLayout = new QVBoxLayout(scrollWidget);
    scrollLayout->setSpacing(20);
    
    scrollArea->setWidget(scrollWidget);
    mainLayout->addWidget(scrollArea);
}

void RecordsWindow::filterByDate(const QDate &date) { filterDate = date; loadData(); }
void RecordsWindow::clearFilter() { filterDate = QDate(); calendar->setSelectedDate(QDate::currentDate()); loadData(); }

void RecordsWindow::updateCalendarHighlights()
{
    if (currentCategory.isEmpty()) return;
    QTextCharFormat defaultFormat;
    calendar->setDateTextFormat(QDate(), defaultFormat); 

    QList<StudentRecord> allRecords = dbManager->getAllRecordsForCategory(currentCategory);
    QMap<QDate, QMap<QString, int>> dateCounts;
    QList<QDate> bookedDates;

    for (const StudentRecord &r : allRecords) {
        QDate d = QDate::fromString(r.getRecordDate(), "yyyy-MM-dd");
        if (d.isValid()) {
            if (!bookedDates.contains(d)) bookedDates.append(d);
            dateCounts[d][r.getStatus()]++;
        }
    }

    bool isDark = QSettings("StudentWorkshopApp", "ThemeSettings").value("darkMode", false).toBool();
    QTextCharFormat highlightFormat;
    // Highlight colors ද වඩාත් පැහැදිලි වන ලෙස වෙනස් කර ඇත
    highlightFormat.setBackground(isDark ? QColor("#0369A1") : QColor("#BAE6FD"));
    highlightFormat.setForeground(isDark ? QColor("#F8FAFC") : QColor("#0C4A6E"));
    highlightFormat.setFontWeight(QFont::Bold);

    for (const QDate &date : bookedDates) {
        calendar->setDateTextFormat(date, highlightFormat);
    }
    calendar->setStatusCounts(dateCounts);
}

void RecordsWindow::openSettingsDialog()
{
    QDialog dialog(this);
    dialog.setWindowTitle("Admin Email Settings");
    dialog.setMinimumWidth(400);
    
    bool isDark = QSettings("StudentWorkshopApp", "ThemeSettings").value("darkMode", false).toBool();
    dialog.setStyleSheet(isDark ? "QDialog { background-color: #1E293B; color: #F8FAFC; } QLabel { font-weight: bold; } QLineEdit { background-color: #0F172A; color: #F8FAFC; padding: 5px; border: 1px solid #334155; border-radius: 4px; }"
                                : "QDialog { background-color: #FFFFFF; color: #0F172A; } QLabel { font-weight: bold; } QLineEdit { padding: 5px; border: 1px solid #CBD5E1; border-radius: 4px; }");
    
    QFormLayout form(&dialog);
    form.setSpacing(15);
    form.setContentsMargins(20, 20, 20, 20);

    QSettings settings("StudentWorkshopApp", "AdminSettings");

    QLineEdit *deanEmailInput = new QLineEdit(settings.value("dean_email", "").toString());
    deanEmailInput->setPlaceholderText("e.g. dean@university.edu");
    
    QLineEdit *depHeadEmailInput = new QLineEdit(settings.value("dep_head_email", "").toString());
    depHeadEmailInput->setPlaceholderText("e.g. head@department.edu");

    form.addRow("Dean Email:", deanEmailInput);
    form.addRow("Department Head Email:", depHeadEmailInput);

    QDialogButtonBox buttonBox(QDialogButtonBox::Save | QDialogButtonBox::Cancel, Qt::Horizontal, &dialog);
    form.addRow(&buttonBox);

    connect(&buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(&buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    if (dialog.exec() == QDialog::Accepted) {
        settings.setValue("dean_email", deanEmailInput->text().trimmed());
        settings.setValue("dep_head_email", depHeadEmailInput->text().trimmed());
        QMessageBox::information(this, "Settings Saved", "Email configurations updated successfully.");
    }
}

void RecordsWindow::printAllRecords()
{
    QList<StudentRecord> records = dbManager->getAllRecordsForCategory(currentCategory);
    if(records.isEmpty()) {
        QMessageBox::information(this, "Empty", "No records available to print.");
        return;
    }

    QPrinter printer(QPrinter::HighResolution);
    printer.setPageOrientation(QPageLayout::Landscape); 
    
    QPrintDialog printDialog(&printer, this);
    if (printDialog.exec() == QDialog::Accepted) {
        QTextDocument doc;
        QString html = "<h1 style='color: #1E293B; text-align: center;'>" + currentCategory.toUpper() + " WORKSHOP - ALL BOOKINGS</h1>";
        html += "<hr><table border='1' cellspacing='0' cellpadding='8' width='100%' style='border-collapse: collapse;'>";
        html += "<tr bgcolor='#CBD5E1'><th align='left'>Booking ID</th><th align='left'>Student Name</th><th align='left'>Index No</th><th align='left'>Booked Date</th><th align='left'>Time Slot</th><th align='left'>Status</th><th align='left'>Admin Remarks</th></tr>";
        
        for(const StudentRecord &r : records) {
            html += "<tr>";
            html += "<td>" + r.getBookingId() + "</td>";
            html += "<td>" + r.getName() + "</td>";
            html += "<td>" + r.getIndexNo() + "</td>";
            html += "<td>" + r.getRecordDate() + "</td>";
            html += "<td>" + r.getTimeSlots() + "</td>";
            
            QString statusColor = "#000000";
            if (r.getStatus() == "Approve") statusColor = "green";
            else if (r.getStatus() == "Reject") statusColor = "red";
            else statusColor = "orange";
            
            html += "<td style='color:" + statusColor + "; font-weight:bold;'>" + r.getStatus() + "</td>";
            html += "<td>" + r.getAdminRemarks() + "</td>";
            html += "</tr>";
        }
        
        html += "</table>";
        html += "<br><p style='font-size:10px; color:#64748B;'>Printed on: " + QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss") + "</p>";
        
        doc.setHtml(html);
        doc.print(&printer);
    }
}

void RecordsWindow::printSingleRecord(const StudentRecord &record)
{
    QPrinter printer(QPrinter::HighResolution);
    QPrintDialog printDialog(&printer, this);
    
    if (printDialog.exec() == QDialog::Accepted) {
        QTextDocument doc;
        
        QString html = "<h1 style='color: #1E293B; text-align: center;'>BOOKING CONFIRMATION</h1><hr><br>";
        html += "<table width='100%' cellpadding='10' style='font-size: 14px;'>";
        html += "<tr><td width='30%'><b>Booking ID:</b></td><td>" + record.getBookingId() + "</td></tr>";
        html += "<tr><td bgcolor='#F1F5F9'><b>Workshop:</b></td><td bgcolor='#F1F5F9'>" + record.getCategory().toUpper() + "</td></tr>";
        html += "<tr><td><b>Student Name:</b></td><td>" + record.getName() + "</td></tr>";
        html += "<tr><td bgcolor='#F1F5F9'><b>Index Number:</b></td><td bgcolor='#F1F5F9'>" + record.getIndexNo() + "</td></tr>";
        html += "<tr><td><b>Department:</b></td><td>" + record.getDepartment() + "</td></tr>";
        html += "<tr><td bgcolor='#F1F5F9'><b>Email Address:</b></td><td bgcolor='#F1F5F9'>" + record.getEmail() + "</td></tr>";
        html += "<tr><td><b>Booked Date:</b></td><td>" + record.getRecordDate() + "</td></tr>";
        html += "<tr><td bgcolor='#F1F5F9'><b>Time Slot(s):</b></td><td bgcolor='#F1F5F9'>" + record.getTimeSlots() + "</td></tr>";
        html += "<tr><td><b>Description:</b></td><td>" + record.getDescription() + "</td></tr>";
        
        QString statusColor = "#000000";
        if (record.getStatus() == "Approve") statusColor = "green";
        else if (record.getStatus() == "Reject") statusColor = "red";
        else statusColor = "orange";
        
        html += "<tr><td bgcolor='#F1F5F9'><b>Current Status:</b></td><td bgcolor='#F1F5F9' style='color:" + statusColor + "; font-weight:bold;'>" + record.getStatus() + "</td></tr>";
        html += "<tr><td><b>Admin Remarks:</b></td><td>" + record.getAdminRemarks() + "</td></tr>";
        html += "<tr><td bgcolor='#F1F5F9'><b>Submitted On:</b></td><td bgcolor='#F1F5F9'>" + record.getCreatedAt() + "</td></tr>";
        html += "</table><br><hr>";
        html += "<p align='center' style='font-size:12px; color:#64748B;'>This is a system-generated document.<br>Student Workshop Management System</p>";
        
        doc.setHtml(html);
        doc.print(&printer);
    }
}

void RecordsWindow::loadData()
{
    QLayoutItem *child;
    while ((child = scrollLayout->takeAt(0)) != nullptr) {
        if(child->widget()) delete child->widget();
        delete child;
    }

    bool isDark = QSettings("StudentWorkshopApp", "ThemeSettings").value("darkMode", false).toBool();
    QString cardBg = isDark ? "#1E293B" : "#FFFFFF";
    QString text = isDark ? "#F8FAFC" : "#0F172A";
    QString border = isDark ? "#334155" : "#CBD5E1";
    QString altBg = isDark ? "#0F172A" : "#F8FAFC";
    QString headerBg = isDark ? "#0F172A" : "#E2E8F0";

    QList<StudentRecord> allRecords = dbManager->getAllRecordsForCategory(currentCategory);
    QList<StudentRecord> recordsToDisplay;

    if (filterDate.isValid()) {
        for (const StudentRecord &r : allRecords) {
            if (r.getRecordDate() == filterDate.toString("yyyy-MM-dd")) recordsToDisplay.append(r);
        }
    } else {
        recordsToDisplay = allRecords; 
    }

    QMap<QString, QList<StudentRecord>> groupedRecords;
    for (const StudentRecord &r : recordsToDisplay) {
        QString submittedDate = r.getCreatedAt().left(10); 
        groupedRecords[submittedDate].append(r);
    }

    if (groupedRecords.isEmpty()) {
        QString msg = filterDate.isValid() ? "No bookings found for the selected date." : "No bookings found for this category.";
        QLabel *emptyLabel = new QLabel(msg, this);
        emptyLabel->setAlignment(Qt::AlignCenter);
        emptyLabel->setStyleSheet(QString("font-size: 16px; color: %1; margin-top: 50px;").arg(text));
        scrollLayout->addWidget(emptyLabel);
        scrollLayout->addStretch();
        updateCalendarHighlights(); 
        return;
    }

    QStringList dateKeys = groupedRecords.keys();
    for (int k = dateKeys.size() - 1; k >= 0; --k) {
        QString date = dateKeys[k];
        QList<StudentRecord> records = groupedRecords[date];

        QGroupBox *groupBox = new QGroupBox(" Date: " + date, this);
        groupBox->setStyleSheet(QString("QGroupBox { font-size: 16px; font-weight: bold; color: %1; border: 2px solid %2; border-radius: 8px; margin-top: 15px; } QGroupBox::title { subcontrol-origin: margin; subcontrol-position: top left; padding: 0 10px; }").arg(isDark?"#38BDF8":"#0F172A", border));
        
        QVBoxLayout *gLayout = new QVBoxLayout(groupBox);
        gLayout->setContentsMargins(15, 25, 15, 15);

        QTableWidget *table = new QTableWidget(records.size(), 12, groupBox);
        table->setHorizontalHeaderLabels({"Booking ID", "Name", "Index", "Email", "PDF", "Time Slot", "Description", "Submitted", "Actions", "Status", "Admin Remarks", "DB ID"});
        table->hideColumn(11); 
        
        table->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
        table->horizontalHeader()->setSectionResizeMode(6, QHeaderView::Stretch); 
        table->horizontalHeader()->setSectionResizeMode(10, QHeaderView::Stretch); 
        table->setWordWrap(true);
        table->setEditTriggers(QAbstractItemView::NoEditTriggers);
        table->setAlternatingRowColors(true);
        table->setSelectionBehavior(QAbstractItemView::SelectRows);
        
        table->setStyleSheet(QString("QTableWidget { background-color: %1; alternate-background-color: %2; border: none; color: %3; } QHeaderView::section { background-color: %4; color: %3; font-weight: bold; border: 1px solid %5; padding: 5px; }")
                             .arg(cardBg, altBg, text, headerBg, border));
        
        for (int i = 0; i < records.size(); ++i) {
            table->setItem(i, 0, new QTableWidgetItem(records[i].getBookingId()));
            table->setItem(i, 1, new QTableWidgetItem(records[i].getName()));
            table->setItem(i, 2, new QTableWidgetItem(records[i].getIndexNo()));
            table->setItem(i, 3, new QTableWidgetItem(records[i].getEmail()));
            
            QString pdfPath = records[i].getPdfPath();
            if(!pdfPath.isEmpty()) {
                QPushButton *pdfBtn = new QPushButton("Open");
                pdfBtn->setStyleSheet("color: #38BDF8; font-weight: bold; border: 1px solid #38BDF8; border-radius: 4px; padding: 4px;");
                connect(pdfBtn, &QPushButton::clicked, [pdfPath](){ QDesktopServices::openUrl(QUrl::fromLocalFile(pdfPath)); });
                table->setCellWidget(i, 4, pdfBtn);
            } else {
                table->setItem(i, 4, new QTableWidgetItem("No PDF"));
            }
            
            table->setItem(i, 5, new QTableWidgetItem(records[i].getTimeSlots()));
            table->setItem(i, 6, new QTableWidgetItem(records[i].getDescription()));
            table->setItem(i, 7, new QTableWidgetItem(records[i].getCreatedAt()));
            
            QWidget *actionWidget = new QWidget();
            QGridLayout *actLayout = new QGridLayout(actionWidget);
            actLayout->setContentsMargins(2, 2, 2, 2);
            actLayout->setSpacing(5);
            
            QPushButton *changeBtn = new QPushButton("Edit");
            changeBtn->setStyleSheet("background-color: #3B82F6; color: white; font-weight: bold; border-radius: 4px; padding: 4px;");
            QPushButton *deleteBtn = new QPushButton("Delete");
            deleteBtn->setStyleSheet("background-color: #64748B; color: white; font-weight: bold; border-radius: 4px; padding: 4px;");
            QPushButton *approveBtn = new QPushButton("Approve");
            approveBtn->setStyleSheet("background-color: #10B981; color: white; font-weight: bold; border-radius: 4px; padding: 4px;");
            QPushButton *rejectBtn = new QPushButton("Reject");
            rejectBtn->setStyleSheet("background-color: #EF4444; color: white; font-weight: bold; border-radius: 4px; padding: 4px;");
            QPushButton *deanAppBtn = new QPushButton("Dean Apprv");
            deanAppBtn->setStyleSheet("background-color: #0284C7; color: white; font-weight: bold; border-radius: 4px; padding: 4px;");
            QPushButton *depHeadAppBtn = new QPushButton("Dep Head");
            depHeadAppBtn->setStyleSheet("background-color: #0D9488; color: white; font-weight: bold; border-radius: 4px; padding: 4px;");
            QPushButton *printBtn = new QPushButton("🖨 Print");
            printBtn->setStyleSheet(QString("background-color: %1; color: %2; font-weight: bold; border-radius: 4px; padding: 4px;").arg(isDark?"#F8FAFC":"#0F172A", isDark?"#0F172A":"#F8FAFC"));
            
            actLayout->addWidget(changeBtn, 0, 0);
            actLayout->addWidget(deleteBtn, 1, 0);
            actLayout->addWidget(approveBtn, 0, 1);
            actLayout->addWidget(rejectBtn, 1, 1);
            actLayout->addWidget(deanAppBtn, 0, 2);
            actLayout->addWidget(depHeadAppBtn, 1, 2);
            actLayout->addWidget(printBtn, 2, 0, 1, 3);
            
            table->setCellWidget(i, 8, actionWidget);
            
            QComboBox *statusCombo = new QComboBox();
            statusCombo->addItems({"Pending", "Approve", "Reject"});
            statusCombo->setCurrentText(records[i].getStatus());
            
            auto updateComboColor = [statusCombo]() {
                if(statusCombo->currentText() == "Approve") statusCombo->setStyleSheet("color: white; background-color: #10B981; font-weight: bold; padding: 4px; border-radius: 4px;");
                else if(statusCombo->currentText() == "Reject") statusCombo->setStyleSheet("color: white; background-color: #EF4444; font-weight: bold; padding: 4px; border-radius: 4px;");
                else statusCombo->setStyleSheet("color: black; background-color: #F59E0B; font-weight: bold; padding: 4px; border-radius: 4px;");
            };
            updateComboColor();
            connect(statusCombo, &QComboBox::currentTextChanged, [this, id = records[i].getId(), updateComboColor](const QString &text){
                dbManager->updateRecordStatus(id, text);
                updateComboColor();
                updateCalendarHighlights(); 
            });
            table->setCellWidget(i, 9, statusCombo);
            
            QWidget *remarkWidget = new QWidget();
            QHBoxLayout *remLayout = new QHBoxLayout(remarkWidget);
            remLayout->setContentsMargins(2, 2, 2, 2);
            remLayout->setSpacing(5);
            
            QLineEdit *remarkInput = new QLineEdit(records[i].getAdminRemarks());
            remarkInput->setPlaceholderText("Type admin remarks...");
            remarkInput->setStyleSheet(QString("padding: 4px; border: 1px solid %1; border-radius: 4px; font-size: 13px; background-color: %2; color: %3;").arg(border, altBg, text));
            
            QPushButton *saveRemBtn = new QPushButton("Save");
            saveRemBtn->setStyleSheet("background-color: #475569; color: white; border-radius: 4px; padding: 4px 10px; font-weight: bold;");
            
            remLayout->addWidget(remarkInput);
            remLayout->addWidget(saveRemBtn);
            table->setCellWidget(i, 10, remarkWidget);
            
            connect(saveRemBtn, &QPushButton::clicked, [this, id = records[i].getId(), remarkInput]() {
                dbManager->updateAdminRemarks(id, remarkInput->text());
                QMessageBox::information(this, "Saved", "Admin remarks saved successfully.");
            });

            connect(changeBtn, &QPushButton::clicked, [this, record = records[i]]() { emit editRequested(record); this->accept(); });
            connect(deleteBtn, &QPushButton::clicked, [this, id = records[i].getId()]() {
                if (QMessageBox::question(this, "Confirm", "Permanently delete this record without passcode?") == QMessageBox::Yes) {
                    dbManager->deleteRecord(id); loadData(); emit databaseUpdated(); 
                }
            });
            connect(printBtn, &QPushButton::clicked, [this, record = records[i]]() { printSingleRecord(record); });
        }
        
        table->resizeRowsToContents(); 
        int totalHeight = table->horizontalHeader()->height() + 2;
        for (int i = 0; i < table->rowCount(); ++i) { totalHeight += table->rowHeight(i); }
        table->setMinimumHeight(totalHeight + 10);
        
        gLayout->addWidget(table);
        scrollLayout->addWidget(groupBox);
    }
    
    updateCalendarHighlights(); 
    scrollLayout->addStretch();
}