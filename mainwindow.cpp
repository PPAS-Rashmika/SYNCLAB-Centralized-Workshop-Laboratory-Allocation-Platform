#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>
#include <QTextCharFormat>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QListView>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setupDatabase();
    setupDynamicUI();
    
    connect(ui->backBtn, &QPushButton::clicked, this, &MainWindow::goBack);
    connect(ui->calendar, &QCalendarWidget::clicked, this, &MainWindow::dateClicked);
    connect(ui->submitBtn, &QPushButton::clicked, this, &MainWindow::submitData);
}

MainWindow::~MainWindow()
{
    delete ui;
}

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
    ui->dateInput->setDate(QDate::currentDate());
}

QWidget* MainWindow::createCard(const QString &title)
{
    QFrame *card = new QFrame(this);
    card->setFixedSize(160, 200);
    card->setStyleSheet("QFrame { background-color: white; border-radius: 10px; border: 1px solid #D0D0D0; }"
                        "QFrame:hover { border: 2px solid #0055FF; }");
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
    
    // UI එකේ Header එක අදාළ Card එකේ නමට වෙනස් කිරීම
    ui->formHeader->setText(category.toUpper() + " - STUDENT INFORMATION");
    
    ui->nameInput->clear();
    ui->indexInput->clear();
    ui->deptInput->setCurrentIndex(0);
    ui->dateInput->setDate(QDate::currentDate());
    ui->descriptionInput->clear();
    
    QStandardItemModel *model = qobject_cast<QStandardItemModel*>(ui->timeSlotInput->model());
    for (int i = 0; i < model->rowCount(); ++i) {
        model->item(i)->setCheckState(Qt::Unchecked);
    }
    
    // මෙම Card එකට අදාළ දින පමණක් Calendar එකේ Highlight කිරීම
    updateCalendarHighlights();
    
    ui->stackedWidget->setCurrentIndex(1);
}

void MainWindow::goBack()
{
    ui->stackedWidget->setCurrentIndex(0);
}

void MainWindow::submitData()
{
    QString name = ui->nameInput->text();
    QString index = ui->indexInput->text();
    QString dateStr = ui->dateInput->date().toString("yyyy-MM-dd"); 

    QStandardItemModel *model = qobject_cast<QStandardItemModel*>(ui->timeSlotInput->model());
    QStringList selectedSlots;
    for (int i = 0; i < model->rowCount(); ++i) {
        if (model->item(i)->checkState() == Qt::Checked) {
            selectedSlots << model->item(i)->text();
        }
    }
    QString finalTimeStr = selectedSlots.join(", "); 

    if(name.isEmpty() || index.isEmpty() || finalTimeStr.isEmpty()) {
        QMessageBox::warning(this, "Input Error", "Name, Index and at least one Time Slot are required!");
        return;
    }

    QSqlQuery query;
    query.prepare("INSERT INTO records (category, name, index_no, department, record_date, time_slot, description) "
                  "VALUES (:category, :name, :index, :dept, :date, :time, :desc)");
    query.bindValue(":category", currentCategory);
    query.bindValue(":name", name);
    query.bindValue(":index", index);
    query.bindValue(":dept", ui->deptInput->currentText());
    query.bindValue(":date", dateStr);
    query.bindValue(":time", finalTimeStr); 
    query.bindValue(":desc", ui->descriptionInput->toPlainText());

    if (query.exec()) {
        QMessageBox::information(this, "Success", "Data saved successfully!");
        updateCalendarHighlights(); // Refresh highlights for current category
    } else {
        QMessageBox::critical(this, "Database Error", query.lastError().text());
    }
}

void MainWindow::updateCalendarHighlights()
{
    QTextCharFormat defaultFormat;
    ui->calendar->setDateTextFormat(QDate(), defaultFormat); 

    QSqlQuery query;
    // තෝරාගත් අංශයට (Category) අදාළ දින පමණක් ලබා ගැනීම
    query.prepare("SELECT DISTINCT record_date FROM records WHERE category = :category");
    query.bindValue(":category", currentCategory);
    query.exec();

    QTextCharFormat highlightFormat;
    highlightFormat.setBackground(QColor("#B3D4FF"));
    highlightFormat.setFontWeight(QFont::Bold);

    while (query.next()) {
        QDate date = QDate::fromString(query.value(0).toString(), "yyyy-MM-dd");
        if(date.isValid()) {
            ui->calendar->setDateTextFormat(date, highlightFormat);
        }
    }
}

void MainWindow::dateClicked(const QDate &date)
{
    QString dateStr = date.toString("yyyy-MM-dd");
    QSqlQuery query;
    
    // තෝරාගත් දිනට සහ තෝරාගෙන ඇති අංශයට අදාළ දත්ත පමණක් ලබා ගැනීම
    query.prepare("SELECT name, index_no, department, time_slot, description FROM records WHERE record_date = :date AND category = :category");
    query.bindValue(":date", dateStr);
    query.bindValue(":category", currentCategory);
    
    if(query.exec() && query.next()) {
        ui->nameInput->setText(query.value(0).toString());
        ui->indexInput->setText(query.value(1).toString());
        ui->deptInput->setCurrentText(query.value(2).toString());
        ui->dateInput->setDate(date);
        
        QString storedTime = query.value(3).toString();
        QStandardItemModel *model = qobject_cast<QStandardItemModel*>(ui->timeSlotInput->model());
        for (int i = 0; i < model->rowCount(); ++i) {
            if (storedTime.contains(model->item(i)->text())) {
                model->item(i)->setCheckState(Qt::Checked);
            } else {
                model->item(i)->setCheckState(Qt::Unchecked);
            }
        }
        
        ui->descriptionInput->setText(query.value(4).toString());
        QMessageBox::information(this, "Data Found", "Data loaded for " + dateStr + " under " + currentCategory);
    } else {
        ui->nameInput->clear();
        ui->indexInput->clear();
        ui->dateInput->setDate(date);
        ui->descriptionInput->clear();
        
        QStandardItemModel *model = qobject_cast<QStandardItemModel*>(ui->timeSlotInput->model());
        for (int i = 0; i < model->rowCount(); ++i) {
            model->item(i)->setCheckState(Qt::Unchecked);
        }
    }
}