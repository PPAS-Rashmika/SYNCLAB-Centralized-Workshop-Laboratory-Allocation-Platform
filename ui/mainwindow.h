#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QDate>
#include <QFrame>
#include <QList>
#include <QMainWindow>
#include <QStringList>
#include "databasemanager.h"
#include "studentrecord.h"

QT_BEGIN_NAMESPACE
namespace Ui
{
class MainWindow;
class Page0;
class Page1;
class Page2;
class Page3;
class Page4;
} // namespace Ui
QT_END_NAMESPACE

class QPushButton;
class QResizeEvent;

// This is the controller class of the whole program.
// One window holds the five pages, and every button on those pages is
// connected to one of the slot functions listed below.
class MainWindow : public QMainWindow
{
    Q_OBJECT

  public:
    // Constructor: builds the window and opens the SQLite database.
    explicit MainWindow(QWidget *parent = nullptr, const QString &databasePath = QString());
    // Destructor: closes the database and frees the pages.
    ~MainWindow();

  private slots:
    // ---- Moving between pages ----
    void startNewBooking();
    void startEditBookingFlow();
    void cancelCurrentFlow();
    void onNextClicked();
    void openSecondInterface(const QString &category);
    void onNextTo3Clicked();
    void goBackTo1();
    void goBackTo2();
    void goBackFromAdmin();

    // ---- Date and time page ----
    void onCalendarSelectionChanged();
    void onTimeSlotClicked();

    // ---- Booking form page ----
    void submitData();
    void updateData();
    void deleteData();
    void loadRecordForEdit(const StudentRecord &record);

    // ---- Administrator page ----
    void showRecordsWindow();
    void loadAdminData();
    void adminFilterByDate(const QDate &date);
    void adminClearFilter();
    void refreshAdminActions();
    void updateAdminCalendarHighlights();
    void composeStatusEmail(const QString &status);
    void openAdminEmailSettings();
    void printAllRecords();

    // ---- Settings ----
    void openSystemSettings();

  private:
    // Called automatically by Qt whenever the window size changes.
    void resizeEvent(QResizeEvent *event) override;

    // ---- First setup ----
    void setupDynamicUI();
    void applyTheme(bool darkMode);
    void showDatabaseError();
    void resetFlow();

    // ---- Workshop cards ----
    void drawCards();
    QWidget *createCard(const QString &id, const QString &title, const QString &subtitle);
    void selectCard(const QString &category);
    void arrangeCards();

    // ---- Time slots ----
    QStringList getSelectedTimeSlotsList();
    void setTimeSlotsInUI(const QString &timeString);
    void updateCalendarHighlights();

    // ---- Booking form ----
    void clearUI();
    void setFormButtonsMode(bool isEditMode);
    void loadRecordIntoUI(const StudentRecord &record);
    StudentRecord readFormRecord();
    bool validateForm();
    bool checkPasscode();
    QString savePdfFile(const QString &sourcePath, const QString &allocationKey);

    // ---- The five pages built by Qt Designer ----
    Ui::MainWindow *ui; // the outer window
    Ui::Page0 *ui0;     // welcome page
    Ui::Page1 *ui1;     // choose a workshop
    Ui::Page2 *ui2;     // choose a date and time slots
    Ui::Page3 *ui3;     // fill in the booking form
    Ui::Page4 *ui4;     // administrator page
    QWidget *page0Widget = nullptr;
    QWidget *page1Widget = nullptr;
    QWidget *page2Widget = nullptr;
    QWidget *page3Widget = nullptr;
    QWidget *page4Widget = nullptr;

    // ---- Database ----
    // Polymorphism: the pointer type is the base interface, but the object it
    // points to is the SQLite derived class. The window never needs to know
    // which database is being used, it just calls the interface functions.
    IDatabaseManager *dbManager = nullptr;
    bool databaseReady = false;
    QString dataDirectory; // folder that holds the database file and the PDF uploads

    // ---- What the user is doing right now ----
    QString currentCategory;   // the workshop being booked
    QString currentWorkshopId; // kept in the record so old data still matches
    StudentRecord currentRecord;
    bool isPasscodeBypassed = false; // true once the allocation key has been accepted
    bool isAdminEditMode = false;    // true when the administrator opened the form
    bool isStudentEditFlow = false;  // true when a student opened their own booking

    // ---- Workshop cards on page 1 ----
    QList<QFrame *> cardFrames;
    QString selectedCategory;
    int cardColumns = 0;

    // ---- Time slot buttons on page 2 ----
    // These two lists always line up: timeSlotValues[i] is the text of
    // timeSlotButtons[i]. That saves reading the value back from each button.
    QList<QPushButton *> timeSlotButtons;
    QStringList timeSlotValues;

    // ---- Administrator page ----
    QDate adminFilterDate; // invalid date means "show every booking"

    // ---- Appearance ----
    bool isDarkTheme = false;
};

#endif // MAINWINDOW_H
