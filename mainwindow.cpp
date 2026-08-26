#include "mainwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QScrollArea>
#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>
#include <QTextCharFormat>
#include <QDate>
#include <QTime>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setupDatabase();
    setupUI();
    updateCalendarHighlights();
    resize(1000, 700);
    setStyleSheet("QMainWindow { background-color: #F4F7FC; }");
}

MainWindow::~MainWindow() {}

void MainWindow::setupDatabase()
{
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("student_data.db");

    if (!db.open()) {
        QMessageBox::critical(this, "Database Error", db.lastError().text());
        return;
    }

    QSqlQuery query;
    query.exec("CREATE TABLE IF NOT EXISTS records ("
               "id INTEGER PRIMARY KEY AUTOINCREMENT, "
               "category TEXT, name TEXT, index_no TEXT, "
               "department TEXT, record_date TEXT, "
               "time_slot TEXT, description TEXT)");
}

void MainWindow::setupUI()
{
    stackedWidget = new QStackedWidget(this);
    setCentralWidget(stackedWidget);

    stackedWidget->addWidget(createFirstInterface());
    stackedWidget->addWidget(createSecondInterface());
}

QWidget* MainWindow::createFirstInterface()
{
    QWidget *widget = new QWidget();
    QVBoxLayout *mainLayout = new QVBoxLayout(widget);

    // Header
    QLabel *header = new QLabel("1ST INTERFACE - WORKSHOPS & LABORATORIES");
    header->setStyleSheet("font-size: 20px; font-weight: bold; color: #003399; padding: 10px;");
    header->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(header);

    QGridLayout *gridLayout = new QGridLayout();
    gridLayout->setSpacing(20);

    // Creating sample cards
    QStringList workshops = {"Automotive", "Robotics", "Mechanical", "Painting", "Welding"};
    for(int i = 0; i < workshops.size(); i++) {
        gridLayout->addWidget(createCard(workshops[i]), 0, i);
    }

    QStringList labs = {"Microscope", "Chemistry", "Coding", "Test Tubes", "Circuitry"};
    for(int i = 0; i < labs.size(); i++) {
        gridLayout->addWidget(createCard(labs[i]), 1, i);
    }

    mainLayout->addLayout(gridLayout);
    mainLayout->addStretch();
    return widget;
}

QWidget* MainWindow::createCard(const QString &title)
{
    QFrame *card = new QFrame();
    card->setFixedSize(160, 200);
    card->setStyleSheet("QFrame { background-color: white; border-radius: 10px; border: 1px solid #D0D0D0; }"
                        "QFrame:hover { border: 2px solid #0055FF; }");
    QVBoxLayout *layout = new QVBoxLayout(card);
    
    QLabel *imgBox = new QLabel();
    imgBox->setStyleSheet("background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #0044CC, stop:1 #0088FF); border-radius: 8px;");
    imgBox->setMinimumHeight(100);
    
    QLabel *nameTag = new QLabel(title);
    nameTag->setAlignment(Qt::AlignCenter);
    nameTag->setStyleSheet("font-weight: bold; color: #333; border: none;");

    QPushButton *btn = new QPushButton("SELECT");
    btn->setStyleSheet("background-color: #E6F0FF; color: #0055FF; border-radius: 5px; padding: 5px; font-weight: bold;");
    connect(btn, &QPushButton::clicked, [this, title](){
        currentCategory = title;
        openSecondInterface();
    });

    layout->addWidget(imgBox);
    layout->addWidget(nameTag);
    layout->addWidget(btn);
    return card;
}

QWidget* MainWindow::createSecondInterface()
{
    QWidget *widget = new QWidget();
    QVBoxLayout *mainLayout = new QVBoxLayout(widget);

    // Top section: Back button & Calendar
    QHBoxLayout *topLayout = new QHBoxLayout();
    
    QPushButton *backBtn = new QPushButton("<- Back to 1st Interface");
    backBtn->setStyleSheet("padding: 10px; background-color: #ccc; border-radius: 5px;");
    connect(backBtn, &QPushButton::clicked, this, &MainWindow::goBack);
    
    calendar = new QCalendarWidget();
    calendar->setStyleSheet("QCalendarWidget QWidget { alternate-background-color: #E6F0FF; }");
    connect(calendar, &QCalendarWidget::clicked, this, &MainWindow::dateClicked);
    
    topLayout->addWidget(backBtn, 0, Qt::AlignTop);
    topLayout->addStretch();
    topLayout->addWidget(calendar);
    mainLayout->addLayout(topLayout);

    // Form Section
    QLabel *formHeader = new QLabel("STUDENT INFORMATION");
    formHeader->setAlignment(Qt::AlignCenter);
    formHeader->setStyleSheet("font-size: 16px; font-weight: bold; color: #003399; margin-top: 20px;");
    mainLayout->addWidget(formHeader);

    QHBoxLayout *formLayout = new QHBoxLayout();
    
    // Left Form
    QVBoxLayout *leftForm = new QVBoxLayout();
    nameInput = new QLineEdit(); nameInput->setPlaceholderText("Enter student name");
    indexInput = new QLineEdit(); indexInput->setPlaceholderText("Enter index number");
    deptInput = new QComboBox(); deptInput->addItems({"Select department", "Computer Science", "Engineering", "Technology"});
    
    // Updated: Date Input with Calendar Popup
    dateInput = new QDateEdit(QDate::currentDate());
    dateInput->setCalendarPopup(true);
    dateInput->setDisplayFormat("yyyy-MM-dd");

    // Updated: Time Input
    timeSlotInput = new QTimeEdit(QTime::currentTime());
    timeSlotInput->setDisplayFormat("hh:mm AP"); // 12-hour format with AM/PM
    
    leftForm->addWidget(new QLabel("NAME")); leftForm->addWidget(nameInput);
    leftForm->addWidget(new QLabel("INDEX")); leftForm->addWidget(indexInput);
    leftForm->addWidget(new QLabel("DEPARTMENT")); leftForm->addWidget(deptInput);
    leftForm->addWidget(new QLabel("DATE")); leftForm->addWidget(dateInput);
    leftForm->addWidget(new QLabel("TIME")); leftForm->addWidget(timeSlotInput);
    
    // Right Form (Description + Submit)
    QVBoxLayout *rightForm = new QVBoxLayout();
    descriptionInput = new QTextEdit();
    descriptionInput->setPlaceholderText("Enter description");
    
    QPushButton *submitBtn = new QPushButton("SUBMIT");
    submitBtn->setStyleSheet("background-color: #0055FF; color: white; padding: 15px; border-radius: 5px; font-weight: bold;");
    connect(submitBtn, &QPushButton::clicked, this, &MainWindow::submitData);
    
    rightForm->addWidget(new QLabel("DESCRIPTION"));
    rightForm->addWidget(descriptionInput);
    rightForm->addWidget(submitBtn);

    formLayout->addLayout(leftForm);
    formLayout->addSpacing(20);
    formLayout->addLayout(rightForm);
    
    mainLayout->addLayout(formLayout);
    return widget;
}

void MainWindow::openSecondInterface()
{
    // Clear/Reset previous inputs when entering 2nd interface
    nameInput->clear();
    indexInput->clear();
    deptInput->setCurrentIndex(0);
    dateInput->setDate(QDate::currentDate());
    timeSlotInput->setTime(QTime::currentTime());
    descriptionInput->clear();
    stackedWidget->setCurrentIndex(1);
}

void MainWindow::goBack()
{
    stackedWidget->setCurrentIndex(0);
}

void MainWindow::submitData()
{
    QString name = nameInput->text();
    QString index = indexInput->text();
    QString dateStr = dateInput->date().toString("yyyy-MM-dd"); // Get date from QDateEdit
    QString timeStr = timeSlotInput->time().toString("hh:mm AP"); // Get time from QTimeEdit

    if(name.isEmpty() || index.isEmpty()) {
        QMessageBox::warning(this, "Input Error", "Name and Index are required!");
        return;
    }

    QSqlQuery query;
    query.prepare("INSERT INTO records (category, name, index_no, department, record_date, time_slot, description) "
                  "VALUES (:category, :name, :index, :dept, :date, :time, :desc)");
    query.bindValue(":category", currentCategory);
    query.bindValue(":name", name);
    query.bindValue(":index", index);
    query.bindValue(":dept", deptInput->currentText());
    query.bindValue(":date", dateStr);
    query.bindValue(":time", timeStr);
    query.bindValue(":desc", descriptionInput->toPlainText());

    if (query.exec()) {
        QMessageBox::information(this, "Success", "Data saved successfully!");
        updateCalendarHighlights(); // Refresh highlights on main calendar
        goBack();
    } else {
        QMessageBox::critical(this, "Database Error", query.lastError().text());
    }
}

void MainWindow::updateCalendarHighlights()
{
    // Clear previous formatting
    QTextCharFormat defaultFormat;
    calendar->setDateTextFormat(QDate(), defaultFormat); 

    QSqlQuery query("SELECT DISTINCT record_date FROM records");
    QTextCharFormat highlightFormat;
    highlightFormat.setBackground(QColor("#B3D4FF")); // Light blue highlight
    highlightFormat.setFontWeight(QFont::Bold);

    while (query.next()) {
        QDate date = QDate::fromString(query.value(0).toString(), "yyyy-MM-dd");
        if(date.isValid()) {
            calendar->setDateTextFormat(date, highlightFormat);
        }
    }
}

void MainWindow::dateClicked(const QDate &date)
{
    QString dateStr = date.toString("yyyy-MM-dd");
    QSqlQuery query;
    query.prepare("SELECT name, index_no, department, time_slot, description FROM records WHERE record_date = :date");
    query.bindValue(":date", dateStr);
    
    if(query.exec() && query.next()) {
        // Displaying the retrieved data back into the form fields
        nameInput->setText(query.value(0).toString());
        indexInput->setText(query.value(1).toString());
        deptInput->setCurrentText(query.value(2).toString());
        
        // Convert stored strings back to QTime/QDate for the UI
        dateInput->setDate(date);
        QTime storedTime = QTime::fromString(query.value(3).toString(), "hh:mm AP");
        if(storedTime.isValid()) {
            timeSlotInput->setTime(storedTime);
        }
        
        descriptionInput->setText(query.value(4).toString());
        
        QMessageBox::information(this, "Data Found", "Data loaded for " + dateStr);
    } else {
        // Clear fields if no data for clicked date
        nameInput->clear();
        indexInput->clear();
        dateInput->setDate(date); // Just update date picker to clicked date
        descriptionInput->clear();
    }
}