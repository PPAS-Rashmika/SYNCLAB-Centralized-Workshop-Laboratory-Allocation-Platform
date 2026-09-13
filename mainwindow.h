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
    void showRecordsWindow(); 
    void loadRecordForEdit(const StudentRecord &record);
    void onDatabaseUpdatedFromPopup();
    void updateTimeSlotDisplayText();
    void onCalendarSelectionChanged();
    void openSystemSettings();

private:
    Ui::MainWindow *ui;
    IDatabaseManager *dbManager; 
    
    QString currentCategory;
    QString currentWorkshopId; 
    StudentRecord currentRecord; 
    bool isPasscodeBypassed; 

    void setupDynamicUI();
    void applyTheme(bool isDark); 
    void drawCards(bool isDark);  
    
    // --- දෝෂය නිවැරදි කළ ස්ථානය (Parameters 4 ක් සඳහා යාවත්කාලීන කර ඇත) ---
    QWidget* createCard(const QString &id, const QString &title, const QString &subtitle, bool isDark); 

    void updateCalendarHighlights();
    bool checkPasscode();
    QStringList getSelectedTimeSlotsList(); 
    void setTimeSlotsInUI(const QString &timeString);
    void clearUI();
    void loadRecordIntoUI(const StudentRecord &record);
    
    QString savePdfFile(const QString &sourcePath, const QString &bookingId);
};

#endif // MAINWINDOW_H