#include "recordswindow.h"
#include <QVBoxLayout>
#include <QHeaderView>

RecordsWindow::RecordsWindow(IDatabaseManager *db, const QString &category, QWidget *parent)
    : QDialog(parent), dbManager(db), currentCategory(category)
{
    setWindowTitle(category.toUpper() + " - ALL BOOKINGS");
    // Description එකක් පෙන්වීමට අවශ්‍ය නිසා Window එකේ පළල (Width) මඳක් වැඩි කර ඇත
    resize(1100, 450); 
    setStyleSheet("QDialog { background-color: #F4F7FC; }");

    setupUI();
    loadData();
}

RecordsWindow::~RecordsWindow() {}

void RecordsWindow::setupUI()
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    
    recordsTable = new QTableWidget(this);
    // Columns ගණන 7 දක්වා වැඩි කරන ලදී
    recordsTable->setColumnCount(7); 
    
    // Header එකට "Description" යන්න එක් කරන ලදී
    recordsTable->setHorizontalHeaderLabels({"Submitted Time", "Name", "Index", "Dept", "Booked Date", "Time Slot", "Description"});
    
    recordsTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
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
        
        // අලුතින් Description දත්තය Table එකට එක් කිරීම
        recordsTable->setItem(i, 6, new QTableWidgetItem(records[i].getDescription())); 
    }
}