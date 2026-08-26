#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSqlDatabase>
#include <QDate>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void openSecondInterface(const QString &category);
    void goBack();
    void submitData();
    void dateClicked(const QDate &date);

private:
    Ui::MainWindow *ui;
    QSqlDatabase db;
    QString currentCategory;

    void setupDatabase();
    void setupDynamicUI();
    void updateCalendarHighlights();
    QWidget* createCard(const QString &title);
};
#endif // MAINWINDOW_H