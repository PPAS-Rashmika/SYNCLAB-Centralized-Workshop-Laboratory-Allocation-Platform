// System settings dialog එක සහ වෙනස්කම් MainWindow වෙත දන්වන signals මෙහි ඇත.
// එකම header එක කිහිප වරක් include වුවත් definitions එකවරක් පමණක් යෙදීමට මේ guard එක ඇත.
#ifndef SYSTEMSETTINGSDIALOG_H
#define SYSTEMSETTINGSDIALOG_H

// #include වලින් මේ file එකට අවශ්‍ය Qt සහ project declarations ලබාගනී.
#include <QDialog>
#include <QString>

// Qt විසින් .ui files වලින් සාදන UI class නම් මෙහි පෙර සඳහන් කරයි.
namespace Ui
{
class SystemSettingsDialog;
}

// System settings වෙනස් කරන QDialog එකයි.
class SystemSettingsDialog : public QDialog
{
    // Qt signals/slots හඳුනාගන්නා ලකුණයි.
    Q_OBJECT

  // අනෙක් code වලින් භාවිත කළ හැකි functions.
  public:
    // Theme, wallpaper සහ passcode controls සකසා buttons වල වැඩ සම්බන්ධ කරයි.
    explicit SystemSettingsDialog(QWidget *parent = nullptr);
    // System settings dialog එකේ UI helper එක නිදහස් කරයි.
    ~SystemSettingsDialog();

  // Settings වෙනස් වූ බව පිටත code වෙත දන්වන signals.
  signals:
    // Theme මාරු කළ බව MainWindow වෙත දන්වන signal එකයි.
    void themeChanged();
    // තෝරාගත් wallpaper path එක MainWindow වෙත දන්වන signal එකයි.
    void wallpaperChanged(const QString &filePath);

  // මෙම class එක තුළ පමණක් භාවිත කරන functions සහ දත්ත.
  private:
    Ui::SystemSettingsDialog *ui; // මෙම dialog එකේ .ui controls වෙත යන generated helper එක.
};

#endif
