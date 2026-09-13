#ifndef RECORDSWINDOW_H
#define RECORDSWINDOW_H

#include <QDialog>
#include <QTableWidget>
#include <QVBoxLayout>
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
    void printAllRecords(); // සියල්ල Print කිරීම සඳහා
    void printSingleRecord(const StudentRecord &record); // තනි Record එකක් Print කිරීම සඳහා

private:
    QVBoxLayout *scrollLayout; 
    IDatabaseManager *dbManager;
    QString currentCategory;

    void setupUI();
    void loadData();
};

#endif // RECORDSWINDOW_H