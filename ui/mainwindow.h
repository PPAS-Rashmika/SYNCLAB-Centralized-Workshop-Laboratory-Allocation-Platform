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
class welcomeWidget;
class selectionWidget;
class calanderWidget;
class formWidget;
class adminWidget;
} // namespace Ui
QT_END_NAMESPACE

class QPushButton;
class QStackedWidget;

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
    void startNewAllocation();
    void startEditAllocationFlow();
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

    // ---- Allocation form page ----
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

    // ---- Settings ----
    void openSystemSettings();

  private:
    // ---- First setup ----
    void setupDynamicUI();
    void applyTheme(bool darkMode);
    void showDatabaseError();
    void resetFlow();

    // ---- Workshop cards ----
    void bindWorkshopCards();
    void selectCard(const QString &category);

    // ---- Time slots ----
    QStringList getSelectedTimeSlotsList();
    void setTimeSlotsInUI(const QString &timeString);
    void updateCalendarHighlights();

    // ---- Allocation form ----
    void clearUI();
    void setFormButtonsMode(bool isEditMode);
    void loadRecordIntoUI(const StudentRecord &record);
    StudentRecord readFormRecord();
    bool validateForm();
    bool checkPasscode();

    // Qt owns this central page host; the pages come from Designer forms.
    QStackedWidget *stackedWidget = nullptr;

    // ---- The five pages built by Qt Designer ----
    Ui::welcomeWidget *welcomeUi;     // welcome page
    Ui::selectionWidget *selectionUi;     // choose a workshop
    Ui::calanderWidget *calanderUi;     // choose a date and time slots
    Ui::formWidget *formUi;     // fill in the allocation form
    Ui::adminWidget *adminUi;     // administrator page
    QWidget *welcomeWidget = nullptr;
    QWidget *selectionWidget = nullptr;
    QWidget *calanderWidget = nullptr;
    QWidget *formWidget = nullptr;
    QWidget *adminWidget = nullptr;

    // ---- Database ----
    // Polymorphism: the pointer type is the base interface, but the object it
    // points to is the SQLite derived class. The window never needs to know
    // which database is being used, it just calls the interface functions.
    IDatabaseManager *dbManager = nullptr;
    bool databaseReady = false;

    // ---- What the user is doing right now ----
    QString currentCategory;   // the workshop being allocated
    QString currentWorkshopId; // kept in the record so old data still matches
    StudentRecord currentRecord;
    bool isPasscodeBypassed = false; // true once the allocation key has been accepted
    bool isAdminEditMode = false;    // true when the administrator opened the form
    bool isStudentEditFlow = false;  // true when a student opened their own allocation

    // ---- Workshop cards on page 1 ----
    QList<QFrame *> cardFrames;
    QString selectedCategory;

    // ---- Time slot buttons on page 2 ----
    // These two lists always line up: timeSlotValues[i] is the text of
    // timeSlotButtons[i]. That saves reading the value back from each button.
    QList<QPushButton *> timeSlotButtons;
    QStringList timeSlotValues;

    // ---- Administrator page ----
    QDate adminFilterDate; // invalid date means "show every allocation"

    // ---- Appearance ----
    bool isDarkTheme = false;
};

#endif // MAINWINDOW_H
