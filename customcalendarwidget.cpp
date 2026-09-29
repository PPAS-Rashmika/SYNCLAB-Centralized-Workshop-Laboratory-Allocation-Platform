#include "customcalendarwidget.h"

#include <QAbstractItemView>
#include <QFontMetrics>
#include <QPalette>
#include <QTextCharFormat>

CustomCalendarWidget::CustomCalendarWidget(QWidget *parent)
    : QCalendarWidget(parent)
{
    setVerticalHeaderFormat(QCalendarWidget::NoVerticalHeader);
    setHorizontalHeaderFormat(QCalendarWidget::ShortDayNames);
    setFirstDayOfWeek(Qt::Monday);
    setGridVisible(false);
    setMinimumSize(320, 240);
    setAccessibleName(tr("Booking calendar"));
}

void CustomCalendarWidget::setStatusCounts(const QMap<QDate, QMap<QString, int>> &counts)
{
    m_counts = counts;
    updateCells();
}

void CustomCalendarWidget::paintCell(QPainter *painter, const QRect &rect, QDate date) const
{
    const auto counts = m_counts.value(date);
    const int approved = counts.value("Approve");
    const int pending = counts.value("Pending");
    const int rejected = counts.value("Reject");
    if (approved + pending + rejected == 0 || rect.height() < 28) {
        QCalendarWidget::paintCell(painter, rect, date);
        return;
    }

    painter->save();
    painter->setClipRect(rect);
    const int badgeHeight = qMin(14, rect.height() / 3);
    const auto *view = findChild<QAbstractItemView *>("qt_calendar_calendarview");
    const QPalette colors = view ? view->palette() : palette();
    QBrush background = colors.base();
    const QBrush dateBackground = dateTextFormat(date).background();
    if (dateBackground.style() != Qt::NoBrush)
        background = dateBackground;
    if (date == selectedDate())
        background = colors.highlight();
    painter->fillRect(rect, background);

    // Reserve a separate strip so status counts never cover the day number.
    QCalendarWidget::paintCell(painter, rect.adjusted(0, 0, 0, -badgeHeight - 2), date);
    QFont badgeFont = painter->font();
    badgeFont.setPixelSize(qMax(8, badgeHeight - 3));
    badgeFont.setBold(true);
    painter->setFont(badgeFont);
    painter->setRenderHint(QPainter::Antialiasing, true);

    const int values[] = {approved, pending, rejected};
    const QString prefixes[] = {QStringLiteral("A"), QStringLiteral("P"), QStringLiteral("R")};
    const QColor fills[] = {QColor("#166534"), QColor("#92400E"), QColor("#B91C1C")};
    const int columnWidth = rect.width() / 3;
    for (int column = 0; column < 3; ++column) {
        if (values[column] <= 0) continue;
        const QRect badge(rect.left() + column * columnWidth + 1,
                          rect.bottom() - badgeHeight,
                          columnWidth - 2, badgeHeight);
        const QString value = values[column] > 99 ? QStringLiteral("99+") : QString::number(values[column]);
        QString label = prefixes[column] + value;
        if (painter->fontMetrics().horizontalAdvance(label) > badge.width() - 2)
            label = value;
        painter->setPen(Qt::NoPen);
        painter->setBrush(fills[column]);
        painter->drawRoundedRect(badge, 3, 3);
        painter->setPen(Qt::white);
        painter->drawText(badge, Qt::AlignCenter, label);
    }
    painter->restore();
}