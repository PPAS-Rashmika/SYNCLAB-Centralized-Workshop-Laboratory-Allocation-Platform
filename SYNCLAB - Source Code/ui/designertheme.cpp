// Styles are authored in Qt Designer: styleSheet (light) and darkStyleSheet (dark).
#include "designertheme.h"
#include <QVariant>
#include <QWidget>

void applyDesignerTheme(QWidget *root, bool darkMode)
{
    QList<QWidget *> widgets = root->findChildren<QWidget *>();
    widgets.prepend(root);
    for (QWidget *widget : widgets) {
        if (!widget->property("_designerStyleSheet").isValid())
            widget->setProperty("_designerStyleSheet", widget->styleSheet());
        const QString light = widget->property("_designerStyleSheet").toString();
        const QVariant dark = widget->property("darkStyleSheet");
        const QString style = darkMode && dark.isValid() ? dark.toString() : light;
        if (widget->styleSheet() != style)
            widget->setStyleSheet(style);
    }
}
