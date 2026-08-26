#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QTextCharFormat>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QListView>
#include <QRandomGenerator>
#include <QInputDialog>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Initialize the Database Manager Object
    dbManager = new DatabaseManager("student_data.db");
    if (!dbManager->createTable()) {
        QMessageBox::critical(this, "DB Error", dbManager->getLastError());
    }

    setupDynamicUI();
    
    // Signal/Slot connections
    connect(ui->backBtn, &QPushButton::clicked, this, &MainWindow::goBack);
    connect(ui->calendar, &QCalendarWidget::clicked, this, &MainWindow::dateClicked);
    connect(ui->submitBtn, &QPushButton::clicked, this, &MainWindow::submitData);
    connect(ui->updateBtn, &QPushButton::clicked, this, &MainWindow::updateData);
    connect(ui->deleteBtn, &QPushButton::clicked, this, &MainWindow::deleteData);
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

    // Time Slot Dropdown Setup
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
    
    clearUI();
    
    // Reset internal state
    currentRecord = StudentRecord();
    
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

void MainWindow::clearUI()
{
    ui->nameInput->clear();
    ui->indexInput->clear();
    ui->deptInput->setCurrentIndex(0);
    ui->dateInput->setDate(QDate::currentDate());
    ui->descriptionInput->clear();
    setTimeSlotsInUI(""); // Uncheck all
}

QString MainWindow::getSelectedTimeSlots()
{
    QStandardItemModel *model = qobject_cast<QStandardItemModel*>(ui->timeSlotInput->model());
    QStringList selectedSlots;
    for (int i = 0; i < model->rowCount(); ++i) {
        if (model->item(i)->checkState() == Qt::Checked) {
            selectedSlots << model->item(i)->text();
        }
    }
    return selectedSlots.join(", ");
}

void MainWindow::setTimeSlotsInUI(const QString &timeString)
{
    QStandardItemModel *model = qobject_cast<QStandardItemModel*>(ui->timeSlotInput->model());
    for (int i = 0; i < model->rowCount(); ++i) {
        if (!timeString.isEmpty() && timeString.contains(model->item(i)->text())) {
            model->item(i)->setCheckState(Qt::Checked);
        } else {
            model->item(i)->setCheckState(Qt::Unchecked);
        }
    }
}

bool MainWindow::checkPasscode()
{
    bool ok;
    QString enteredCode = QInputDialog::getText(this, "Authentication", "Enter the 4-digit Passcode:", QLineEdit::Password, "", &ok);
    if (!ok) return false; 
    
    if (enteredCode != currentRecord.getPasscode()) {
        QMessageBox::critical(this, "Access Denied", "Incorrect Passcode!");
        return false;
    }
    return true;
}

void MainWindow::submitData()
{
    QString finalTimeStr = getSelectedTimeSlots();

    if(ui->nameInput->text().isEmpty() || ui->indexInput->text().isEmpty() || finalTimeStr.isEmpty()) {
        QMessageBox::warning(this, "Input Error", "Name, Index and at least one Time Slot are required!");
        return;
    }

    QString pinStr = QString::number(QRandomGenerator::global()->bounded(1000, 10000));

    // Populate Model Object
    StudentRecord newRecord;
    newRecord.setCategory(currentCategory);
    newRecord.setName(ui->nameInput->text());
    newRecord.setIndexNo(ui->indexInput->text());
    newRecord.setDepartment(ui->deptInput->currentText());
    newRecord.setRecordDate(ui->dateInput->date().toString("yyyy-MM-dd"));
    newRecord.setTimeSlots(finalTimeStr);
    newRecord.setDescription(ui->descriptionInput->toPlainText());
    newRecord.setPasscode(pinStr);

    // Pass Object to DatabaseManager
    if (dbManager->addRecord(newRecord)) {
        QMessageBox::information(this, "Success", QString("Data saved!\nYour Unique Passcode is: %1").arg(pinStr));
        updateCalendarHighlights();
        goBack();
    } else {
        QMessageBox::critical(this, "Database Error", dbManager->getLastError());
    }
}

void MainWindow::updateData()
{
    if (currentRecord.getId() == -1) return;
    if (!checkPasscode()) return;
    
    currentRecord.setName(ui->nameInput->text());
    currentRecord.setIndexNo(ui->indexInput->text());
    currentRecord.setDepartment(ui->deptInput->currentText());
    currentRecord.setRecordDate(ui->dateInput->date().toString("yyyy-MM-dd"));
    currentRecord.setTimeSlots(getSelectedTimeSlots());
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
    if (!checkPasscode()) return;

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

void MainWindow::dateClicked(const QDate &date)
{
    QString dateStr = date.toString("yyyy-MM-dd");
    
    // Fetch Object from DatabaseManager
    currentRecord = dbManager->getRecordByDateAndCategory(dateStr, currentCategory);
    
    if(currentRecord.getId() != -1) {
        // Load Object into UI
        ui->nameInput->setText(currentRecord.getName());
        ui->indexInput->setText(currentRecord.getIndexNo());
        ui->deptInput->setCurrentText(currentRecord.getDepartment());
        ui->dateInput->setDate(date);
        setTimeSlotsInUI(currentRecord.getTimeSlots());
        ui->descriptionInput->setText(currentRecord.getDescription());
        
        ui->submitBtn->hide();
        ui->updateBtn->show();
        ui->deleteBtn->show();
        
        QMessageBox::information(this, "Found", "Data loaded for " + dateStr + "\nNeed passcode to modify.");
    } else {
        clearUI();
        ui->dateInput->setDate(date);
        
        ui->submitBtn->show();
        ui->updateBtn->hide();
        ui->deleteBtn->hide();
    }
}