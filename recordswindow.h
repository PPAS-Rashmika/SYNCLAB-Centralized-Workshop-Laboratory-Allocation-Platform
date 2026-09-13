#ifndef RECORDSWINDOW_H
#define RECORDSWINDOW_H

#include <QDialog>
#include <QTableWidget>
#include <QVBoxLayout>
#include <QCalendarWidget>
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
    void filterByDate(const QDate &date); // Calendar දිනයක් Click කළ විට Filter කිරීමට
    void clearFilter();                   // Filter එක ඉවත් කර සියල්ල බැලීමට

private:
    QVBoxLayout *scrollLayout; 
    IDatabaseManager *dbManager;
    QString currentCategory;
    
    QCalendarWidget *calendar; // අලුතින් එක් කළ Calendar එක
    QDate filterDate;          // තෝරාගත් දිනය රඳවා ගැනීමට
    void updateCalendarHighlights(); // Calendar එකේ Booked Dates Highlight කිරීමට

    void setupUI();
    void loadData();
};

#endif // RECORDSWINDOW_H