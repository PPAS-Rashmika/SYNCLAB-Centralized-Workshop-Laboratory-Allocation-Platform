#ifndef RECORDSWINDOW_H
#define RECORDSWINDOW_H

#include <QDialog>
#include <QTableWidget>
#include "databasemanager.h"
#include "studentrecord.h"

class RecordsWindow : public QDialog
{
    Q_OBJECT
public:
    explicit RecordsWindow(IDatabaseManager *db, const QString &category, QWidget *parent = nullptr);
    ~RecordsWindow();

// MainWindow එක සමඟ සම්බන්ධ වීමට Signals
signals:
    void editRequested(const StudentRecord &record);
    void databaseUpdated();

private:
    QTableWidget *recordsTable;
    IDatabaseManager *dbManager;
    QString currentCategory;

    void setupUI();
    void loadData();
};

#endif // RECORDSWINDOW_H