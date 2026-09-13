#ifndef RECORDSWINDOW_H
#define RECORDSWINDOW_H

#include <QDialog>
#include <QTableWidget>
#include <QVBoxLayout>
#include "customcalendarwidget.h" // අලුතින් එක් කරන ලදී
#include <QDate>
#include "databasemanager.h"
#include "studentrecord.h"

class RecordsWindow : public QDialog
{
    Q_OBJECT
public:
    explicit RecordsWindow(IDatabaseManager *db, const QString &category, QWidget *parent = nullptr);
    ~RecordsWindow();

signals:
    void editRequested(const StudentRecord &record);
    void databaseUpdated();

private slots:
    void openSettingsDialog(); 
    void printAllRecords(); 
    void printSingleRecord(const StudentRecord &record); 
    void filterByDate(const QDate &date); 
    void clearFilter();                   

private:
    QVBoxLayout *scrollLayout; 
    IDatabaseManager *dbManager;
    QString currentCategory;
    
    CustomCalendarWidget *calendar; // Custom Component එක භාවිතා කර ඇත
    QDate filterDate;          
    void updateCalendarHighlights(); 

    void setupUI();
    void loadData();
};

#endif // RECORDSWINDOW_H