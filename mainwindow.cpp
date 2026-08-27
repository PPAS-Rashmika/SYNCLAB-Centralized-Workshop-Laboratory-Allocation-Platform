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
#include <QFile>
#include <QDir>
#include <QFileInfo>
#include <QCoreApplication>
#include <QGraphicsDropShadowEffect>
#include <QMap> // QMap භාවිතා කිරීම සඳහා අලුතින් එක් කළ Header එක

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
    QString logoPath = QCoreApplication::applicationDirPath() + "/assets/logo.png";
    QPixmap logoPix(logoPath);
    
    if (!logoPix.isNull()) {
        ui->logoLabel->setPixmap(logoPix.scaled(120, 120, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        ui->logoLabel->setText(""); 
        ui->logoLabel->setStyleSheet("background: transparent; border: none;");
    } else {
        ui->logoLabel->setStyleSheet("background-color: #E2E8F0; color: #64748B; font-weight: bold; border-radius: 60px; border: 2px dashed #CBD5E1;");
    }

    ui->gridLayout->setAlignment(Qt::AlignCenter); 
    ui->gridLayout->setHorizontalSpacing(30);      
    ui->gridLayout->setVerticalSpacing(30);        

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
    newBookingBtn->setStyleSheet("background-color: #10B981; color: white; padding: 15px; border-radius: 5px; font-weight: bold;");
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
    card->setFixedSize(210, 310);
    
    card->setStyleSheet(
        "QFrame {"
        "background-color: #FFFFFF;"
        "border-radius: 16px;"
        "border: 2px solid transparent;"
        "}"
        "QFrame:hover {"
        "border: 2px solid #93C5FD;"
        "background-color: #FAFCFF;"
        "}"
    );

    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect(card);
    shadow->setBlurRadius(25);
    shadow->setXOffset(0);
    shadow->setYOffset(8);
    shadow->setColor(QColor(0, 0, 0, 25)); 
    card->setGraphicsEffect(shadow);
    
    QVBoxLayout *layout = new QVBoxLayout(card);
    layout->setContentsMargins(15, 20, 15, 20); 
    layout->setSpacing(8);
    layout->setAlignment(Qt::AlignTop | Qt::AlignHCenter);
    
    QLabel *imgBox = new QLabel(card);
    imgBox->setFixedSize(170, 140);
    imgBox->setAlignment(Qt::AlignCenter);
    
    QString imgName = title.toLower() + ".png"; 
    QString imgPath = QCoreApplication::applicationDirPath() + "/assets/" + imgName;
    QPixmap pix(imgPath);
    
    if (!pix.isNull()) {
        imgBox->setPixmap(pix.scaled(170, 140, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        imgBox->setStyleSheet("background-color: transparent; border: none;");
    } else {
        imgBox->setStyleSheet("background-color: #F1F5F9; border-radius: 12px; color: #64748B; font-weight: bold; border: 1px dashed #CBD5E1;");
        imgBox->setText("[ Image ]\n" + title);
    }
    
    QLabel *nameTag = new QLabel(title, card);
    nameTag->setAlignment(Qt::AlignCenter);
    nameTag->setStyleSheet("font-size: 17px; font-weight: bold; color: #0F172A; border: none; font-family: 'Segoe UI', Arial;");

    QPushButton *btn = new QPushButton("   SELECT\n   THIS WORKSHOP", card);
    btn->setFixedSize(170, 48);
    btn->setStyleSheet(
        "QPushButton {"
        "background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #3B82F6, stop:1 #2563EB);" 
        "color: white;"
        "border-radius: 24px;" 
        "font-size: 11px;"
        "font-weight: bold;"
        "text-align: left;"
        "padding-left: 20px;"
        "border: none;"
        "}"
        "QPushButton:hover {"
        "background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #2563EB, stop:1 #1D4ED8);" 
        "}"
        "QPushButton:pressed {"
        "background-color: #1E3A8A;"
        "}"
    );
    
    QLabel *arrowIcon = new QLabel(">", btn);
    arrowIcon->setStyleSheet("color: white; font-weight: bold; font-size: 16px; background: transparent; border: none;");
    arrowIcon->setGeometry(15, 13, 20, 20);

    connect(btn, &QPushButton::clicked, [this, title](){
        openSecondInterface(title);
    });

    layout->addWidget(imgBox);
    layout->addWidget(nameTag);
    layout->addSpacerItem(new QSpacerItem(20, 10, QSizePolicy::Minimum, QSizePolicy::Expanding)); 
    layout->addWidget(btn);
    
    return card;
}

// -------------------------------------------------------------------------
// 2nd Interface එක Open කිරීම සහ Top Left Card එක යාවත්කාලීන කිරීම (Update)
// -------------------------------------------------------------------------
void MainWindow::openSecondInterface(const QString &category)
{
    currentCategory = category;
    
    // මධ්‍යයේ ඇති Form Header එක
    ui->formHeader->setText(category.toUpper() + " - STUDENT INFORMATION");
    
    // Top-Left Info Card එකේ Name එක
    ui->selectedNameLabel->setText(category.toUpper() + " WORKSHOP");

    // Workshop Descriptions Map එක
    QMap<QString, QString> descriptions = {
        {"Automotive", "Vehicle systems, maintenance, and automotive engineering technology."},
        {"Robotics", "Automation, robotic arms, and intelligent machine programming."},
        {"Mechanical", "Heavy machinery, tools, and mechanical component fabrication."},
        {"Painting", "Professional surface treatment, coatings, and material painting."},
        {"Welding", "Metal fabrication, joining processes, and industrial welding."},
        {"Microscope", "Nanomaterials analysis and microscopic optical evaluations."},
        {"Chemistry", "Organic chemistry, chemical synthesis, and molecular analysis."},
        {"Coding", "Software engineering, algorithms, data structures, and application development."},
        {"Test Tubes", "Sample preparation, fluid dynamics, and biochemical experimentation."},
        {"Circuitry", "PCB design, embedded systems, soldering, and analog/digital electronics."}
    };
    
    // Top-Left Info Card එකේ Description එක (Map එකෙන් සොයා ගැනීම)
    ui->selectedDescLabel->setText(descriptions.value(category, "Explore facilities, tools, and scheduled activities."));

    // Top-Left Info Card එකේ Image එක Load කිරීම
    QString imgName = category.toLower() + ".png"; 
    QString imgPath = QCoreApplication::applicationDirPath() + "/assets/" + imgName;
    QPixmap pix(imgPath);
    
    if (!pix.isNull()) {
        ui->selectedImgLabel->setPixmap(pix.scaled(120, 90, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        ui->selectedImgLabel->setStyleSheet("background-color: transparent; border: none;");
        ui->selectedImgLabel->setText("");
    } else {
        ui->selectedImgLabel->setStyleSheet("background-color: #F1F5F9; border-radius: 8px; color: #64748B; font-weight: bold; border: 1px dashed #CBD5E1;");
        ui->selectedImgLabel->setText("[ Image ]");
    }

    currentRecord = StudentRecord();
    clearUI();
    
    ui->submitBtn->show();
    ui->updateBtn->hide();
    ui->deleteBtn->hide();
    
    updateCalendarHighlights();
    ui->stackedWidget->setCurrentIndex(1);
}
// -------------------------------------------------------------------------

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

QString MainWindow::savePdfFile(const QString &sourcePath)
{
    if (sourcePath.isEmpty()) return "";

    QString uploadDirPath = QCoreApplication::applicationDirPath() + "/uploads/pdfs";
    
    if (sourcePath.startsWith(uploadDirPath)) {
        return sourcePath;
    }

    QDir dir(uploadDirPath);
    if (!dir.exists()) {
        dir.mkpath("."); 
    }

    QFileInfo fileInfo(sourcePath);
    QString uniqueFileName = QString::number(QDateTime::currentMSecsSinceEpoch()) + "_" + fileInfo.fileName();
    QString destPath = dir.absoluteFilePath(uniqueFileName);

    if (QFile::copy(sourcePath, destPath)) {
        return destPath; 
    } else {
        return sourcePath; 
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
    newRecord.setPdfPath(securePdfPath); 
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