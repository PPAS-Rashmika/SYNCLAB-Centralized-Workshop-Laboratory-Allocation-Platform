#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "recordswindow.h"
#include <QMessageBox>
#include <QTextCharFormat>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QListView>
#include <QRandomGenerator>
#include <QInputDialog>
#include <QBrush>
#include <QColor>
#include <QDateTime>
#include <QFileDialog>
// අලුතින් එක් කළ Headers (File Management සඳහා)
#include <QFile>
#include <QDir>
#include <QFileInfo>
#include <QCoreApplication>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    isPasscodeBypassed = false; 

    dbManager = new SqliteDatabaseManager("student_data.db");
    if (!dbManager->createTable()) {
        QMessageBox::critical(this, "DB Error", dbManager->getLastError());
    }

    setupDynamicUI();
    
    connect(ui->backBtn, &QPushButton::clicked, this, &MainWindow::goBack);
    connect(ui->viewRecordsBtn, &QPushButton::clicked, this, &MainWindow::showRecordsWindow);
    connect(ui->calendar, &QCalendarWidget::clicked, this, &MainWindow::dateClicked);
    connect(ui->submitBtn, &QPushButton::clicked, this, &MainWindow::submitData);
    connect(ui->updateBtn, &QPushButton::clicked, this, &MainWindow::updateData);
    connect(ui->deleteBtn, &QPushButton::clicked, this, &MainWindow::deleteData);
    connect(ui->dateInput, &QDateEdit::dateChanged, this, &MainWindow::updateAvailableTimeSlots);
    
    connect(ui->browsePdfBtn, &QPushButton::clicked, this, [this](){
        QString filePath = QFileDialog::getOpenFileName(this, "Select PDF File", "", "PDF Documents (*.pdf)");
        if (!filePath.isEmpty()) {
            ui->pdfInput->setText(filePath);
        }
    });
}

MainWindow::~MainWindow()
{
    delete dbManager;
    delete ui;
}

void MainWindow::setupDynamicUI()
{
    QStringList workshops = {"Automotive", "Robotics", "Mechanical", "Painting", "Welding"};
    for(int i = 0; i < workshops.size(); i++) {
        ui->gridLayout->addWidget(createCard(workshops[i]), 0, i);
    }

    QStringList labs = {"Microscope", "Chemistry", "Coding", "Test Tubes", "Circuitry"};
    for(int i = 0; i < labs.size(); i++) {
        ui->gridLayout->addWidget(createCard(labs[i]), 1, i);
    }

    QStandardItemModel *timeModel = new QStandardItemModel(4, 1, this);
    QStringList timeSlotsList = {"8.30 am - 10.30 am", "10.30 am - 12.30 pm", "1.30 pm - 3.30 pm", "3.30 pm - 5.30 pm"};
    
    for (int i = 0; i < timeSlotsList.size(); ++i) {
        QStandardItem *item = new QStandardItem(timeSlotsList[i]);
        item->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
        item->setData(Qt::Unchecked, Qt::CheckStateRole);
        timeModel->setItem(i, 0, item);
    }
    ui->timeSlotInput->setModel(timeModel);
    QListView* listView = new QListView(ui->timeSlotInput);
    ui->timeSlotInput->setView(listView);

    QPushButton *newBookingBtn = new QPushButton("NEW BOOKING", this);
    newBookingBtn->setStyleSheet("background-color: #28A745; color: white; padding: 15px; border-radius: 5px; font-weight: bold;");
    ui->actionButtonsLayout->insertWidget(0, newBookingBtn);
    
    connect(newBookingBtn, &QPushButton::clicked, this, [this](){
        QDate cd = ui->dateInput->date();
        currentRecord = StudentRecord();
        clearUI();
        ui->dateInput->setDate(cd); 
        updateAvailableTimeSlots(cd); 
        ui->submitBtn->show();
        ui->updateBtn->hide();
        ui->deleteBtn->hide();
    });
}

QWidget* MainWindow::createCard(const QString &title)
{
    QFrame *card = new QFrame(this);
    card->setFixedSize(160, 200);
    card->setStyleSheet("QFrame { background-color: white; border-radius: 10px; border: 1px solid #D0D0D0; } QFrame:hover { border: 2px solid #0055FF; }");
    QVBoxLayout *layout = new QVBoxLayout(card);
    
    QLabel *imgBox = new QLabel(card);
    imgBox->setStyleSheet("background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #0044CC, stop:1 #0088FF); border-radius: 8px;");
    imgBox->setMinimumHeight(100);
    
    QLabel *nameTag = new QLabel(title, card);
    nameTag->setAlignment(Qt::AlignCenter);
    nameTag->setStyleSheet("font-weight: bold; color: #333; border: none;");

    QPushButton *btn = new QPushButton("SELECT", card);
    btn->setStyleSheet("background-color: #E6F0FF; color: #0055FF; border-radius: 5px; padding: 5px; font-weight: bold;");
    
    connect(btn, &QPushButton::clicked, [this, title](){
        openSecondInterface(title);
    });

    layout->addWidget(imgBox);
    layout->addWidget(nameTag);
    layout->addWidget(btn);
    return card;
}

void MainWindow::openSecondInterface(const QString &category)
{
    currentCategory = category;
    ui->formHeader->setText(category.toUpper() + " - STUDENT INFORMATION");
    
    currentRecord = StudentRecord();
    clearUI();
    
    ui->submitBtn->show();
    ui->updateBtn->hide();
    ui->deleteBtn->hide();
    
    updateCalendarHighlights();
    ui->stackedWidget->setCurrentIndex(1);
}

void MainWindow::goBack()
{
    ui->stackedWidget->setCurrentIndex(0);
}

void MainWindow::showRecordsWindow()
{
    bool ok;
    QString enteredAdminCode = QInputDialog::getText(this, "Admin Authentication", 
                                                     "Enter Admin Passcode:", 
                                                     QLineEdit::Password, "", &ok);
    if (!ok) return; 
    
    if (enteredAdminCode != "0000000") {
        QMessageBox::critical(this, "Access Denied", "Incorrect Admin Passcode!");
        return;
    }

    RecordsWindow *recordsWin = new RecordsWindow(dbManager, currentCategory, this);
    
    connect(recordsWin, &RecordsWindow::editRequested, this, &MainWindow::loadRecordForEdit);
    connect(recordsWin, &RecordsWindow::databaseUpdated, this, &MainWindow::onDatabaseUpdatedFromPopup);
    
    recordsWin->exec(); 
    delete recordsWin;
}

void MainWindow::loadRecordForEdit(const StudentRecord &record)
{
    loadRecordIntoUI(record);
    isPasscodeBypassed = true; 
}

void MainWindow::onDatabaseUpdatedFromPopup()
{
    updateCalendarHighlights();
    clearUI();
}

void MainWindow::updateAvailableTimeSlots(const QDate &date)
{
    if (currentCategory.isEmpty()) return;

    QString dateStr = date.toString("yyyy-MM-dd");
    QStringList bookedSlots = dbManager->getBookedTimeSlotsStringList(dateStr, currentCategory, currentRecord.getId());
    
    QStandardItemModel *model = qobject_cast<QStandardItemModel*>(ui->timeSlotInput->model());
    for (int i = 0; i < model->rowCount(); ++i) {
        QStandardItem *item = model->item(i);
        if (bookedSlots.contains(item->text())) {
            item->setEnabled(false); 
            item->setCheckState(Qt::Unchecked);
            item->setForeground(QBrush(Qt::gray)); 
        } else {
            item->setEnabled(true);
            item->setForeground(QBrush(Qt::black));
        }
    }
}

void MainWindow::clearUI()
{
    ui->nameInput->clear();
    ui->indexInput->clear();
    ui->deptInput->setCurrentIndex(0);
    ui->emailInput->clear();
    ui->pdfInput->clear();
    
    QDate currentDate = QDate::currentDate();
    ui->dateInput->setDate(currentDate);
    updateAvailableTimeSlots(currentDate);
    setTimeSlotsInUI(""); 
    ui->descriptionInput->clear();
    
    isPasscodeBypassed = false; 
}

QStringList MainWindow::getSelectedTimeSlotsList()
{
    QStandardItemModel *model = qobject_cast<QStandardItemModel*>(ui->timeSlotInput->model());
    QStringList selectedSlots;
    for (int i = 0; i < model->rowCount(); ++i) {
        if (model->item(i)->checkState() == Qt::Checked && model->item(i)->isEnabled()) {
            selectedSlots << model->item(i)->text();
        }
    }
    return selectedSlots;
}

void MainWindow::setTimeSlotsInUI(const QString &timeString)
{
    QStandardItemModel *model = qobject_cast<QStandardItemModel*>(ui->timeSlotInput->model());
    for (int i = 0; i < model->rowCount(); ++i) {
        if (!timeString.isEmpty() && timeString.contains(model->item(i)->text()) && model->item(i)->isEnabled()) {
            model->item(i)->setCheckState(Qt::Checked);
        } else {
            model->item(i)->setCheckState(Qt::Unchecked);
        }
    }
}

bool MainWindow::checkPasscode()
{
    bool ok;
    QString enteredCode = QInputDialog::getText(this, "Authentication", "Enter Passcode:", QLineEdit::Password, "", &ok);
    if (!ok) return false; 
    
    if (enteredCode != currentRecord.getPasscode()) {
        QMessageBox::critical(this, "Access Denied", "Incorrect Passcode!");
        return false;
    }
    return true;
}

// ------------------------------------------------------------
// නවතම තර්කනය: PDF එක සුරක්ෂිතව System එක ඇතුළට Copy කරගැනීම
// ------------------------------------------------------------
QString MainWindow::savePdfFile(const QString &sourcePath)
{
    if (sourcePath.isEmpty()) return "";

    // Application එක ධාවනය වන තැන (Build folder එක) ඇතුළේ uploads/pdfs ෆෝල්ඩරය සෑදීම
    QString uploadDirPath = QCoreApplication::applicationDirPath() + "/uploads/pdfs";
    
    // දැනටමත් අපේ ෆෝල්ඩරේ තියෙන ගොනුවක් නම් (උදා: Update කරන විට) එය Copy කිරීම අවශ්‍ය නැත
    if (sourcePath.startsWith(uploadDirPath)) {
        return sourcePath;
    }

    QDir dir(uploadDirPath);
    if (!dir.exists()) {
        dir.mkpath("."); // ෆෝල්ඩරය නැත්නම් අලුතින් සාදයි
    }

    QFileInfo fileInfo(sourcePath);
    // අලුත් නම සෑදීම: "මිලි-තත්පර_මුල්නම.pdf" (Duplicate නම් ගැටීම වැළැක්වීමට)
    QString uniqueFileName = QString::number(QDateTime::currentMSecsSinceEpoch()) + "_" + fileInfo.fileName();
    QString destPath = dir.absoluteFilePath(uniqueFileName);

    // ගොනුව Copy කිරීම
    if (QFile::copy(sourcePath, destPath)) {
        return destPath; // සාර්ථක නම් අලුත් Path එක Return කරයි
    } else {
        return sourcePath; // යම් හෙයකින් අසමත් වුවහොත් පැරණි Path එකම යවයි
    }
}

void MainWindow::submitData()
{
    QStringList selectedSlots = getSelectedTimeSlotsList();

    if(ui->nameInput->text().isEmpty() || ui->indexInput->text().isEmpty() || selectedSlots.isEmpty()) {
        QMessageBox::warning(this, "Input Error", "Name, Index and Time Slot required!");
        return;
    }

    QString dateStr = ui->dateInput->date().toString("yyyy-MM-dd");
    
    if (!dbManager->isTimeSlotAvailable(dateStr, currentCategory, selectedSlots)) {
        QMessageBox::critical(this, "Time Slot Booked", "Selected time slot is already booked!");
        return;
    }

    // PDF එක ආරක්ෂිත ෆෝල්ඩරයකට Copy කර එහි නව ලිපිනය ලබාගැනීම
    QString securePdfPath = savePdfFile(ui->pdfInput->text());

    QString pinStr = QString::number(QRandomGenerator::global()->bounded(1000, 10000));
    QString finalTimeStr = selectedSlots.join(", ");
    QString currentTimestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm AP");

    StudentRecord newRecord;
    newRecord.setCategory(currentCategory);
    newRecord.setName(ui->nameInput->text());
    newRecord.setIndexNo(ui->indexInput->text());
    newRecord.setDepartment(ui->deptInput->currentText());
    newRecord.setEmail(ui->emailInput->text());
    newRecord.setPdfPath(securePdfPath); // ආරක්ෂිත ලිපිනය මෙහි ලබාදෙයි
    newRecord.setRecordDate(dateStr);
    newRecord.setTimeSlots(finalTimeStr);
    newRecord.setDescription(ui->descriptionInput->toPlainText());
    newRecord.setPasscode(pinStr);
    newRecord.setCreatedAt(currentTimestamp);

    if (dbManager->addRecord(newRecord)) {
        QMessageBox::information(this, "Success", QString("Data saved!\nYour Passcode is: %1").arg(pinStr));
        updateCalendarHighlights();
        currentRecord = StudentRecord();
        clearUI();
    } else {
        QMessageBox::critical(this, "Database Error", dbManager->getLastError());
    }
}

void MainWindow::updateData()
{
    if (currentRecord.getId() == -1) return;
    
    QStringList selectedSlots = getSelectedTimeSlotsList();
    if(selectedSlots.isEmpty()) return;

    QString dateStr = ui->dateInput->date().toString("yyyy-MM-dd");
    
    if (!dbManager->isTimeSlotAvailable(dateStr, currentCategory, selectedSlots, currentRecord.getId())) {
        QMessageBox::critical(this, "Time Slot Booked", "Time slot taken by another booking!");
        return;
    }

    if (!isPasscodeBypassed) {
        if (!checkPasscode()) return;
    }
    
    // Update කරන විටත් අලුත් PDF එකක් දැම්මොත් එය Copy කරගැනීම
    QString securePdfPath = savePdfFile(ui->pdfInput->text());
    
    currentRecord.setName(ui->nameInput->text());
    currentRecord.setIndexNo(ui->indexInput->text());
    currentRecord.setDepartment(ui->deptInput->currentText());
    currentRecord.setEmail(ui->emailInput->text());
    currentRecord.setPdfPath(securePdfPath); 
    currentRecord.setRecordDate(dateStr);
    currentRecord.setTimeSlots(selectedSlots.join(", "));
    currentRecord.setDescription(ui->descriptionInput->toPlainText());

    if (dbManager->updateRecord(currentRecord)) {
        QMessageBox::information(this, "Updated", "Record updated successfully!");
        updateCalendarHighlights();
        openSecondInterface(currentCategory);
    } else {
        QMessageBox::critical(this, "Database Error", dbManager->getLastError());
    }
}

void MainWindow::deleteData()
{
    if (currentRecord.getId() == -1) return;
    
    if (!isPasscodeBypassed) {
        if (!checkPasscode()) return;
    }

    if (QMessageBox::question(this, "Confirm", "Permanently delete this record?") == QMessageBox::Yes) {
        // PDF ගොනුව Delete කිරීමද මෙහිදී කළ හැක (Advanced feature), නමුත් දැනට Database Record එක පමණක් මකා දමමු.
        if (dbManager->deleteRecord(currentRecord.getId())) {
            QMessageBox::information(this, "Deleted", "Record deleted successfully!");
            updateCalendarHighlights();
            openSecondInterface(currentCategory);
        } else {
            QMessageBox::critical(this, "Database Error", dbManager->getLastError());
        }
    }
}

void MainWindow::updateCalendarHighlights()
{
    QTextCharFormat defaultFormat;
    ui->calendar->setDateTextFormat(QDate(), defaultFormat); 

    QList<QDate> bookedDates = dbManager->getBookedDates(currentCategory);
    QTextCharFormat highlightFormat;
    highlightFormat.setBackground(QColor("#B3D4FF"));
    highlightFormat.setFontWeight(QFont::Bold);

    for (const QDate &date : bookedDates) {
        ui->calendar->setDateTextFormat(date, highlightFormat);
    }
}

void MainWindow::loadRecordIntoUI(const StudentRecord &record)
{
    currentRecord = record;
    ui->nameInput->setText(currentRecord.getName());
    ui->indexInput->setText(currentRecord.getIndexNo());
    ui->deptInput->setCurrentText(currentRecord.getDepartment());
    ui->emailInput->setText(currentRecord.getEmail());
    ui->pdfInput->setText(currentRecord.getPdfPath());
    ui->dateInput->setDate(QDate::fromString(currentRecord.getRecordDate(), "yyyy-MM-dd"));
    updateAvailableTimeSlots(ui->dateInput->date()); 
    setTimeSlotsInUI(currentRecord.getTimeSlots());
    ui->descriptionInput->setText(currentRecord.getDescription());
    
    ui->submitBtn->hide();
    ui->updateBtn->show();
    ui->deleteBtn->show();
}

void MainWindow::dateClicked(const QDate &date)
{
    isPasscodeBypassed = false; 
    
    QString dateStr = date.toString("yyyy-MM-dd");
    QList<StudentRecord> records = dbManager->getRecordsByDateAndCategory(dateStr, currentCategory);
    
    if(records.isEmpty()) {
        currentRecord = StudentRecord();
        clearUI();
        ui->dateInput->setDate(date);
        ui->submitBtn->show();
        ui->updateBtn->hide();
        ui->deleteBtn->hide();
    } 
    else if (records.size() == 1) {
        loadRecordIntoUI(records.first());
        QMessageBox::information(this, "Booking Loaded", "One booking exists for this date.\n(Click 'NEW BOOKING' to add another)");
    } 
    else {
        QStringList items;
        for (const StudentRecord &rec : records) {
            items << QString("%1 (%2)").arg(rec.getName(), rec.getTimeSlots());
        }
        bool ok;
        QString selectedItem = QInputDialog::getItem(this, "Multiple Bookings", "Select a booking to view/edit:", items, 0, false, &ok);
        
        if (ok && !selectedItem.isEmpty()) {
            int index = items.indexOf(selectedItem);
            loadRecordIntoUI(records[index]);
            QMessageBox::information(this, "Booking Loaded", "Booking loaded successfully.");
        } else {
            currentRecord = StudentRecord();
            clearUI();
            ui->dateInput->setDate(date);
        }
    }
}