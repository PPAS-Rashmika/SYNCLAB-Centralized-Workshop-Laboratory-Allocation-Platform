// Displays availability and counts; SQLite remains responsible for the final conflict check.
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
#include <QTextCharFormat>

void MainWindow::dateClicked(const QDate &)
{
    onCalendarSelectionChanged();
}

QStringList MainWindow::getSelectedTimeSlotsList()
{
    QStringList selectedSlots;
    for (QPushButton *button : {ui2->timeBtn1, ui2->timeBtn2, ui2->timeBtn3, ui2->timeBtn4})
        if (button->isChecked() && button->isEnabled())
            selectedSlots.append(button->property("timeVal").toString());
    return selectedSlots;
}

void MainWindow::setTimeSlotsInUI(const QString &timeString)
{
    QStringList selectedSlots = timeString.split(',', Qt::SkipEmptyParts);
    for (QString &slot : selectedSlots)
        slot = slot.trimmed();
    for (QPushButton *button : {ui2->timeBtn1, ui2->timeBtn2, ui2->timeBtn3, ui2->timeBtn4})
        button->setChecked(button->isEnabled() &&
                           selectedSlots.contains(button->property("timeVal").toString()));
    onTimeSlotClicked();
}

void MainWindow::onTimeSlotClicked()
{
    ui2->nextTo3Btn->setEnabled(!getSelectedTimeSlotsList().isEmpty());
}

void MainWindow::onCalendarSelectionChanged()
{
    if (currentCategory.isEmpty()) {
        ui2->nextTo3Btn->setEnabled(false);
        return;
    }
    const QDate date = ui2->calendar->selectedDate();
    const auto booked = dbManager->getBookedTimeSlotsStringList(
        date.toString(Qt::ISODate), currentCategory, currentRecord.getId());
    const bool queryOk = dbManager->getLastError().isEmpty();
    for (QPushButton *button : {ui2->timeBtn1, ui2->timeBtn2, ui2->timeBtn3, ui2->timeBtn4}) {
        const bool available = queryOk && date >= QDate::currentDate() &&
                               !booked.contains(button->property("timeVal").toString());
        button->setEnabled(available);
        if (!available)
            button->setChecked(false);
        button->setToolTip(
            !queryOk ? "Availability could not be loaded"
                     : (available ? "Select this time slot" : "This time slot is unavailable"));
    }
    onTimeSlotClicked();
}

void MainWindow::updateCalendarHighlights()
{
    if (!m_databaseReady || currentCategory.isEmpty())
        return;
    ui2->calendar->setDateTextFormat(QDate(), QTextCharFormat());
    QMap<QDate, QMap<QString, int>> counts;
    for (const auto &record : dbManager->getAllRecordsForCategory(currentCategory)) {
        const QDate date = QDate::fromString(record.getRecordDate(), Qt::ISODate);
        if (date.isValid())
            counts[date][record.getStatus()]++;
    }
    ui2->calendar->setStatusCounts(counts);
}

void MainWindow::updateAdminCalendarHighlights()
{
    if (!m_databaseReady || currentCategory.isEmpty())
        return;
    ui4->adminCalendar->setDateTextFormat(QDate(), QTextCharFormat());
    QMap<QDate, QMap<QString, int>> counts;
    for (const auto &record : dbManager->getAllRecordsForCategory(currentCategory)) {
        const QDate date = QDate::fromString(record.getRecordDate(), Qt::ISODate);
        if (date.isValid())
            counts[date][record.getStatus()]++;
    }
    ui4->adminCalendar->setStatusCounts(counts);
}
