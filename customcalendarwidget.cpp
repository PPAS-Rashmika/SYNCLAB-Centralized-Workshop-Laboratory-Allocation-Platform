#include "customcalendarwidget.h"

CustomCalendarWidget::CustomCalendarWidget(QWidget *parent) : QCalendarWidget(parent) {}

void CustomCalendarWidget::setStatusCounts(const QMap<QDate, QMap<QString, int>> &counts)
{
    m_counts = counts;
    updateCells(); 
}

// Qt 6 සඳහා යාවත්කාලීන කරන ලදී (QDate date)
void CustomCalendarWidget::paintCell(QPainter *painter, const QRect &rect, QDate date) const
{
    QCalendarWidget::paintCell(painter, rect, date);

    if (m_counts.contains(date)) {
        QMap<QString, int> counts = m_counts.value(date);
        int approve = counts.value("Approve", 0);
        int pending = counts.value("Pending", 0);
        int reject = counts.value("Reject", 0);

        painter->save();
        QFont font = painter->font();
        font.setPointSize(7); 
        font.setBold(true);
        painter->setFont(font);

        int sectionWidth = rect.width() / 3;
        int yOffset = rect.bottom() - 12; 

        if (approve > 0) {
            painter->setPen(QColor("#10B981")); 
            painter->drawText(QRect(rect.left(), yOffset, sectionWidth, 14), Qt::AlignCenter, QString::number(approve));
        }
        if (pending > 0) {
            painter->setPen(QColor("#F59E0B")); 
            painter->drawText(QRect(rect.left() + sectionWidth, yOffset, sectionWidth, 14), Qt::AlignCenter, QString::number(pending));
        }
        if (reject > 0) {
            painter->setPen(QColor("#EF4444")); 
            painter->drawText(QRect(rect.left() + 2 * sectionWidth, yOffset, sectionWidth, 14), Qt::AlignCenter, QString::number(reject));
        }
        
        painter->restore();
    }
}