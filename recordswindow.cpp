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
    resize(1250, 500); // Buttons වලට ඉඩ මදි නොවීම සඳහා පළල වැඩි කර ඇත
    setStyleSheet("QDialog { background-color: #F4F7FC; }");

    setupUI();
    loadData();
}

RecordsWindow::~RecordsWindow() {}

void RecordsWindow::setupUI()
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    
    recordsTable = new QTableWidget(this);
    recordsTable->setColumnCount(8); // තීරු ගණන 8 දක්වා වැඩි කරන ලදී (Actions සඳහා)
    
    recordsTable->setHorizontalHeaderLabels({"Submitted Time", "Name", "Index", "Dept", "Booked Date", "Time Slot", "Description", "Actions"});
    
    recordsTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    recordsTable->horizontalHeader()->setSectionResizeMode(7, QHeaderView::ResizeToContents); // Action තීරුව ප්‍රමාණයට හැඩගැස්වීම
    
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
        recordsTable->setItem(i, 4, new QTableWidgetItem(records[i].getRecordDate()));
        recordsTable->setItem(i, 5, new QTableWidgetItem(records[i].getTimeSlots()));
        recordsTable->setItem(i, 6, new QTableWidgetItem(records[i].getDescription()));
        
        // --- අලුතින් එක් කරන Action Buttons ---
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
        
        recordsTable->setCellWidget(i, 7, actionWidget);
        
        // Change Button Action (Passcode නොමැතිව Edit කිරීමට යැවීම)
        connect(changeBtn, &QPushButton::clicked, [this, record = records[i]]() {
            emit editRequested(record);
            this->accept(); // Pop-up Window එක වසා දැමීම
        });
        
        // Delete Button Action (Passcode නොමැතිව මකා දැමීම)
        connect(deleteBtn, &QPushButton::clicked, [this, record = records[i]]() {
            if (QMessageBox::question(this, "Confirm", "Permanently delete this record without passcode?") == QMessageBox::Yes) {
                if(dbManager->deleteRecord(record.getId())) {
                    QMessageBox::information(this, "Deleted", "Record deleted successfully!");
                    loadData(); // වගුව අලුත් කිරීම
                    emit databaseUpdated(); // MainWindow එකේ Calendar එක අලුත් කිරීමට දැන්වීම
                }
            }
        });
        
        // Verify Button Action (සනාථ කිරීමේ පණිවිඩය)
        connect(verifyBtn, &QPushButton::clicked, [this, record = records[i]]() {
            QMessageBox::information(this, "Verified", "Booking for " + record.getName() + " has been successfully verified!");
        });
    }
}