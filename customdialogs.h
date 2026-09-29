#ifndef CUSTOMDIALOGS_H
#define CUSTOMDIALOGS_H

#include <QDialog>
#include <QString>

namespace Ui {
    class CustomAlertDialog;
    class CustomInputDialog;
    class AdminSettingsDialog;
    class SystemSettingsDialog;
}

class CustomAlertDialog : public QDialog {
    Q_OBJECT
public:
    enum AlertType { Info, Warning, Error, Question };
    explicit CustomAlertDialog(const QString &title, const QString &msg, AlertType type, QWidget *parent = nullptr);
    ~CustomAlertDialog();
protected:
    void resizeEvent(QResizeEvent *event) override;
private:
    Ui::CustomAlertDialog *ui;
};

class CustomInputDialog : public QDialog {
    Q_OBJECT
public:
    explicit CustomInputDialog(const QString &title, const QString &prompt, QWidget *parent = nullptr);
    ~CustomInputDialog();
    QString getInput() const;
private:
    Ui::CustomInputDialog *ui;
};

class AdminSettingsDialog : public QDialog {
    Q_OBJECT
public:
    explicit AdminSettingsDialog(QWidget *parent = nullptr);
    ~AdminSettingsDialog();
private:
    Ui::AdminSettingsDialog *ui;
};

class SystemSettingsDialog : public QDialog {
    Q_OBJECT
signals:
    void themeChanged();
    void wallpaperChanged(const QString &filePath); // නව සිග්නල් එක
public:
    explicit SystemSettingsDialog(QWidget *parent = nullptr);
    ~SystemSettingsDialog();
private:
    Ui::SystemSettingsDialog *ui;
};

#endif // CUSTOMDIALOGS_H
