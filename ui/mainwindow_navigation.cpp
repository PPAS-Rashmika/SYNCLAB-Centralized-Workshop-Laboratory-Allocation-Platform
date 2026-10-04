// Moves between pages and starts or cancels a booking workflow.
#include "mainwindow.h"
#include <QStackedWidget>
#include "ui_welcome.h"
#include "ui_selection.h"
#include "ui_calander.h"
#include "ui_form.h"
#include "ui_admin.h"
#include "customdialogs.h"
#include "workshopcatalog.h"
#include "apphelpers.h"

void MainWindow::resetFlow()
{
    currentRecord = StudentRecord();
    currentCategory.clear();
    currentWorkshopId.clear();
    isPasscodeBypassed = false;
    isAdminEditMode = false;
    isStudentEditFlow = false;
    adminFilterDate = QDate();
    calanderUi->calendar->setMinimumDate(QDate::currentDate());
    calanderUi->calendar->setSelectedDate(QDate::currentDate());
    clearUI();
    selectCard({}, nullptr);
    calanderUi->adminBtn2->show();
    calanderUi->backTo1Btn->setText("Back to workshops");
    setFormButtonsMode(false);
}

void MainWindow::startNewBooking()
{
    if (!m_databaseReady)
        return;
    resetFlow();
    stackedWidget->setCurrentIndex(1);
    arrangeCards();
}

void MainWindow::startEditBookingFlow()
{
    if (!m_databaseReady)
        return;
    CustomInputDialog dialog("Edit booking", "Enter your allocation key:", this);
    if (dialog.exec() != QDialog::Accepted)
        return;
    const QString entered = dialog.getInput();
    QList<StudentRecord> matches;
    for (const auto &category : workshopCategories()) {
        const auto records = dbManager->getAllRecordsForCategory(category);
        if (!dbManager->getLastError().isEmpty()) {
            showDatabaseError();
            return;
        }
        for (const auto &record : records)
            if (record.getPasscode().compare(entered, Qt::CaseInsensitive) == 0)
                matches.append(record);
    }
    if (matches.size() > 1) {
        CustomInputDialog identity(
            "Confirm booking",
            "This allocation key matches multiple bookings.\nEnter your student index:",
            this);
        if (identity.exec() != QDialog::Accepted)
            return;
        const QString identifier = identity.getInput();
        for (auto it = matches.begin(); it != matches.end();)
            if (it->getIndexNo() != identifier)
                it = matches.erase(it);
            else
                ++it;
    }
    if (matches.size() != 1) {
        CustomAlertDialog("Booking not found",
                          "The details do not identify a single booking. Check your allocation key "
                          "or contact the administrator.",
                          CustomAlertDialog::Error, this)
            .exec();
        return;
    }
    resetFlow();
    currentRecord = matches.first();
    currentCategory = currentRecord.getCategory();
    currentWorkshopId = currentCategory;
    isStudentEditFlow = true;
    isPasscodeBypassed = true;
    calanderUi->adminBtn2->hide();
    loadRecordIntoUI(currentRecord);
    selectCard(currentCategory, nullptr);
    const QDate savedDate = QDate::fromString(currentRecord.getRecordDate(), Qt::ISODate);
    // Historical bookings must still expose the delete action and editable details.
    stackedWidget->setCurrentIndex(savedDate < QDate::currentDate() ? 3 : 1);
    arrangeCards();
}

void MainWindow::cancelCurrentFlow()
{
    const bool hasDraft =
        isStudentEditFlow || isAdminEditMode || !formUi->nameInput->text().isEmpty() ||
        !formUi->indexInput->text().isEmpty() || !formUi->emailInput->text().isEmpty() ||
        !formUi->descriptionInput->toPlainText().isEmpty() || !formUi->pdfInput->text().isEmpty();
    if (hasDraft &&
        CustomAlertDialog("Discard changes", "Leave this booking and discard unsaved changes?",
                          CustomAlertDialog::Question, this)
                .exec() != QDialog::Accepted)
        return;
    resetFlow();
    stackedWidget->setCurrentIndex(0);
}

void MainWindow::onNextClicked()
{
    if (m_selectedCategory.isEmpty())
        return;
    const bool changedCategory = currentCategory != m_selectedCategory;
    openSecondInterface(m_selectedCategory);
    if (changedCategory)
        setTimeSlotsInUI({});
    setFormButtonsMode(isStudentEditFlow || isAdminEditMode);
}

void MainWindow::openSecondInterface(const QString &category)
{
    if (currentCategory != category)
        adminFilterDate = QDate();
    currentCategory = category;
    currentWorkshopId = category;
    calanderUi->selectedNameLabel2->setText(workshopTitle(category));
    calanderUi->selectedDescLabel2->setText(
        workshopDepartment(category) +
        "\n\nChoose a date and one or more available time slots.\nGrey slots are unavailable.");
    adminUi->adminWsTitleLbl->setText(workshopTitle(category));
    const QPixmap pix = workshopImage(category, QSize(160, 100));
    calanderUi->selectedImgLabel2->setPixmap(pix);
    adminUi->adminImgLabel->setPixmap(pix);
    updateCalendarHighlights();
    onCalendarSelectionChanged();
    stackedWidget->setCurrentIndex(2);
}

void MainWindow::onNextTo3Clicked()
{
    onCalendarSelectionChanged();
    const QStringList selectedSlots = getSelectedTimeSlotsList();
    if (selectedSlots.isEmpty())
        return;
    formUi->summaryDateLbl->setText(workshopTitle(currentCategory) + "  |  " +
                                 calanderUi->calendar->selectedDate().toString("dddd, d MMMM yyyy"));
    formUi->summaryTimeLbl->setText(selectedSlots.join("  |  "));
    stackedWidget->setCurrentIndex(3);
    formUi->nameInput->setFocus();
}

void MainWindow::goBackTo1()
{
    if (isAdminEditMode) {
        isAdminEditMode = false;
        currentRecord = StudentRecord();
        clearUI();
        loadAdminData();
        stackedWidget->setCurrentIndex(4);
    } else {
        stackedWidget->setCurrentIndex(1);
        arrangeCards();
    }
}

void MainWindow::goBackTo2()
{
    stackedWidget->setCurrentIndex(2);
}

void MainWindow::goBackFromAdmin()
{
    resetFlow();
    stackedWidget->setCurrentIndex(0);
}

void MainWindow::goBackTo0()
{
    cancelCurrentFlow();
}
