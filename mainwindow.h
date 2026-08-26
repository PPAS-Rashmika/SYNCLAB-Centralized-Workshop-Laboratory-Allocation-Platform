#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>
#include <QSqlDatabase>
#include <QCalendarWidget>
#include <QLineEdit>
#include <QComboBox>
#include <QTextEdit>
#include <QPushButton>
#include <QDateEdit>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void openSecondInterface();
    void goBack();
    void submitData();
    void dateClicked(const QDate &date);

private:
    void setupUI();
    void setupDatabase();
    void updateCalendarHighlights();
    QWidget* createFirstInterface();
    QWidget* createSecondInterface();
    QWidget* createCard(const QString &title);

    QStackedWidget *stackedWidget;
    QSqlDatabase db;

    // 2nd Interface Widgets
    QCalendarWidget *calendar;
    QLineEdit *nameInput;
    QLineEdit *indexInput;
    QComboBox *deptInput;
    QDateEdit *dateInput;
    QComboBox *timeSlotInput; // Changed to QComboBox for Dropdown
    QTextEdit *descriptionInput;
    QString currentCategory;
};
#endif // MAINWINDOW_H