#include "recordswindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QHeaderView>
#include <QMessageBox>
#include <QWidget>

RecordsWindow::RecordsWindow(IDatabaseManager *db, const QString &category, QWidget *parent)
    : QDialog(parent), dbManager(db), currentCategory(category)
{
    setWindowTitle(category.toUpper() + " - ALL BOOKINGS");
    resize(1400, 500); // නව තීරු සඳහා Window එක තවත් විශාල කරන ලදී
    setStyleSheet("QDialog { background-color: #F4F7FC; }");

    setupUI();
    loadData();
}

RecordsWindow::~RecordsWindow() {}

void RecordsWindow::setupUI()
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    
    recordsTable = new QTableWidget(this);
    // Columns ගණන 10 දක්වා වැඩි කරන ලදී (Email, PDF)
    recordsTable->setColumnCount(10); 
    
    recordsTable->setHorizontalHeaderLabels({"Submitted Time", "Name", "Index", "Dept", "Email", "PDF Document", "Booked Date", "Time Slot", "Description", "Actions"});
    
    recordsTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    recordsTable->horizontalHeader()->setSectionResizeMode(9, QHeaderView::ResizeToContents);
    
    recordsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    recordsTable->setAlternatingRowColors(true);
    recordsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    recordsTable->setStyleSheet("QTableWidget { background-color: white; alternate-background-color: #F0F4FA; }");
    
    layout->addWidget(recordsTable);
}

void RecordsWindow::loadData()
{
    QList<StudentRecord> records = dbManager->getAllRecordsForCategory(currentCategory);
    recordsTable->setRowCount(records.size());
    
    for (int i = 0; i < records.size(); ++i) {
        recordsTable->setItem(i, 0, new QTableWidgetItem(records[i].getCreatedAt()));
        recordsTable->setItem(i, 1, new QTableWidgetItem(records[i].getName()));
        recordsTable->setItem(i, 2, new QTableWidgetItem(records[i].getIndexNo()));
        recordsTable->setItem(i, 3, new QTableWidgetItem(records[i].getDepartment()));
        recordsTable->setItem(i, 4, new QTableWidgetItem(records[i].getEmail()));
        recordsTable->setItem(i, 5, new QTableWidgetItem(records[i].getPdfPath()));
        recordsTable->setItem(i, 6, new QTableWidgetItem(records[i].getRecordDate()));
        recordsTable->setItem(i, 7, new QTableWidgetItem(records[i].getTimeSlots()));
        recordsTable->setItem(i, 8, new QTableWidgetItem(records[i].getDescription()));
        
        QWidget *actionWidget = new QWidget();
        QHBoxLayout *hLayout = new QHBoxLayout(actionWidget);
        hLayout->setContentsMargins(2, 2, 2, 2);
        hLayout->setSpacing(5);
        
        QPushButton *changeBtn = new QPushButton("Change");
        changeBtn->setStyleSheet("background-color: #FFA500; color: white; font-weight: bold; border-radius: 3px; padding: 5px;");
        
        QPushButton *deleteBtn = new QPushButton("Delete");
        deleteBtn->setStyleSheet("background-color: #FF0000; color: white; font-weight: bold; border-radius: 3px; padding: 5px;");
        
        QPushButton *verifyBtn = new QPushButton("Verify");
        verifyBtn->setStyleSheet("background-color: #28A745; color: white; font-weight: bold; border-radius: 3px; padding: 5px;");
        
        hLayout->addWidget(changeBtn);
        hLayout->addWidget(deleteBtn);
        hLayout->addWidget(verifyBtn);
        
        recordsTable->setCellWidget(i, 9, actionWidget);
        
        connect(changeBtn, &QPushButton::clicked, [this, record = records[i]]() {
            emit editRequested(record);
            this->accept(); 
        });
        
        connect(deleteBtn, &QPushButton::clicked, [this, record = records[i]]() {
            if (QMessageBox::question(this, "Confirm", "Permanently delete this record without passcode?") == QMessageBox::Yes) {
                if(dbManager->deleteRecord(record.getId())) {
                    QMessageBox::information(this, "Deleted", "Record deleted successfully!");
                    loadData(); 
                    emit databaseUpdated(); 
                }
            }
        });
        
        connect(verifyBtn, &QPushButton::clicked, [this, record = records[i]]() {
            QMessageBox::information(this, "Verified", "Booking for " + record.getName() + " has been successfully verified!");
        });
    }
}