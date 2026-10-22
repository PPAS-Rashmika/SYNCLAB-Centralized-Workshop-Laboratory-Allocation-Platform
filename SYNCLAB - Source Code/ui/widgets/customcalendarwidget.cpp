// Calendar දිනවල A/P/R booking ගණන් අඳියි. Badge පාට සහ ප්‍රමාණය මෙහි වෙනස් කරන්න.
// #include වලින් මේ file එකට අවශ්‍ය Qt සහ project declarations ලබාගනී.
#include "customcalendarwidget.h"

#include <QAbstractItemView>
#include <QFontMetrics>
#include <QPalette>
#include <QTextCharFormat>

// Calendar දින නාම, මුල් සතියේ දිනය සහ අවම ප්‍රමාණය සකසයි.
CustomCalendarWidget::CustomCalendarWidget(QWidget *parent) : QCalendarWidget(parent)
{
    setVerticalHeaderFormat(QCalendarWidget::NoVerticalHeader);
    setHorizontalHeaderFormat(QCalendarWidget::ShortDayNames);
    // සතියේ මුල් දිනය මෙහි තෝරයි; දැනට Monday වේ.
    setFirstDayOfWeek(Qt::Monday);
    setGridVisible(false);

    setAccessibleName(tr("Allocation calendar"));

    // Calendar දින නාම පෙන්වන ඉහළ bar එකේ පාට සහ font මෙහි සකසයි.

}

QColor CustomCalendarWidget::headerBackground() const { return headerTextFormat().background().color(); }
QColor CustomCalendarWidget::headerForeground() const { return headerTextFormat().foreground().color(); }
void CustomCalendarWidget::setHeaderBackground(const QColor &color)
{
    QTextCharFormat format = headerTextFormat();
    format.setBackground(color);
    format.setFontWeight(QFont::Bold);
    setHeaderTextFormat(format);
}
void CustomCalendarWidget::setHeaderForeground(const QColor &color)
{
    QTextCharFormat format = headerTextFormat();
    format.setForeground(color);
    setHeaderTextFormat(format);
}

// එක් දිනක status ගණන් save කර calendar දින නැවත අඳියි.
void CustomCalendarWidget::setStatusCounts(const QMap<QDate, QMap<QString, int>> &counts)
{
    statusCounts = counts;
    updateCells();
}

// දින අංකය සහ A/P/R ගණන් එක calendar කොටුවක අඳියි.
void CustomCalendarWidget::paintCell(QPainter *painter, const QRect &rect, QDate date) const
{
    const auto counts = statusCounts.value(date);
    const int approved = counts.value("Approve");
    const int pending = counts.value("Pending");
    const int rejected = counts.value("Reject");

    // Bookings නැති හෝ ඉඩ කුඩා දිනයක සාමාන්‍ය day number එක පමණක් අඳියි.
    if (approved + pending + rejected == 0 || rect.height() < 28) {
        // දින අංකය සහ A/P/R ගණන් එක calendar කොටුවක අඳියි.
        QCalendarWidget::paintCell(painter, rect, date);
        return;
    }

    // මේ cell එකේ drawing වෙනස්කම් අන් cells වලට බල නොපාන්න මුල් painter state තබයි.
    painter->save();
    painter->setClipRect(rect);
    // Status badge strip එකේ උසයි; වැඩි කළොත් date number සඳහා ඇති ඉඩත් ගළපන්න.
    const int badgeHeight = qMin(14, rect.height() / 3);

    // Selected date සහ වෙනම background colours රැකගෙන cell පසුබිම තෝරයි.
    const auto *view = findChild<QAbstractItemView *>("qt_calendar_calendarview");
    const QPalette colors = view ? view->palette() : palette();
    QBrush background = colors.base();
    const QBrush dateBackground = dateTextFormat(date).background();
    if (dateBackground.style() != Qt::NoBrush)
        background = dateBackground;
    if (date == selectedDate())
        background = colors.highlight();
    painter->fillRect(rect, background);

    // දින අංකය උඩ කොටසේ අඳියි; පහළින් badges සඳහා ඉඩ තබයි.
    // දින අංකය සහ A/P/R ගණන් එක calendar කොටුවක අඳියි.
    QCalendarWidget::paintCell(painter, rect.adjusted(0, 0, 0, -badgeHeight - 2), date);

    QFont badgeFont = painter->font();
    badgeFont.setPixelSize(qMax(1, qMin(badgeFontSize, badgeHeight - 3)));
    badgeFont.setBold(true);
    painter->setFont(badgeFont);
    painter->setRenderHint(QPainter::Antialiasing, true);

    const int values[] = {approved, pending, rejected};
    // A=Approve, P=Pending, R=Reject; එම අනුපිළිවෙළේ colours පහත ඇත.
    const QString prefixes[] = {"A", "P", "R"};
    // Badges තුනේ green/amber/red පාට මෙහි වෙනස් කරන්න.
    const QColor fills[] = {approvedColor, pendingColor, rejectedColor};
    const int columnWidth = rect.width() / 3;
    for (int column = 0; column < 3; ++column) {
        if (values[column] <= 0)
            continue;
        const QRect badge(rect.left() + column * columnWidth + 1, rect.bottom() - badgeHeight,
                          columnWidth - 2, badgeHeight);
        const QString value =
            // ගණන 99 ඉක්මවූ විට 99+ පෙන්වයි; ඉඩ මදි නම් A/P/R අකුර නොපෙන්වයි.
            values[column] > 99 ? QString("99+") : QString::number(values[column]);
        QString label = prefixes[column] + value;
        if (painter->fontMetrics().horizontalAdvance(label) > badge.width() - 2)
            label = value;
        painter->setPen(Qt::NoPen);
        painter->setBrush(fills[column]);
        painter->drawRoundedRect(badge, 3, 3);
        painter->setPen(badgeTextColor);
        painter->drawText(badge, Qt::AlignCenter, label);
    }
    painter->restore();
}
