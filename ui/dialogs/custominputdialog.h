#ifndef CUSTOMINPUTDIALOG_H
#define CUSTOMINPUTDIALOG_H

#include <QDialog>
#include <QString>

namespace Ui
{
class CustomInputDialog;
}

// Collects one non-empty text value from the user.
class CustomInputDialog : public QDialog
{
    Q_OBJECT

  public:
    explicit CustomInputDialog(const QString &title, const QString &prompt,
                               QWidget *parent = nullptr);
    ~CustomInputDialog();
    QString getInput() const;

  private:
    Ui::CustomInputDialog *ui;
};

#endif // CUSTOMINPUTDIALOG_H
