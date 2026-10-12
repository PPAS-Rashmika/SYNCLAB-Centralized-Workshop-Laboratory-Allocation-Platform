// Admin email settings dialog එකේ functions සහ UI pointer එක මෙහි ඇත.
// එකම header එක කිහිප වරක් include වුවත් definitions එකවරක් පමණක් යෙදීමට මේ guard එක ඇත.
#ifndef ADMINSETTINGSDIALOG_H
#define ADMINSETTINGSDIALOG_H

// #include වලින් මේ file එකට අවශ්‍ය Qt සහ project declarations ලබාගනී.
#include <QDialog>

// Qt විසින් .ui files වලින් සාදන UI class නම් මෙහි පෙර සඳහන් කරයි.
namespace Ui
{
class AdminSettingsDialog;
}

// Admin email addresses දෙක වෙනස් කරන QDialog එකයි.
class AdminSettingsDialog : public QDialog
{
    // Qt signals/slots හඳුනාගන්නා ලකුණයි.
    Q_OBJECT

  // අනෙක් code වලින් භාවිත කළ හැකි functions.
  public:
    // Save කළ emails දෙක පෙන්වා නිවැරදි emails දෙකම ලැබුණොත් පමණක් save කරයි.
    explicit AdminSettingsDialog(QWidget *parent = nullptr);
    // Email dialog එකේ UI helper එක නිදහස් කරයි.
    ~AdminSettingsDialog();

  // මෙම class එක තුළ පමණක් භාවිත කරන functions සහ දත්ත.
  private:
    Ui::AdminSettingsDialog *ui; // මෙම dialog එකේ .ui controls වෙත යන generated helper එක.
};

#endif
