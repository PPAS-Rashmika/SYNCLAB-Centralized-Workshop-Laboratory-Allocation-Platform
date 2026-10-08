// එක් text input එකක් ලබාගන්නා dialog class එක මෙහි සඳහන් කරයි.
// එකම header එක කිහිප වරක් include වුවත් definitions එකවරක් පමණක් යෙදීමට මේ guard එක ඇත.
#ifndef CUSTOMINPUTDIALOG_H
#define CUSTOMINPUTDIALOG_H

// #include වලින් මේ file එකට අවශ්‍ය Qt සහ project declarations ලබාගනී.
#include <QDialog>
#include <QString>

// Qt විසින් .ui files වලින් සාදන UI class නම් මෙහි පෙර සඳහන් කරයි.
namespace Ui
{
class CustomInputDialog;
}

// එක් text අගයක් ඉල්ලා ලබාගන්නා QDialog එකයි.
class CustomInputDialog : public QDialog
{
    // Qt signals/slots හඳුනාගන්නා ලකුණයි.
    Q_OBJECT

  // අනෙක් code වලින් භාවිත කළ හැකි functions.
  public:
    // එක් text box එකක් සහ OK/Cancel buttons සකසයි.
    explicit CustomInputDialog(const QString &title, const QString &prompt,
                               QWidget *parent = nullptr);
    // Input dialog එකේ UI helper එක නිදහස් කරයි.
    ~CustomInputDialog();
    // ඇතුළත් කළ text එකේ මුල/අග spaces ඉවත් කර ලබාදෙයි.
    QString getInput() const;

  // මෙම class එක තුළ පමණක් භාවිත කරන functions සහ දත්ත.
  private:
    Ui::CustomInputDialog *ui; // මෙම dialog එකේ .ui controls වෙත යන generated helper එක.
};

#endif
