#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDate>
#include <QStringList>
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
    void updateAvailableTimeSlots(const QDate &date);
    void showRecordsWindow(); 
    
    void loadRecordForEdit(const StudentRecord &record);
    void onDatabaseUpdatedFromPopup();

private:
    Ui::MainWindow *ui;
    IDatabaseManager *dbManager; 
    
    QString currentCategory;
    StudentRecord currentRecord; 
    bool isPasscodeBypassed; 

    void setupDynamicUI();
    void updateCalendarHighlights();
    QWidget* createCard(const QString &title);
    bool checkPasscode();
    QStringList getSelectedTimeSlotsList(); 
    void setTimeSlotsInUI(const QString &timeString);
    void clearUI();
    void loadRecordIntoUI(const StudentRecord &record);
    
    // නවතම ශ්‍රිතය: PDF ගොනුව Application එක ඇතුළට Copy කරගැනීම සඳහා
    QString savePdfFile(const QString &sourcePath);
};

#endif // MAINWINDOW_H