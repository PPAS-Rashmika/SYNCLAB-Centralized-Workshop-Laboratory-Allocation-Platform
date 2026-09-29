// Moves between pages and starts or cancels a booking workflow.
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

void MainWindow::resetFlow()
{
    currentRecord = StudentRecord();
    currentCategory.clear();
    currentWorkshopId.clear();
    isPasscodeBypassed = false;
    isAdminEditMode = false;
    isStudentEditFlow = false;
    adminFilterDate = QDate();
    ui2->calendar->setMinimumDate(QDate::currentDate());
    ui2->calendar->setSelectedDate(QDate::currentDate());
    clearUI();
    selectCard({}, nullptr);
    ui2->adminBtn2->show();
    ui2->backTo1Btn->setText("Back to workshops");
    setFormButtonsMode(false);
}

void MainWindow::startNewBooking()
{
    if (!m_databaseReady)
        return;
    resetFlow();
    ui->stackedWidget->setCurrentIndex(1);
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
            "This older key matches multiple bookings.\nEnter your booking ID or student index:",
            this);
        if (identity.exec() != QDialog::Accepted)
            return;
        const QString identifier = identity.getInput();
        for (auto it = matches.begin(); it != matches.end();)
            if (it->getBookingId() != identifier && it->getIndexNo() != identifier)
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
    ui2->adminBtn2->hide();
    loadRecordIntoUI(currentRecord);
    selectCard(currentCategory, nullptr);
    const QDate savedDate = QDate::fromString(currentRecord.getRecordDate(), Qt::ISODate);
    // Historical bookings must still expose the delete action and editable details.
    ui->stackedWidget->setCurrentIndex(savedDate < QDate::currentDate() ? 3 : 1);
    arrangeCards();
}

void MainWindow::cancelCurrentFlow()
{
    const bool hasDraft =
        isStudentEditFlow || isAdminEditMode || !ui3->nameInput->text().isEmpty() ||
        !ui3->indexInput->text().isEmpty() || !ui3->emailInput->text().isEmpty() ||
        !ui3->descriptionInput->toPlainText().isEmpty() || !ui3->pdfInput->text().isEmpty();
    if (hasDraft &&
        CustomAlertDialog("Discard changes", "Leave this booking and discard unsaved changes?",
                          CustomAlertDialog::Question, this)
                .exec() != QDialog::Accepted)
        return;
    resetFlow();
    ui->stackedWidget->setCurrentIndex(0);
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
    ui2->selectedNameLabel2->setText(workshopTitle(category));
    ui2->selectedDescLabel2->setText(
        workshopDepartment(category) +
        "\n\nChoose a date and one or more available time slots.\nGrey slots are unavailable.");
    ui4->adminWsTitleLbl->setText(workshopTitle(category));
    const QPixmap pix = workshopImage(category, QSize(160, 100));
    ui2->selectedImgLabel2->setPixmap(pix);
    ui4->adminImgLabel->setPixmap(pix);
    updateCalendarHighlights();
    onCalendarSelectionChanged();
    ui->stackedWidget->setCurrentIndex(2);
}

void MainWindow::onNextTo3Clicked()
{
    onCalendarSelectionChanged();
    const QStringList selectedSlots = getSelectedTimeSlotsList();
    if (selectedSlots.isEmpty())
        return;
    ui3->summaryDateLbl->setText(workshopTitle(currentCategory) + "  |  " +
                                 ui2->calendar->selectedDate().toString("dddd, d MMMM yyyy"));
    ui3->summaryTimeLbl->setText(selectedSlots.join("  |  "));
    ui->stackedWidget->setCurrentIndex(3);
    ui3->nameInput->setFocus();
}

void MainWindow::goBackTo1()
{
    if (isAdminEditMode) {
        isAdminEditMode = false;
        currentRecord = StudentRecord();
        clearUI();
        loadAdminData();
        ui->stackedWidget->setCurrentIndex(4);
    } else {
        ui->stackedWidget->setCurrentIndex(1);
        arrangeCards();
    }
}

void MainWindow::goBackTo2()
{
    ui->stackedWidget->setCurrentIndex(2);
}

void MainWindow::goBackFromAdmin()
{
    resetFlow();
    ui->stackedWidget->setCurrentIndex(0);
}

void MainWindow::goBackTo0()
{
    cancelCurrentFlow();
}
