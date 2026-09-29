#ifndef CUSTOMCALENDARWIDGET_H
#define CUSTOMCALENDARWIDGET_H

#include <QCalendarWidget>
#include <QDate>
#include <QMap>
#include <QPainter>

// A normal Qt calendar that also draws a small badge on each day showing how
// many bookings that day has: A = approved, P = pending, R = rejected.
//
// INHERITANCE: it is a derived class of QCalendarWidget, so it already behaves
// like a calendar and we only add what is missing.
// POLYMORPHISM: paintCell() is a virtual function of QCalendarWidget. By
// overriding it here, Qt calls our version when it draws each day cell, while
// still calling the base class version for the day number itself.
class CustomCalendarWidget : public QCalendarWidget
{
    Q_OBJECT

  public:
    // Constructor
    explicit CustomCalendarWidget(QWidget *parent = nullptr);

    // MainWindow calls this with the booking counts for the shown month.
    void setStatusCounts(const QMap<QDate, QMap<QString, int>> &counts);

  protected:
    // Override the base class function that paints one day cell.
    // In Qt 6 the date must be passed by value, not by reference.
    void paintCell(QPainter *painter, const QRect &rect, QDate date) const override;

  private:
    // Encapsulation: the counts are private. Outside code can only replace all
    // of them at once through setStatusCounts().
    // Outer key = the date, inner key = the status, inner value = how many.
    QMap<QDate, QMap<QString, int>> statusCounts;
};

#endif // CUSTOMCALENDARWIDGET_H
