// පණිවිඩ dialog එකේ වර්ග සහ functions මෙහි සඳහන් කරයි.
// එකම header එක කිහිප වරක් include වුවත් definitions එකවරක් පමණක් යෙදීමට මේ guard එක ඇත.
#ifndef CUSTOMALERTDIALOG_H
#define CUSTOMALERTDIALOG_H

// #include වලින් මේ file එකට අවශ්‍ය Qt සහ project declarations ලබාගනී.
#include <QDialog>
#include <QString>

// Qt විසින් .ui files වලින් සාදන UI class නම් මෙහි පෙර සඳහන් කරයි.
namespace Ui
{
class CustomAlertDialog;
}

// සාමාන්‍ය QDialog එකට message වර්ග සහ දිග text ගළපන වැඩ එක් කරයි.
class CustomAlertDialog : public QDialog
{
    // Qt signals/slots හඳුනාගන්නා ලකුණයි.
    Q_OBJECT

  // අනෙක් code වලින් භාවිත කළ හැකි functions.
  public:
    // Dialog වර්ග: තොරතුරු, අවවාදය, දෝෂය සහ තහවුරු කිරීම.
    enum AlertType { Info, Warning, Error, Question };
    // Title/message සහ OK/Cancel සකසයි; message වර්ගයට ගැළපෙන පාට තෝරයි.
    explicit CustomAlertDialog(const QString &title, const QString &message, AlertType type,
                               QWidget *parent = nullptr);
    // මෙම dialog එකේ UI helper එක නිදහස් කරයි.
    ~CustomAlertDialog();

    // Allocation key වැනි text එක message එක තුළ ලොකු පැහැදිලි badge එකක් ලෙස පෙන්වයි.
    void highlightText(const QString &text);

  // මෙම class එකට සහ එයින් ලබාගත් classes වලට භාවිත කළ හැකි functions.
  protected:
    // කවුළුවේ ප්‍රමාණය වෙනස් වූ විට ඇතුළත controls නැවත ගළපයි.
    void resizeEvent(QResizeEvent *event) override;

  // මෙම class එක තුළ පමණක් භාවිත කරන functions සහ දත්ත.
  private:
    // දිග message එක නොකැපෙන ලෙස dialog උස සහ buttons වල ස්ථාන සකසයි.
    void fitMessage();
    Ui::CustomAlertDialog *ui; // මෙම dialog එකේ .ui controls වෙත යන generated helper එක.
};

#endif
