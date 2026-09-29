#ifndef CUSTOMALERTDIALOG_H
#define CUSTOMALERTDIALOG_H

#include <QDialog>
#include <QString>

namespace Ui
{
class CustomAlertDialog;
}

// Shows information, warnings, errors, or confirmation questions.
class CustomAlertDialog : public QDialog
{
    Q_OBJECT

  public:
    enum AlertType { Info, Warning, Error, Question };
    explicit CustomAlertDialog(const QString &title, const QString &message, AlertType type,
                               QWidget *parent = nullptr);
    ~CustomAlertDialog();

    // Draws every copy of "text" inside the message as a highlighted badge.
    void highlightText(const QString &text);

  protected:
    void resizeEvent(QResizeEvent *event) override;

  private:
    Ui::CustomAlertDialog *ui;
};

#endif // CUSTOMALERTDIALOG_H
