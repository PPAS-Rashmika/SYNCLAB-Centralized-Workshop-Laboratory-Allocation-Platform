#include "customcalendarwidget.h"

CustomCalendarWidget::CustomCalendarWidget(QWidget *parent) : QCalendarWidget(parent) {}

void CustomCalendarWidget::setStatusCounts(const QMap<QDate, QMap<QString, int>> &counts)
{
    m_counts = counts;
    updateCells(); // දත්ත වෙනස් වූ විට Calendar එක Refresh කිරීම
}

void CustomCalendarWidget::paintCell(QPainter *painter, const QRect &rect, const QDate &date) const
{
    // සාමාන්‍ය විදිහට මුලින්ම Date එක සහ Background එක ඇඳීම
    QCalendarWidget::paintCell(painter, rect, date);

    // එම දිනයට අදාළව දත්ත ඇත්නම් යටින් ඉලක්කම් ඇඳීම
    if (m_counts.contains(date)) {
        QMap<QString, int> counts = m_counts.value(date);
        int approve = counts.value("Approve", 0);
        int pending = counts.value("Pending", 0);
        int reject = counts.value("Reject", 0);

        painter->save();
        QFont font = painter->font();
        font.setPointSize(7); // කුඩා අකුරු ප්‍රමාණයක්
        font.setBold(true);
        painter->setFont(font);

        int sectionWidth = rect.width() / 3;
        int yOffset = rect.bottom() - 12; // කොටුවේ පහළින්ම ස්ථානගත කිරීම

        // Green (Approve)
        if (approve > 0) {
            painter->setPen(QColor("#10B981")); 
            painter->drawText(QRect(rect.left(), yOffset, sectionWidth, 14), Qt::AlignCenter, QString::number(approve));
        }
        // Orange (Pending)
        if (pending > 0) {
            painter->setPen(QColor("#F59E0B")); 
            painter->drawText(QRect(rect.left() + sectionWidth, yOffset, sectionWidth, 14), Qt::AlignCenter, QString::number(pending));
        }
        // Red (Reject)
        if (reject > 0) {
            painter->setPen(QColor("#EF4444")); 
            painter->drawText(QRect(rect.left() + 2 * sectionWidth, yOffset, sectionWidth, 14), Qt::AlignCenter, QString::number(reject));
        }
        
        painter->restore();
    }
}