#include "workshopcatalog.h"

#include <QColor>
#include <QFont>
#include <QPainter>
#include <QRect>

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

QString workshopTitle(const QString &category)
{
    // Two workshops already carry their full name, the others simply add " Lab".
    if (category == "Drawing Office")
        return category;
    if (category.endsWith(" Laboratory"))
        return category;
    return category + " Lab";
}

QString workshopDepartment(const QString &category)
{
    // Explicit membership keeps the owning department stable when cards are reordered.
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

QPixmap workshopImage(const QString &category, const QSize &size)
{
    // resources/images.qrc embeds pictures from "resources/workshops and laboratory"
    // under /workshops, so they are found from any run directory.
    const QPixmap supplied(QString(":/workshops/%1.png").arg(category));
    if (!supplied.isNull())
        return supplied.scaled(size, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    // Otherwise draw a badge: a soft rounded square with the initials on top.
    // The picture is drawn at twice the size so it stays sharp on high DPI screens.
    QPixmap image(size * 2);
    image.setDevicePixelRatio(2);
    image.fill(Qt::transparent);

    QPainter painter(&image);
    painter.setRenderHint(QPainter::Antialiasing);

    // Every workshop gets its own colour by moving around the HSV colour wheel.
    const int position = qMax(0, workshopCategories().indexOf(category));
    const QColor accent = QColor::fromHsv((210 + position * 19) % 360, 170, 195);
    QColor soft = accent;
    soft.setAlpha(24);

    const int diameter = qMin(size.width(), size.height()) - 8;
    QRect circle((size.width() - diameter) / 2, 4, diameter, diameter);
    painter.setPen(Qt::NoPen);
    painter.setBrush(soft);
    painter.drawRoundedRect(circle, 20, 20);

    // Take the first letter of each word, for example "Drawing Office" -> "DO".
    QString initials;
    for (const QString &word : category.split(' ', Qt::SkipEmptyParts))
        initials += word.left(1);
    if (category == "SYNCLAB")
        initials = "SL";

    painter.setPen(accent);
    painter.setFont(QFont("Segoe UI", diameter / 4, QFont::Bold));
    painter.drawText(circle, Qt::AlignCenter, initials.left(2));
    painter.end();
    return image;
}
