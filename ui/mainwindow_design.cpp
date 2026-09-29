// Keeps the existing colours, images, workshop cards, and responsive layout.
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "ui_page0.h"
#include "ui_page1.h"
#include "ui_page2.h"
#include "ui_page3.h"
#include "ui_page4.h"
#include "customdialogs.h"
#include "workshopcatalog.h"
#include "apphelpers.h"
#include <QDir>
#include <QFileInfo>
#include <QResizeEvent>
#include <QScrollArea>
#include <QSettings>
#include <QTimer>

void MainWindow::applyTheme(bool isDark)
{
    m_isDark = isDark;
    for (QWidget *widget : findChildren<QWidget *>())
        widget->setStyleSheet({});
    const QString bg = isDark ? "#0F172A" : "#F4F7FB";
    const QString surface = isDark ? "#1E293B" : "#FFFFFF";
    const QString text = isDark ? "#F1F5F9" : "#17243B";
    const QString muted = isDark ? "#AABAD0" : "#52647D";
    const QString border = isDark ? "#43536B" : "#D4DEEB";
    const QString accent = isDark ? "#8DB8FF" : "#2563EB";
    const QString soft = isDark ? "#253C61" : "#EAF2FF";
    QPalette palette;
    palette.setColor(QPalette::Window, QColor(bg));
    palette.setColor(QPalette::WindowText, QColor(text));
    palette.setColor(QPalette::Base, QColor(surface));
    palette.setColor(QPalette::AlternateBase, QColor(bg));
    palette.setColor(QPalette::Text, QColor(text));
    palette.setColor(QPalette::Button, QColor(surface));
    palette.setColor(QPalette::ButtonText, QColor(text));
    palette.setColor(QPalette::Highlight, QColor("#2563EB"));
    palette.setColor(QPalette::HighlightedText, Qt::white);
    palette.setColor(QPalette::PlaceholderText, QColor(muted));
    palette.setColor(QPalette::Disabled, QPalette::Text, QColor(muted));
    palette.setColor(QPalette::Disabled, QPalette::ButtonText, QColor(muted));
    setPalette(palette);
    setStyleSheet(
        QString("QWidget { color:%1; font-family:'Segoe UI'; font-size:13px; }"
                "QMainWindow, QStackedWidget, QWidget#page0Widget, QWidget#page1Widget, "
                "QWidget#page2Widget, QWidget#page3Widget, QWidget#page4Widget { background:%2; }"
                "QScrollArea { border:none; background:transparent; } QScrollArea > QWidget > "
                "QWidget { background:%2; }"
                "QLabel { background:transparent; }"
                "QLabel#welcomeTitle { color:%4; font-size:16px; font-weight:600; }"
                "QLabel#syncLabTitle { font-size:48px; font-weight:800; }"
                "QLabel#welcomeDesc, QLabel#selectedDescLabel2, QLabel#adminWsDescLbl { color:%4; "
                "font-size:14px; }"
                "QLabel#selectTitleLabel, QLabel#selectedNameLabel2, QLabel#confirmTitleLbl, "
                "QLabel#adminWsTitleLbl { font-size:25px; font-weight:700; }"
                "QLineEdit, QTextEdit, QComboBox { background:%3; border:1px solid %5; "
                "border-radius:7px; padding:8px; selection-background-color:#2563EB; "
                "selection-color:white; }"
                "QLineEdit:focus, QTextEdit:focus, QComboBox:focus { border:1px solid %6; }"
                "QComboBox QAbstractItemView { background:%3; color:%1; "
                "selection-background-color:#2563EB; selection-color:white; }"
                "QPushButton { background:%3; color:%1; border:1px solid %5; border-radius:8px; "
                "padding:9px 14px; font-weight:600; }"
                "QPushButton:hover, QPushButton:focus { background:%7; border-color:%6; }"
                "QPushButton:pressed { border:2px solid %6; }"
                "QPushButton#allocateNowBtn, QPushButton#nextBtn, QPushButton#nextTo3Btn, "
                "QPushButton#submitBtn, QPushButton#updateBtn { background:#2563EB; color:white; "
                "border:1px solid #2563EB; }"
                "QPushButton#allocateNowBtn:hover, QPushButton#nextBtn:hover, "
                "QPushButton#nextTo3Btn:hover, QPushButton#submitBtn:hover, "
                "QPushButton#updateBtn:hover { background:#1D4ED8; }"
                "QPushButton#deleteBtn, QPushButton#adminDelBtn { color:%8; }"
                "QPushButton[timeSlot=true] { padding:10px 8px; }"
                "QPushButton:checked { background:#2563EB; color:white; border-color:#2563EB; }"
                "QPushButton:disabled { background:%2; color:%4; border:1px solid %5; }"
                "QPushButton#allocateNowBtn:disabled, QPushButton#nextBtn:disabled, "
                "QPushButton#nextTo3Btn:disabled, QPushButton#submitBtn:disabled, "
                "QPushButton#updateBtn:disabled, QPushButton#deleteBtn:disabled, "
                "QPushButton#adminDelBtn:disabled { background:%2; color:%4; border:1px solid %5; }"
                "QFrame#summaryBox { background:%7; border:1px solid %5; border-radius:10px; }"
                "QLabel#summaryDateLbl { color:%6; font-weight:700; }"
                "QTableWidget { background:%3; alternate-background-color:%2; border:1px solid %5; "
                "gridline-color:%5; selection-background-color:%7; selection-color:%1; }"
                "QHeaderView::section { background:%7; color:%1; border:none; padding:10px; "
                "font-weight:600; }"
                "QTableWidget QLineEdit, QTableWidget QComboBox { padding:4px; border-radius:4px; }"
                "QCalendarWidget { background:%3; border:1px solid %5; border-radius:8px; }"
                "QCalendarWidget QWidget#qt_calendar_navigationbar { background:#2563EB; }"
                "QCalendarWidget QToolButton { color:white; background:transparent; padding:6px; "
                "font-weight:600; }"
                "QCalendarWidget QMenu, QCalendarWidget QSpinBox { background:%3; color:%1; }"
                "QCalendarWidget QAbstractItemView { background:%3; color:%1; "
                "selection-background-color:#2563EB; selection-color:white; outline:0; }"
                "QToolTip { background:%3; color:%1; border:1px solid %5; padding:6px; }")
            .arg(text, bg, surface, muted, border, accent, soft, isDark ? "#FDA4AF" : "#BE123C"));
    ui0->welcomeImg->setPixmap(workshopImage("SYNCLAB", QSize(110, 110)));
    QString wallpaper = QSettings(QSettings::defaultFormat(), QSettings::UserScope,
                                  "StudentWorkshopApp", "ThemeSettings")
                            .value("wallpaper")
                            .toString();
    if (QFileInfo::exists(wallpaper)) {
        wallpaper = QDir::fromNativeSeparators(QFileInfo(wallpaper).absoluteFilePath());
        wallpaper.replace('"', "\\\"");
        for (QWidget *page : {page0Widget, page1Widget, page2Widget, page3Widget, page4Widget})
            page->setStyleSheet(
                QString("QWidget#%1 { border-image:url(\"%2\") 0 0 0 0 stretch stretch; }")
                    .arg(page->objectName(), wallpaper));
    }
    if (m_cardFrames.isEmpty())
        drawCards(isDark);
    else
        selectCard(m_selectedCategory, nullptr);
    updateCalendarHighlights();
    updateAdminCalendarHighlights();
}

QWidget *MainWindow::createCard(const QString &id, const QString &title, const QString &subtitle,
                                bool)
{
    auto *card = new QFrame(page1Widget);
    card->setObjectName("cardFrame");
    card->setProperty("category", id);
    card->setFixedSize(212, 210);
    auto *layout = new QVBoxLayout(card);
    layout->setContentsMargins(14, 14, 14, 14);
    layout->setSpacing(8);
    auto *image = new QLabel(card);
    image->setPixmap(workshopImage(id, QSize(110, 80)));
    image->setAlignment(Qt::AlignCenter);
    auto *heading = new QLabel(title, card);
    heading->setAlignment(Qt::AlignCenter);
    heading->setWordWrap(true);
    QFont font = heading->font();
    font.setBold(true);
    heading->setFont(font);
    auto *description = new QLabel(subtitle, card);
    description->setAlignment(Qt::AlignCenter);
    description->setWordWrap(true);
    layout->addWidget(image);
    layout->addWidget(heading);
    layout->addWidget(description, 1);
    auto *overlay = new QPushButton(card);
    overlay->setObjectName("workshopCard_" + QString(id).replace(' ', '_'));
    overlay->setAccessibleName(title);
    overlay->setToolTip(title + "\n" + subtitle);
    overlay->setGeometry(card->rect());
    overlay->setCursor(Qt::PointingHandCursor);
    overlay->raise();
    connect(overlay, &QPushButton::clicked, this, [this, id, card] { selectCard(id, card); });
    m_cardFrames.append(card);
    return card;
}

void MainWindow::drawCards(bool)
{
    for (const auto &category : workshopCategories())
        createCard(category, workshopTitle(category), workshopDepartment(category), m_isDark);
    m_cardColumns = 0;
    arrangeCards();
    selectCard(m_selectedCategory, nullptr);
}

void MainWindow::selectCard(const QString &category, QFrame *)
{
    m_selectedCategory = category;
    for (QFrame *card : m_cardFrames) {
        const bool selected = card->property("category").toString() == category;
        card->setStyleSheet(
            QString("QFrame#cardFrame { background:%1; border:2px solid %2; border-radius:12px; }")
                .arg(selected ? (m_isDark ? "#253C61" : "#EAF2FF")
                              : (m_isDark ? "#1E293B" : "#FFFFFF"),
                     selected ? "#6099F5" : (m_isDark ? "#43536B" : "#D4DEEB")));
        auto *button = card->findChild<QPushButton *>();
        button->setStyleSheet("QPushButton { background:transparent; border:2px solid transparent; "
                              "border-radius:12px; } QPushButton:hover, QPushButton:focus { "
                              "border:2px solid #6099F5; background:rgba(80,140,240,18); }");
        button->setAccessibleDescription(selected ? "Selected workshop" : "Select workshop");
    }
    ui1->nextBtn->setEnabled(!category.isEmpty());
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);
    QTimer::singleShot(0, this, &MainWindow::arrangeCards);
}

void MainWindow::arrangeCards()
{
    if (!page1Widget)
        return;
    const auto *area = page1Widget->findChild<QScrollArea *>("cardsScrollArea");
    const int available = area ? area->viewport()->width() : width() - 70;
    const int columns = qBound(1, (available - 12) / 228, 5);
    if (columns == m_cardColumns)
        return;
    m_cardColumns = columns;
    while (auto *item = ui1->cardsLayout->takeAt(0))
        delete item;
    // Keep the additional laboratories below the original ten, even on narrower windows.
    constexpr int originalWorkshopCount = 10;
    const int additionalRow = (originalWorkshopCount + columns - 1) / columns;
    for (int i = 0; i < m_cardFrames.size(); ++i) {
        const int offset = i < originalWorkshopCount ? i : i - originalWorkshopCount;
        const int row =
            i < originalWorkshopCount ? offset / columns : additionalRow + offset / columns;
        ui1->cardsLayout->addWidget(m_cardFrames[i], row, offset % columns);
    }
}
