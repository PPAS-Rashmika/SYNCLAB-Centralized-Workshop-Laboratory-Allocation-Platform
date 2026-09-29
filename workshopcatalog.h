#ifndef WORKSHOPCATALOG_H
#define WORKSHOPCATALOG_H

#include <QStringList>

inline QStringList workshopCategories()
{
    return {"Biochemistry", "Electronics", "Computer", "Advanced Material",
            "Geotechnical", "Surveying", "Concrete", "Physics", "Drawing Office", "Manufacturing",
            "Environment and Water Resources Laboratory", "Highway Laboratory", "Structures Laboratory",
            "Electrical Machines & Communications Laboratory", "Measurement Laboratory"};
}

inline QString workshopTitle(const QString &category)
{
    return category == "Drawing Office" || category.endsWith(" Laboratory") ? category : category + " Lab";
}

inline QString workshopDepartment(const QString &category)
{
    const int index = workshopCategories().indexOf(category);
    if (index < 0) return {};
    if (index < 4) return "Department of Nano Science Technology";
    if (index < 7 || (index >= 10 && index <= 12)) return "Department of Construction Technology";
    if (index >= 13) return "Department of Electrotechnology";
    return "Department of Mechanical and Manufacturing Technology";
}

#endif
