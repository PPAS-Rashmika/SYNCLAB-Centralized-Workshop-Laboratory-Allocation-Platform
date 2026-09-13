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
    
    // දිනවලට අදාළ දත්ත ප්‍රමාණ ලබාදීම සඳහා
    void setStatusCounts(const QMap<QDate, QMap<QString, int>> &counts);

protected:
    // Calendar Cell එක ඇතුළේ ඉලක්කම් ඇඳීම (Draw කිරීම)
    void paintCell(QPainter *painter, const QRect &rect, const QDate &date) const override;

private:
    QMap<QDate, QMap<QString, int>> m_counts;
};

#endif // CUSTOMCALENDARWIDGET_H