#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDate>
#include "databasemanager.h"
#include "studentrecord.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void openSecondInterface(const QString &category);
    void goBack();
    void submitData();
    void updateData();
    void deleteData();
    void dateClicked(const QDate &date);

private:
    Ui::MainWindow *ui;
    DatabaseManager *dbManager; // OOP: Using DatabaseManager as a service
    QString currentCategory;
    StudentRecord currentRecord; // OOP: Holding current state as an Object

    void setupDynamicUI();
    void updateCalendarHighlights();
    QWidget* createCard(const QString &title);
    bool checkPasscode();
    QString getSelectedTimeSlots();
    void setTimeSlotsInUI(const QString &timeString);
    void clearUI();
};

#endif // MAINWINDOW_H