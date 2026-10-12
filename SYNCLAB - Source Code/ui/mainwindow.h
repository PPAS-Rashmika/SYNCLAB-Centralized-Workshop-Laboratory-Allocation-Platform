// MainWindow හි functions සහ මතකයේ තබන දත්ත මෙහි සඳහන් කරයි. ඒවායේ වැඩ mainwindow.cpp හි ඇත.
// එකම header එක කිහිප වරක් include වුවත් definitions එකවරක් පමණක් යෙදීමට මේ guard එක ඇත.
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

// #include වලින් මේ file එකට අවශ්‍ය Qt සහ project declarations ලබාගනී.
#include <QDate>
#include <QFrame>
#include <QList>
#include <QMainWindow>
#include <QStringList>
#include "databasemanager.h"
#include "studentrecord.h"

QT_BEGIN_NAMESPACE
// Qt විසින් .ui files වලින් සාදන UI class නම් මෙහි පෙර සඳහන් කරයි.
namespace Ui
{
class welcomeWidget;
class selectionWidget;
class calanderWidget;
class formWidget;
class adminWidget;
}
QT_END_NAMESPACE

class QPushButton;
class QStackedWidget;

// Qt MainWindow එකෙන් ලබාගත් ප්‍රධාන window class එකයි; පිටු පහ මෙයින් පාලනය කරයි.
class MainWindow : public QMainWindow
{
    // Qt signals/slots හඳුනාගන්නා ලකුණයි.
    Q_OBJECT

  // අනෙක් code වලින් භාවිත කළ හැකි functions.
  public:
    // පිටු පහ සාදා database එක විවෘත කර buttons සහ ඒවායේ functions සම්බන්ධ කරයි.
    explicit MainWindow(QWidget *parent = nullptr, const QString &databasePath = QString());
    // Window එක වසන විට database එක සහ new වලින් හදපු UI helpers නිදහස් කරයි.
    ~MainWindow();

  // Button සහ calendar events වලට සම්බන්ධ කරන functions.
  private slots:
    // නව booking එකක් සඳහා form එක හිස් කර workshop selection පිටුව පෙන්වයි.
    void startNewAllocation();
    // Allocation key එකෙන් booking එක සොයා student ට edit කිරීමට විවෘත කරයි.
    void startEditAllocationFlow();
    // Save නොකළ දත්ත තිබේ නම් තහවුරු කිරීමක් ඉල්ලා welcome පිටුවට යයි.
    void cancelCurrentFlow();
    // තෝරාගත් workshop එකට calendar පිටුව පෙන්වයි; workshop මාරු වුණොත් slots ඉවත් කරයි.
    void onNextClicked();
    // Workshop නම, department සහ image calendar/admin පිටුවලට දමා calendar පිටුව විවෘත කරයි.
    void openSecondInterface(const QString &category);
    // Slots නැවත පරීක්ෂා කර දිනය/වෙලාව summary එකට දමා form පිටුව පෙන්වයි.
    void onNextTo3Clicked();
    // Student නම් selection පිටුවටත් admin edit එකක් නම් dashboard එකටත් ආපසු යයි.
    void goBackTo1();
    // Form පිටුවෙන් calendar පිටුවට ආපසු යයි.
    void goBackTo2();
    // Admin වැඩේ අවසන් කර welcome පිටුව පෙන්වයි.
    void goBackFromAdmin();

    // තෝරාගත් දිනට පුරවා ඇති slots සොයා unavailable buttons අක්‍රිය කරයි.
    void onCalendarSelectionChanged();
    // අවම වශයෙන් slot එකක් තෝරා ඇත්නම් පමණක් Next ක්‍රියාත්මක කරයි.
    void onTimeSlotClicked();

    // නව booking එක පරීක්ෂා කර database එකට save කර allocation key එක පෙන්වයි.
    void submitData();
    // පවතින booking එකේ වෙනස්කම් save කර අදාළ පිටුවට ආපසු යයි.
    void updateData();
    // User තහවුරු කළ පසු දැනට edit කරන booking එක මකා දමයි.
    void deleteData();
    // Admin තෝරාගත් booking එක form පිටුවට ගෙන edit mode එක සකසයි.
    void loadRecordForEdit(const StudentRecord &record);

    // Workshop admin passcode එක පරීක්ෂා කර dashboard එක විවෘත කරයි.
    void showRecordsWindow();
    // Database bookings table rows වලට දමා editable remarks සහ status controls සකසයි.
    void loadAdminData();
    // Admin තෝරාගත් දිනයේ bookings පමණක් table එකට ගනී.
    void adminFilterByDate(const QDate &date);
    // Date filter එක ඉවත් කර සියලු bookings පෙන්වයි.
    void adminClearFilter();
    // Table row එකක් තෝරා ඇති විට පමණක් Edit/Delete/Email buttons ක්‍රියාත්මක කරයි.
    void refreshAdminActions();
    // එම booking ගණන් admin calendar එකටත් යොදයි.
    void updateAdminCalendarHighlights();
    // Status එක save කර email draft විවෘත කරයි; subject/body mainwindow.cpp හි මෙම function එකෙන් වෙනස් කරන්න.
    void composeStatusEmail(const QString &status);
    // Admin email settings dialog එක විවෘත කරයි.
    void openAdminEmailSettings();

    // Master passcode එක පරීක්ෂා කර system settings dialog එක විවෘත කරයි.
    void openSystemSettings();

  // මෙම class එක තුළ පමණක් භාවිත කරන functions සහ දත්ත.
  private:
    // Time slots, table පළල සහ input සීමා සකසයි; mainwindow.cpp හි මෙම function එකේ අගයන් වෙනස් කරන්න.
    void setupDynamicUI();
    // තෝරාගත් theme සහ wallpaper පිටු පහට යොදයි; calendar සහ card පාටත් නැවත සකසයි.
    void applyTheme(bool darkMode);
    // Database වැඩේ අසාර්ථක වූ හේතුව user ට පෙන්වයි.
    void showDatabaseError();
    // දැනට කරන booking වැඩේ සහ form values මුල් තත්ත්වයට ගෙන එයි.
    void resetFlow();

    // selection.ui හි card category එක කියවා එහි button එක selectCard() වෙත සම්බන්ධ කරයි.
    void bindWorkshopCards();
    // තෝරාගත් card එකට selected ලකුණ දී Next button එක ක්‍රියාත්මක කරයි.
    void selectCard(const QString &category);

    // Enable වී tick කළ time buttons වල values ලැයිස්තුවක් ලබාදෙයි.
    QStringList getSelectedTimeSlotsList();
    // Save කළ slot list එකට ගැළපෙන available time buttons tick කරයි.
    void setTimeSlotsInUI(const QString &timeString);
    // Booking status අනුව දිනකට ඇති ගණන් සොයා student calendar එකට දෙයි.
    void updateCalendarHighlights();

    // Student form inputs සහ තෝරාගත් slots හිස් කරයි.
    void clearUI();
    // නව booking එකට Submit; edit එකට Update/Delete පෙන්වයි.
    void setFormButtonsMode(bool isEditMode);
    // Save කළ booking දත්ත input fields සහ date/time controls වලට දමයි.
    void loadRecordIntoUI(const StudentRecord &record);
    // Screen එකේ input values කියවා StudentRecord එකකට දමයි.
    StudentRecord readFormRecord();
    // Form එක පරීක්ෂා කර වැරදි field එකට cursor එක ගෙන යයි.
    bool validateForm();
    // ඇතුළත් කළ allocation key එක මෙම booking එකේ key එකදැයි බලයි.
    bool checkPasscode();

    QStackedWidget *stackedWidget = nullptr; // එක් වරකට එක් UI පිටුවක් පෙන්වන Qt stack එක.

    Ui::welcomeWidget *welcomeUi; // welcome.ui controls වෙත යන generated UI helper එක.
    Ui::selectionWidget *selectionUi; // selection.ui controls වෙත යන generated UI helper එක.
    Ui::calanderWidget *calanderUi; // calander.ui controls වෙත යන generated UI helper එක.
    Ui::formWidget *formUi; // form.ui controls වෙත යන generated UI helper එක.
    Ui::adminWidget *adminUi; // admin.ui controls වෙත යන generated UI helper එක.
    QWidget *welcomeWidget = nullptr; // Welcome පිටුවේ controls අඩංගු widget එක.
    QWidget *selectionWidget = nullptr; // Workshop selection පිටුවේ controls අඩංගු widget එක.
    QWidget *calanderWidget = nullptr; // Date/time පිටුවේ controls අඩංගු widget එක.
    QWidget *formWidget = nullptr; // Student form controls අඩංගු widget එක.
    QWidget *adminWidget = nullptr; // Admin dashboard controls අඩංගු widget එක.

    IDatabaseManager *dbManager = nullptr; // Database interface pointer එක; සැබෑ SQLite object එකේ functions කැඳවයි.
    bool databaseReady = false; // Database එක සාර්ථකව සූදානම් නම් true වේ.

    QString currentCategory; // දැනට වැඩ කරන workshop category එක.
    QString currentWorkshopId; // Category එකම පරණ model නමෙන් තබන අගය.
    StudentRecord currentRecord; // දැනට කරන හෝ edit කරන booking එකේ දත්ත.
    bool isPasscodeBypassed = false; // මෙම වැඩේ සඳහා key එක දැනටමත් තහවුරු වී ඇත්ද යන්න.
    bool isAdminEditMode = false; // Admin dashboard එකෙන් booking edit කිරීමට ආවේද යන්න.
    bool isStudentEditFlow = false; // Student තම booking එක edit කරන්නේද යන්න.

    QList<QFrame *> cardFrames; // selection.ui හි category ඇති workshop cards ලැයිස්තුව.
    QString selectedCategory; // User දැනට තෝරාගත් card එකේ category එක.

    QList<QPushButton *> timeSlotButtons; // Time buttons හතර; timeSlotValues සමඟ අනුපිළිවෙළ ගළපන්න.
    QStringList timeSlotValues; // එම buttons සඳහා database වෙත save කරන time text values.

    QDate adminFilterDate; // හිස් දිනයක් නම් සියලු bookings; නැතිනම් එම දිනය පමණක්.

    bool isDarkTheme = false; // දැනට dark theme භාවිත වේද යන්න.
};

#endif
