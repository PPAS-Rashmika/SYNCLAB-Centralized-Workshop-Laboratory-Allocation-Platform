#include "designertheme.h"

#include <QDialog>
#include <QHash>
#include <QPalette>
#include <QRegularExpression>
#include <QVariant>
#include <QWidget>

namespace {
QString recolor(const QString &source, bool dialog)
{
    const QHash<QString, QString> replacements = {
        {"#f4f7fb", "#0F172A"}, {"#f8fafc", "#0F172A"}, {"#f1f5f9", "#0F172A"},
        {"#ffffff", dialog ? "#172033" : "#1E293B"},
        {"#17243b", "#F1F5F9"}, {"#0f172a", "#F1F5F9"},
        {"#52647d", "#AABAD0"}, {"#64748b", "#AABAD0"},
        {"#d4deeb", "#43536B"}, {"#e2e8f0", "#43536B"}, {"#cbd5e1", "#475569"},
        {"#eaf2ff", "#253C61"}, {"#eff6ff", "#253C61"}, {"#bfdbfe", "#43536B"},
        {"#be123c", "#FDA4AF"}
    };
    static const QRegularExpression color("#[0-9a-fA-F]{6}(?![0-9a-fA-F])");
    QString result;
    qsizetype position = 0;
    auto matches = color.globalMatch(source);
    while (matches.hasNext()) {
        const auto match = matches.next();
        result += source.mid(position, match.capturedStart() - position);
        result += replacements.value(match.captured().toLower(), match.captured());
        position = match.capturedEnd();
    }
    result += source.mid(position);
    // A blue caption needs a lighter shade on dark surfaces. Blue button and
    // selection backgrounds retain their original white-text contrast.
    static const QRegularExpression accentText(
        "(?<![\\w-])(color\\s*:\\s*)#2563EB(?![0-9a-fA-F])",
        QRegularExpression::CaseInsensitiveOption);
    result.replace(accentText, "\\1#8DB8FF");
    return result;
}
}

void applyDesignerTheme(QWidget *root, bool darkMode)
{
    const bool dialog = qobject_cast<QDialog *>(root) != nullptr;
    QPalette palette = root->palette();
    palette.setColor(QPalette::Window, darkMode ? QColor(dialog ? "#172033" : "#0F172A") : QColor("#F4F7FB"));
    palette.setColor(QPalette::WindowText, darkMode ? QColor("#F1F5F9") : QColor("#17243B"));
    palette.setColor(QPalette::Base, darkMode ? QColor("#1E293B") : QColor("#FFFFFF"));
    palette.setColor(QPalette::AlternateBase, darkMode ? QColor("#0F172A") : QColor("#F4F7FB"));
    palette.setColor(QPalette::Text, palette.color(QPalette::WindowText));
    palette.setColor(QPalette::Button, palette.color(QPalette::Base));
    palette.setColor(QPalette::ButtonText, palette.color(QPalette::WindowText));
    palette.setColor(QPalette::Highlight, QColor("#2563EB"));
    palette.setColor(QPalette::HighlightedText, Qt::white);
    palette.setColor(QPalette::PlaceholderText, darkMode ? QColor("#AABAD0") : QColor("#52647D"));
    palette.setColor(QPalette::Disabled, QPalette::Text, palette.color(QPalette::PlaceholderText));
    palette.setColor(QPalette::Disabled, QPalette::ButtonText, palette.color(QPalette::PlaceholderText));
    root->setPalette(palette);

    QList<QWidget *> widgets = root->findChildren<QWidget *>();
    widgets.prepend(root);
    for (QWidget *widget : widgets) {
        // The snapshot is per-widget so individual Designer style edits survive.
        if (!widget->property("_designerStyleSheet").isValid())
            widget->setProperty("_designerStyleSheet", widget->styleSheet());
        const QString original = widget->property("_designerStyleSheet").toString();
        const QString style = darkMode ? recolor(original, dialog) : original;
        if (widget->styleSheet() != style)
            widget->setStyleSheet(style);
    }
}
