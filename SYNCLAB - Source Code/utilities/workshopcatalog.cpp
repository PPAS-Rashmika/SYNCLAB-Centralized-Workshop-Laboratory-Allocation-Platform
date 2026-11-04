// Workshop නම්, departments සහ images තෝරයි. ලැයිස්තුව සහ department සම්බන්ධය මෙහි වෙනස් කරන්න.
// #include වලින් මේ file එකට අවශ්‍ය Qt සහ project declarations ලබාගනී.
#include "workshopcatalog.h"

#include <QColor>
#include <QFont>
#include <QPainter>
#include <QRect>

// Cards වල workshop අනුපිළිවෙළයි. මෙය වෙනස් කළොත් selection.ui හි category values ද ගළපන්න.
QStringList workshopCategories()
{
    return {"Biochemistry",
            "Electronics",
            "Computer",
            "Geotechnical",
            "Surveying",
            "Concrete",
            "Drawing Office",
            "Manufacturing",
            "Electrical Machines & Communications Laboratory",
            "Measurement Laboratory"};
}

// Workshop එකට පෙන්වන සම්පූර්ණ නම හදයි; අවශ්‍ය නම් Lab යන කොටස වෙනස් කරන්න.
QString workshopTitle(const QString &category)
{
    if (category == "Drawing Office")
        return category;
    if (category.endsWith(" Laboratory"))
        return category;
    return category + " Lab";
}

// Workshop එක අයත් department එක තෝරයි. Student ගේ department එක මෙයින් තීරණය නොකරයි.
QString workshopDepartment(const QString &category)
{
    if (QStringList{"Biochemistry", "Electronics", "Computer"}.contains(category))
        return "Department of Nano Science Technology";
    if (QStringList{"Geotechnical", "Surveying", "Concrete"}.contains(category))
        return "Department of Construction Technology";
    if (QStringList{"Electrical Machines & Communications Laboratory", "Measurement Laboratory"}.contains(category))
        return "Department of Electrotechnology";
    if (QStringList{"Drawing Office", "Manufacturing"}.contains(category))
        return "Department of Mechanical and Manufacturing Technology";
    return QString();
}

// Resource image එක label ප්‍රමාණයට ගළපයි; image නැත්නම් අකුරු සහිත badge එකක් අඳියි.
QPixmap workshopImage(const QString &category, const QSize &size)
{
    // සැබෑ image path resources/images.qrc හි ඇත; :/workshops/... යනු එහි alias එකයි.
    const QPixmap supplied(QString(":/workshops/%1.png").arg(category));
    if (!supplied.isNull())
        return supplied.scaled(size, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    // Image නැත්නම් පැහැදිලි badge එකක් සඳහා දෙගුණයක් resolution එකෙන් අඳියි.
    QPixmap image(size * 2);
    image.setDevicePixelRatio(2);
    image.fill(Qt::transparent);

    QPainter painter(&image);
    painter.setRenderHint(QPainter::Antialiasing);

    const int position = qMax(0, workshopCategories().indexOf(category));
    // Workshop අනුව badge පාට වෙනස් කරයි; HSV අගයන් මෙහි වෙනස් කළ හැක.
    const QColor accent = QColor::fromHsv((210 + position * 19) % 360, 170, 195);
    QColor soft = accent;
    soft.setAlpha(24);

    // Badge එකේ ප්‍රමාණය සහ කොන් වටකුරු කිරීමේ අගයන් පහත ඇත.
    const int diameter = qMin(size.width(), size.height()) - 8;
    QRect circle((size.width() - diameter) / 2, 4, diameter, diameter);
    painter.setPen(Qt::NoPen);
    painter.setBrush(soft);
    painter.drawRoundedRect(circle, 20, 20);

    // Workshop වචනවල මුල් අකුරු එකතු කරයි; Drawing Office සඳහා DO වේ.
    QString initials;
    for (const QString &word : category.split(' ', Qt::SkipEmptyParts))
        initials += word.left(1);
    if (category == "SYNCLAB")
        initials = "SL";

    painter.setPen(accent);
    // Badge අකුරු font එක සහ ප්‍රමාණය මෙහි වෙනස් කරන්න.
    painter.setFont(QFont("Segoe UI", diameter / 4, QFont::Bold));
    painter.drawText(circle, Qt::AlignCenter, initials.left(2));
    painter.end();
    return image;
}
