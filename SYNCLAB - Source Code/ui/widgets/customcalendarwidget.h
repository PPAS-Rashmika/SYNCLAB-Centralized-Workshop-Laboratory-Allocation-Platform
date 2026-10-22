// සාමාන්‍ය Qt calendar එකට booking ගණන් පෙන්වීම එක් කරන class එකයි.
// එකම header එක කිහිප වරක් include වුවත් definitions එකවරක් පමණක් යෙදීමට මේ guard එක ඇත.
#ifndef CUSTOMCALENDARWIDGET_H
#define CUSTOMCALENDARWIDGET_H

// #include වලින් මේ file එකට අවශ්‍ය Qt සහ project declarations ලබාගනී.
#include <QCalendarWidget>
#include <QDate>
#include <QMap>
#include <QPainter>

// Qt calendar එකේ paintCell() වෙනුවට අපේ function එක යොදා booking badges එක් කරයි.
class CustomCalendarWidget : public QCalendarWidget
{
    // Qt signals/slots හඳුනාගන්නා ලකුණයි.
    Q_OBJECT
    Q_PROPERTY(QColor approvedColor MEMBER approvedColor)
    Q_PROPERTY(QColor pendingColor MEMBER pendingColor)
    Q_PROPERTY(QColor rejectedColor MEMBER rejectedColor)
    Q_PROPERTY(QColor badgeTextColor MEMBER badgeTextColor)
    Q_PROPERTY(QColor headerBackground READ headerBackground WRITE setHeaderBackground)
    Q_PROPERTY(QColor headerForeground READ headerForeground WRITE setHeaderForeground)
    Q_PROPERTY(int badgeFontSize MEMBER badgeFontSize)

  // අනෙක් code වලින් භාවිත කළ හැකි functions.
  public:
    // Calendar දින නාම, මුල් සතියේ දිනය සහ අවම ප්‍රමාණය සකසයි.
    explicit CustomCalendarWidget(QWidget *parent = nullptr);
    QColor headerBackground() const;
    QColor headerForeground() const;
    void setHeaderBackground(const QColor &color);
    void setHeaderForeground(const QColor &color);

    // එක් දිනක status ගණන් save කර calendar දින නැවත අඳියි.
    void setStatusCounts(const QMap<QDate, QMap<QString, int>> &counts);

  // මෙම class එකට සහ එයින් ලබාගත් classes වලට භාවිත කළ හැකි functions.
  protected:
    // දින අංකය සහ A/P/R ගණන් එක calendar කොටුවක අඳියි.
    void paintCell(QPainter *painter, const QRect &rect, QDate date) const override;

  // මෙම class එක තුළ පමණක් භාවිත කරන functions සහ දත්ත.
  private:
    QColor approvedColor, pendingColor, rejectedColor, badgeTextColor;
    int badgeFontSize = 11;
    QMap<QDate, QMap<QString, int>> statusCounts; // දිනය -> status -> booking ගණන ලෙස දත්ත තබන map එක.
};

#endif
