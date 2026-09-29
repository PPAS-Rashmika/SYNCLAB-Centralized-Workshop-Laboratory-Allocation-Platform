#ifndef CUSTOMCALENDARWIDGET_H
#define CUSTOMCALENDARWIDGET_H

#include <QCalendarWidget>
#include <QMap>
#include <QDate>
#include <QPainter>

class CustomCalendarWidget : public QCalendarWidget
{
    Q_OBJECT
  public:
    explicit CustomCalendarWidget(QWidget *parent = nullptr);

    void setStatusCounts(const QMap<QDate, QMap<QString, int>> &counts);

  protected:
    // Qt 6 සඳහා QDate අනිවාර්යයෙන් Pass by Value ලෙස ලබා දිය යුතුය
    void paintCell(QPainter *painter, const QRect &rect, QDate date) const override;

  private:
    QMap<QDate, QMap<QString, int>> m_counts;
};

#endif // CUSTOMCALENDARWIDGET_H