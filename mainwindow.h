#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDate>
#include <QStringList>
#include <QFrame>
#include <QList>
#include <QTableWidget>
#include "databasemanager.h"
#include "studentrecord.h"

QT_BEGIN_NAMESPACE
namespace Ui { 
    class MainWindow; 
    class Page0;
    class Page1;
    class Page2;
    class Page3;
    class Page4;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr, const QString &databasePath = QString());
    ~MainWindow();

private slots:
    void openSecondInterface(const QString &category);
    void goBackTo0();
    void goBackTo1();
    void goBackTo2(); 
    void onNextTo3Clicked(); 
    void onNextClicked(); 
    void submitData();
    void updateData();
    void deleteData();
    void dateClicked(const QDate &date);
    void loadRecordForEdit(const StudentRecord &record);
    void onCalendarSelectionChanged();
    void onTimeSlotClicked(); 
    void openSystemSettings();
    void showRecordsWindow(); 
    void loadAdminData();
    void adminFilterByDate(const QDate &date);
    void adminClearFilter();
    void updateAdminCalendarHighlights();
    void openAdminEmailSettings();
    void printAllRecords();
    void printSingleRecord(const StudentRecord &record);
    void goBackFromAdmin();
    
    // නව Navigation සහ Edit Flow පාලනය සඳහා
    void startNewBooking();
    void startEditBookingFlow();
    void cancelCurrentFlow();

private:
    void resizeEvent(QResizeEvent *event) override;
    void arrangeCards();
    void resetFlow();
    bool validateForm();
    void showDatabaseError();
    void refreshAdminActions();
    void composeStatusEmail(const QString &status);
    QString m_dataDirectory;
    bool m_isDark = false;
    bool m_databaseReady = false;
    int m_cardColumns = 0;
    Ui::MainWindow *ui;
    Ui::Page0 *ui0;
    Ui::Page1 *ui1;
    Ui::Page2 *ui2;
    Ui::Page3 *ui3;
    Ui::Page4 *ui4;

    QWidget *page0Widget = nullptr;
    QWidget *page1Widget = nullptr;
    QWidget *page2Widget = nullptr;
    QWidget *page3Widget = nullptr;
    QWidget *page4Widget = nullptr;

    IDatabaseManager *dbManager; 
    QString currentCategory;
    QString currentWorkshopId; 
    StudentRecord currentRecord; 
    bool isPasscodeBypassed; 
    bool isAdminEditMode; 
    bool isStudentEditFlow; // Student Edit Booking Flow එක හඳුනාගැනීමට

    QString m_selectedCategory; 
    QList<QFrame*> m_cardFrames; 
    QDate adminFilterDate;

    void setupDynamicUI();
    void applyTheme(bool isDark); 
    void drawCards(bool isDark);  
    QWidget* createCard(const QString &id, const QString &title, const QString &subtitle, bool isDark); 
    void selectCard(const QString &category, QFrame *cardFrame); 

    void updateCalendarHighlights();
    bool checkPasscode();
    QStringList getSelectedTimeSlotsList(); 
    void setTimeSlotsInUI(const QString &timeString);
    void clearUI();
    void loadRecordIntoUI(const StudentRecord &record);
    QString savePdfFile(const QString &sourcePath, const QString &bookingId);
    
    // බොත්තම් පාලනය සඳහා
    void setFormButtonsMode(bool isEditMode);
};

#endif // MAINWINDOW_H
