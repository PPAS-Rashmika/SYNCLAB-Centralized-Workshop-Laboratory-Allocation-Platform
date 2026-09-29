#ifndef SYSTEMSETTINGSDIALOG_H
#define SYSTEMSETTINGSDIALOG_H

#include <QDialog>
#include <QString>

namespace Ui
{
class SystemSettingsDialog;
}

// Handles theme, wallpaper, and administrator passcode settings.
class SystemSettingsDialog : public QDialog
{
    Q_OBJECT

  public:
    explicit SystemSettingsDialog(QWidget *parent = nullptr);
    ~SystemSettingsDialog();

  signals:
    void themeChanged();
    void wallpaperChanged(const QString &filePath);

  private:
    Ui::SystemSettingsDialog *ui;
};

#endif // SYSTEMSETTINGSDIALOG_H
