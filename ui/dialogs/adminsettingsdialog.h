#ifndef ADMINSETTINGSDIALOG_H
#define ADMINSETTINGSDIALOG_H

#include <QDialog>

namespace Ui
{
class AdminSettingsDialog;
}

// Displays and saves the optional administration email addresses.
class AdminSettingsDialog : public QDialog
{
    Q_OBJECT

  public:
    explicit AdminSettingsDialog(QWidget *parent = nullptr);
    ~AdminSettingsDialog();

  private:
    Ui::AdminSettingsDialog *ui;
};

#endif // ADMINSETTINGSDIALOG_H
