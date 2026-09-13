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
    resize(1750, 800); 
    setStyleSheet("QDialog { background-color: #F4F7FC; }");

    setupUI();
    loadData();
}

RecordsWindow::~RecordsWindow() {}

void RecordsWindow::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    
    QHBoxLayout *topBar = new QHBoxLayout();
    
    QLabel *titleLabel = new QLabel("Booking Management: " + currentCategory.toUpper(), this);
    titleLabel->setStyleSheet("font-size: 20px; font-weight: bold; color: #1E293B;");
    
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
    calFrame->setStyleSheet("QFrame { background-color: #FFFFFF; border-radius: 10px; border: 1px solid #CBD5E1; margin-bottom: 10px; }");
    
    QVBoxLayout *calLayout = new QVBoxLayout(calFrame);
    calLayout->setContentsMargins(15, 15, 15, 15);
    calLayout->setSpacing(15);

    QHBoxLayout *calTopLayout = new QHBoxLayout();
    
    QVBoxLayout *calTextLayout = new QVBoxLayout();
    QLabel *calTitle = new QLabel("Filter by Booked Date", calFrame);
    calTitle->setStyleSheet("font-size: 16px; font-weight: bold; color: #1E293B; border: none;");
    QLabel *calDesc = new QLabel("Select a date on the calendar to view bookings. The numbers below each date represent Approved (Green), Pending (Orange), and Rejected (Red) bookings.", calFrame);
    calDesc->setStyleSheet("font-size: 13px; color: #475569; border: none;");
    
    calTextLayout->addWidget(calTitle);
    calTextLayout->addWidget(calDesc);

    QPushButton *showAllBtn = new QPushButton("Show All Bookings", calFrame);
    showAllBtn->setStyleSheet("background-color: #3B82F6; color: white; font-weight: bold; border-radius: 4px; padding: 10px 20px; font-size: 13px;");

    calTopLayout->addLayout(calTextLayout);
    calTopLayout->addStretch();
    calTopLayout->addWidget(showAllBtn);

    // --- මෙහි CustomCalendarWidget භාවිතා කර ඇත ---
    calendar = new CustomCalendarWidget(calFrame);
    calendar->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred); 
    calendar->setMinimumHeight(250); 
    calendar->setStyleSheet("CustomCalendarWidget QWidget { alternate-background-color: #F1F5F9; border: none; font-size: 13px; }");

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

void RecordsWindow::filterByDate(const QDate &date)
{
    filterDate = date;
    loadData(); 
}

void RecordsWindow::clearFilter()
{
    filterDate = QDate(); 
    calendar->setSelectedDate(QDate::currentDate()); 
    loadData();
}

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

    QTextCharFormat highlightFormat;
    highlightFormat.setBackground(QColor("#E0F2FE"));
    highlightFormat.setFontWeight(QFont::Bold);

    for (const QDate &date : bookedDates) {
        calendar->setDateTextFormat(date, highlightFormat);
    }

    // Custom Calendar එකට දත්ත යැවීම
    calendar->setStatusCounts(dateCounts);
}

void RecordsWindow::openSettingsDialog()
{
    QDialog dialog(this);
    dialog.setWindowTitle("Admin Email Settings");
    dialog.setMinimumWidth(400);
    dialog.setStyleSheet("QDialog { background-color: #FFFFFF; } QLabel { font-weight: bold; } QLineEdit { padding: 5px; border: 1px solid #CBD5E1; border-radius: 4px; }");
    
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

    QList<StudentRecord> allRecords = dbManager->getAllRecordsForCategory(currentCategory);
    QList<StudentRecord> recordsToDisplay;

    if (filterDate.isValid()) {
        for (const StudentRecord &r : allRecords) {
            if (r.getRecordDate() == filterDate.toString("yyyy-MM-dd")) {
                recordsToDisplay.append(r);
            }
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
        emptyLabel->setStyleSheet("font-size: 16px; color: #64748B; margin-top: 50px;");
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
        groupBox->setStyleSheet("QGroupBox { font-size: 16px; font-weight: bold; color: #0F172A; border: 2px solid #CBD5E1; border-radius: 8px; margin-top: 15px; } QGroupBox::title { subcontrol-origin: margin; subcontrol-position: top left; padding: 0 10px; }");
        
        QVBoxLayout *gLayout = new QVBoxLayout(groupBox);
        gLayout->setContentsMargins(15, 25, 15, 15);

        QPushButton *emailAllBtn = new QPushButton(" Email All Records Submitted on " + date + " to ppas.rashmika");
        emailAllBtn->setStyleSheet("background-color: #8B5CF6; color: white; padding: 10px; border-radius: 5px; font-weight: bold; margin-bottom: 10px;");
        gLayout->addWidget(emailAllBtn);
        
        connect(emailAllBtn, &QPushButton::clicked, [this, date, records]() {
            bool ok;
            QString recommendation = QInputDialog::getMultiLineText(this, "Admin Recommendation", "Enter Admin Recommendation for all these bookings (Optional):", "", &ok);
            if (!ok) return;

            QString subject = "All Bookings Submitted for " + currentCategory + " on " + date;
            QString body = "Here is the summary of all bookings submitted on " + date + ":\n\n";
            
            if (!recommendation.trimmed().isEmpty()) {
                body += ">> ADMIN RECOMMENDATION:\n" + recommendation + "\n\n";
                body += "--------------------------------------\n\n";
            }
            
            for (const StudentRecord &r : records) {
                body += "Booking ID: " + r.getBookingId() + "\n";
                body += "Name: " + r.getName() + " (Index: " + r.getIndexNo() + ")\n";
                body += "Time Slot: " + r.getTimeSlots() + "\n";
                body += "Status: " + r.getStatus() + "\n";
                body += "--------------------------------------\n";
            }
            
            QString link = QString("mailto:ppas.rashmika?subject=%1&body=%2").arg(QString(QUrl::toPercentEncoding(subject))).arg(QString(QUrl::toPercentEncoding(body)));
            QDesktopServices::openUrl(QUrl(link, QUrl::TolerantMode));
        });

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
        table->setStyleSheet("QTableWidget { background-color: white; alternate-background-color: #F8FAFC; border: none; } QHeaderView::section { background-color: #E2E8F0; font-weight: bold; }");
        
        for (int i = 0; i < records.size(); ++i) {
            table->setItem(i, 0, new QTableWidgetItem(records[i].getBookingId()));
            table->setItem(i, 1, new QTableWidgetItem(records[i].getName()));
            table->setItem(i, 2, new QTableWidgetItem(records[i].getIndexNo()));
            table->setItem(i, 3, new QTableWidgetItem(records[i].getEmail()));
            
            QString pdfPath = records[i].getPdfPath();
            if(!pdfPath.isEmpty()) {
                QPushButton *pdfBtn = new QPushButton("Open");
                pdfBtn->setStyleSheet("color: #2563EB; font-weight: bold; border: 1px solid #2563EB; border-radius: 4px; padding: 4px;");
                connect(pdfBtn, &QPushButton::clicked, [pdfPath](){
                    QDesktopServices::openUrl(QUrl::fromLocalFile(pdfPath));
                });
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
            printBtn->setStyleSheet("background-color: #0F172A; color: white; font-weight: bold; border-radius: 4px; padding: 4px;");
            
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
                updateCalendarHighlights(); // Status වෙනස් වූ විට Calendar එක Update වීම
            });
            table->setCellWidget(i, 9, statusCombo);
            
            QWidget *remarkWidget = new QWidget();
            QHBoxLayout *remLayout = new QHBoxLayout(remarkWidget);
            remLayout->setContentsMargins(2, 2, 2, 2);
            remLayout->setSpacing(5);
            
            QLineEdit *remarkInput = new QLineEdit(records[i].getAdminRemarks());
            remarkInput->setPlaceholderText("Type admin remarks...");
            remarkInput->setStyleSheet("padding: 4px; border: 1px solid #CBD5E1; border-radius: 4px; font-size: 13px;");
            
            QPushButton *saveRemBtn = new QPushButton("Save");
            saveRemBtn->setStyleSheet("background-color: #475569; color: white; border-radius: 4px; padding: 4px 10px; font-weight: bold;");
            
            remLayout->addWidget(remarkInput);
            remLayout->addWidget(saveRemBtn);
            table->setCellWidget(i, 10, remarkWidget);
            
            connect(saveRemBtn, &QPushButton::clicked, [this, id = records[i].getId(), remarkInput]() {
                dbManager->updateAdminRemarks(id, remarkInput->text());
                QMessageBox::information(this, "Saved", "Admin remarks saved successfully.");
            });

            connect(changeBtn, &QPushButton::clicked, [this, record = records[i]]() {
                emit editRequested(record);
                this->accept(); 
            });
            
            connect(deleteBtn, &QPushButton::clicked, [this, id = records[i].getId()]() {
                if (QMessageBox::question(this, "Confirm", "Permanently delete this record without passcode?") == QMessageBox::Yes) {
                    if(dbManager->deleteRecord(id)) {
                        QMessageBox::information(this, "Deleted", "Record deleted successfully!");
                        loadData(); 
                        emit databaseUpdated(); 
                    }
                }
            });

            connect(approveBtn, &QPushButton::clicked, [this, record = records[i], statusCombo]() {
                bool ok;
                QString recommendation = QInputDialog::getMultiLineText(this, "Admin Recommendation", "Enter Admin Recommendation for this approval (Optional):", "", &ok);
                if (!ok) return;

                dbManager->updateRecordStatus(record.getId(), "Approve");
                statusCombo->setCurrentText("Approve"); 

                QString studentEmail = record.getEmail();
                if (studentEmail.trimmed().isEmpty()) {
                    QMessageBox::warning(this, "Missing Email", "No email address found for this booking.");
                    return;
                }
                QString subject = "Booking Approved - " + record.getBookingId();
                QString body = "Dear " + record.getName() + ",\n\nYour booking ID " + record.getBookingId() + " has been APPROVED.\n\n";
                if (!recommendation.trimmed().isEmpty()) body += ">> ADMIN NOTES:\n" + recommendation + "\n\n";
                
                QString link = QString("mailto:%1?subject=%2&body=%3").arg(studentEmail).arg(QString(QUrl::toPercentEncoding(subject))).arg(QString(QUrl::toPercentEncoding(body)));
                QDesktopServices::openUrl(QUrl(link, QUrl::TolerantMode));
            });

            connect(rejectBtn, &QPushButton::clicked, [this, record = records[i], statusCombo]() {
                bool ok;
                QString recommendation = QInputDialog::getMultiLineText(this, "Admin Recommendation", "Enter reason for rejection (Optional):", "", &ok);
                if (!ok) return;

                dbManager->updateRecordStatus(record.getId(), "Reject");
                statusCombo->setCurrentText("Reject"); 

                QString studentEmail = record.getEmail();
                if (studentEmail.trimmed().isEmpty()) {
                    QMessageBox::warning(this, "Missing Email", "No email address found for this booking.");
                    return;
                }
                QString subject = "Booking Rejected - " + record.getBookingId();
                QString body = "Dear " + record.getName() + ",\n\nYour booking ID " + record.getBookingId() + " has been REJECTED.\n\n";
                if (!recommendation.trimmed().isEmpty()) body += ">> REASON:\n" + recommendation + "\n\n";
                
                QString link = QString("mailto:%1?subject=%2&body=%3").arg(studentEmail).arg(QString(QUrl::toPercentEncoding(subject))).arg(QString(QUrl::toPercentEncoding(body)));
                QDesktopServices::openUrl(QUrl(link, QUrl::TolerantMode));
            });

            connect(deanAppBtn, &QPushButton::clicked, [this, record = records[i]]() {
                QSettings settings("StudentWorkshopApp", "AdminSettings");
                QString deanEmail = settings.value("dean_email", "").toString();
                if (deanEmail.isEmpty()) { QMessageBox::warning(this, "Error", "Dean's Email is not configured in Settings."); return; }
                QString subject = "Approval Request - " + record.getBookingId();
                QString body = "Dear Dean,\nPlease review booking ID: " + record.getBookingId();
                QString link = QString("mailto:%1?subject=%2&body=%3").arg(deanEmail).arg(QString(QUrl::toPercentEncoding(subject))).arg(QString(QUrl::toPercentEncoding(body)));
                QDesktopServices::openUrl(QUrl(link, QUrl::TolerantMode));
            });

            connect(depHeadAppBtn, &QPushButton::clicked, [this, record = records[i]]() {
                QSettings settings("StudentWorkshopApp", "AdminSettings");
                QString depHeadEmail = settings.value("dep_head_email", "").toString();
                if (depHeadEmail.isEmpty()) { QMessageBox::warning(this, "Error", "Department Head's Email is not configured."); return; }
                QString subject = "Approval Request - " + record.getBookingId();
                QString body = "Dear Department Head,\nPlease review booking ID: " + record.getBookingId();
                QString link = QString("mailto:%1?subject=%2&body=%3").arg(depHeadEmail).arg(QString(QUrl::toPercentEncoding(subject))).arg(QString(QUrl::toPercentEncoding(body)));
                QDesktopServices::openUrl(QUrl(link, QUrl::TolerantMode));
            });
            
            connect(printBtn, &QPushButton::clicked, [this, record = records[i]]() {
                printSingleRecord(record);
            });
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