#include "customcalendarwidget.h"

#include <QAbstractItemView>
#include <QFontMetrics>
#include <QPalette>
#include <QTextCharFormat>

// Constructor: fixes the look of the calendar once, when it is created.
CustomCalendarWidget::CustomCalendarWidget(QWidget *parent) : QCalendarWidget(parent)
{
    setVerticalHeaderFormat(QCalendarWidget::NoVerticalHeader);
    setHorizontalHeaderFormat(QCalendarWidget::ShortDayNames);
    setFirstDayOfWeek(Qt::Monday);
    setGridVisible(false);
    setMinimumSize(320, 240);
    setAccessibleName(tr("Booking calendar"));

    // The day bar is the strip of weekday names above the dates. Qt paints it
    // with the "header text format", so changing that format recolours the bar.
    // Qt always draws the Saturday and Sunday names in red, so the bar stays a
    // light colour to keep those two names readable. Change these three lines
    // to use a different colour.
    QTextCharFormat dayBar;
    dayBar.setBackground(QColor("#DBEAFE")); // light blue strip above the dates
    dayBar.setForeground(QColor("#1E40AF")); // dark blue weekday names
    dayBar.setFontWeight(QFont::Bold);
    setHeaderTextFormat(dayBar);
}

// Stores new counts and asks Qt to redraw every day cell.
void CustomCalendarWidget::setStatusCounts(const QMap<QDate, QMap<QString, int>> &counts)
{
    statusCounts = counts;
    updateCells();
}

// Qt calls this once for every day cell that needs drawing.
void CustomCalendarWidget::paintCell(QPainter *painter, const QRect &rect, QDate date) const
{
    const auto counts = statusCounts.value(date);
    const int approved = counts.value("Approve");
    const int pending = counts.value("Pending");
    const int rejected = counts.value("Reject");

    // Nothing to add, or the cell is too small for a badge: draw the plain day.
    if (approved + pending + rejected == 0 || rect.height() < 28) {
        QCalendarWidget::paintCell(painter, rect, date);
        return;
    }

    // save() and restore() make sure our drawing does not leak into other cells.
    painter->save();
    painter->setClipRect(rect);
    const int badgeHeight = qMin(14, rect.height() / 3);

    // Work out the background colour the cell should already have.
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

    // The three badges sit side by side along the bottom of the cell.
    const int values[] = {approved, pending, rejected};
    const QString prefixes[] = {"A", "P", "R"};
    const QColor fills[] = {QColor("#166534"), QColor("#92400E"), QColor("#B91C1C")};
    const int columnWidth = rect.width() / 3;
    for (int column = 0; column < 3; ++column) {
        if (values[column] <= 0)
            continue;
        const QRect badge(rect.left() + column * columnWidth + 1, rect.bottom() - badgeHeight,
                          columnWidth - 2, badgeHeight);
        // Very large numbers are shortened so the text still fits.
        const QString value =
            values[column] > 99 ? QString("99+") : QString::number(values[column]);
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
