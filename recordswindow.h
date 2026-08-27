#ifndef RECORDSWINDOW_H
#define RECORDSWINDOW_H

#include <QDialog>
#include <QTableWidget>
#include "databasemanager.h"

// නව Window එක සඳහා වන Class එක
class RecordsWindow : public QDialog
{
    Q_OBJECT
public:
    explicit RecordsWindow(IDatabaseManager *db, const QString &category, QWidget *parent = nullptr);
    ~RecordsWindow();

private:
    QTableWidget *recordsTable;
    IDatabaseManager *dbManager;
    QString currentCategory;

    void setupUI();
    void loadData();
};

#endif // RECORDSWINDOW_H