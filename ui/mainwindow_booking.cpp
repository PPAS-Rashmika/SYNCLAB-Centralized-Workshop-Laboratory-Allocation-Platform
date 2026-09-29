// Reads the form, applies booking rules, and handles save, update, and delete actions.
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
#include "bookingrules.h"
#include "attachmentstorage.h"
#include <QFile>

void MainWindow::clearUI()
{
    ui3->nameInput->clear();
    ui3->indexInput->clear();
    ui3->deptInput->setCurrentIndex(0);
    ui3->emailInput->clear();
    ui3->pdfInput->clear();
    ui3->descriptionInput->clear();
    setTimeSlotsInUI({});
}

void MainWindow::setFormButtonsMode(bool edit)
{
    ui3->submitBtn->setVisible(!edit);
    ui3->updateBtn->setVisible(edit);
    ui3->deleteBtn->setVisible(edit);
    ui3->confirmTitleLbl->setText(edit ? "Edit your booking" : "Finalize your booking");
}

void MainWindow::loadRecordIntoUI(const StudentRecord &record)
{
    currentRecord = record;
    const QDate date = QDate::fromString(record.getRecordDate(), Qt::ISODate);
    // Old bookings remain viewable/deletable, but resubmission requires a current date.
    ui2->calendar->setMinimumDate(
        qMin(date.isValid() ? date : QDate::currentDate(), QDate::currentDate()));
    ui2->calendar->setSelectedDate(date);
    onCalendarSelectionChanged();
    setTimeSlotsInUI(record.getTimeSlots());
    ui3->nameInput->setText(record.getName());
    ui3->indexInput->setText(record.getIndexNo());
    ui3->deptInput->setCurrentIndex(0);
    ui3->deptInput->setCurrentText(record.getDepartment());
    ui3->emailInput->setText(record.getEmail());
    ui3->pdfInput->setText(record.getPdfPath());
    ui3->descriptionInput->setPlainText(record.getDescription());
    ui3->summaryDateLbl->setText(workshopTitle(currentCategory) + "  |  " +
                                 date.toString("dddd, d MMMM yyyy"));
    ui3->summaryTimeLbl->setText(record.getTimeSlots());
    setFormButtonsMode(true);
}

void MainWindow::loadRecordForEdit(const StudentRecord &record)
{
    isAdminEditMode = true;
    isStudentEditFlow = false;
    isPasscodeBypassed = true;
    currentRecord = record;
    openSecondInterface(record.getCategory());
    loadRecordIntoUI(record);
    ui2->backTo1Btn->setText("Back to dashboard");
    ui2->adminBtn2->hide();
    ui->stackedWidget->setCurrentIndex(3);
}

StudentRecord MainWindow::readFormRecord()
{
    StudentRecord record = currentRecord;
    record.setName(ui3->nameInput->text().trimmed());
    record.setIndexNo(ui3->indexInput->text().trimmed());
    record.setDepartment(ui3->deptInput->currentText());
    record.setEmail(ui3->emailInput->text().trimmed());
    record.setCategory(currentCategory);
    record.setWorkshopId(currentWorkshopId);
    record.setRecordDate(ui2->calendar->selectedDate().toString(Qt::ISODate));
    record.setTimeSlots(getSelectedTimeSlotsList().join(", "));
    record.setDescription(ui3->descriptionInput->toPlainText().trimmed());
    return record;
}

bool MainWindow::validateForm()
{
    const BookingRules::Field field =
        BookingRules::validate(readFormRecord(), ui3->deptInput->currentIndex() > 0);
    if (field == BookingRules::Field::None) {
        return true;
    }
    CustomAlertDialog("Check booking details", BookingRules::validationMessage(field),
                      CustomAlertDialog::Warning, this)
        .exec();
    // Only the UI controller is responsible for keyboard focus and page changes.
    switch (field) {
    case BookingRules::Field::Name:
        ui3->nameInput->setFocus();
        break;
    case BookingRules::Field::Index:
        ui3->indexInput->setFocus();
        break;
    case BookingRules::Field::Department:
        ui3->deptInput->setFocus();
        break;
    case BookingRules::Field::Email:
        ui3->emailInput->setFocus();
        break;
    case BookingRules::Field::Schedule:
        ui->stackedWidget->setCurrentIndex(2);
        break;
    case BookingRules::Field::None:
        break;
    }
    return false;
}

QString MainWindow::savePdfFile(const QString &sourcePath, const QString &allocationKey)
{
    QString error;
    const QString savedPath =
        AttachmentStorage::savePdf(sourcePath, allocationKey, m_dataDirectory, error);
    if (!error.isEmpty()) {
        CustomAlertDialog("Attachment unavailable", error, CustomAlertDialog::Error, this).exec();
    }
    return savedPath;
}

void MainWindow::submitData()
{
    if (currentRecord.getId() >= 0 || !validateForm())
        return;
    StudentRecord record = readFormRecord();
    BookingRules::prepareNewBooking(record);
    const QString source = ui3->pdfInput->text();
    const QString pdf = savePdfFile(source, record.getPasscode());
    if (!source.isEmpty() && pdf.isEmpty())
        return;
    record.setPdfPath(pdf);
    if (!dbManager->addRecord(record)) {
        if (!pdf.isEmpty() && pdf != source)
            QFile::remove(pdf);
        showDatabaseError();
        return;
    }
    CustomAlertDialog("Booking saved",
                      "Your booking is pending approval.\n\nAllocation key: " +
                          record.getPasscode() +
                          "\n\nKeep your allocation key to edit this booking.\n" +
                          workshopTitle(currentCategory) + "\n" + record.getRecordDate() + "\n" +
                          record.getTimeSlots(),
                      CustomAlertDialog::Info, this)
        .exec();
    resetFlow();
    ui->stackedWidget->setCurrentIndex(0);
}

void MainWindow::updateData()
{
    if (currentRecord.getId() < 0 || (!isPasscodeBypassed && !checkPasscode()) || !validateForm())
        return;
    StudentRecord updated = readFormRecord();
    BookingRules::prepareUpdatedBooking(updated, isStudentEditFlow);
    const QString source = ui3->pdfInput->text();
    const QString pdf = savePdfFile(source, updated.getPasscode());
    if (!source.isEmpty() && pdf.isEmpty())
        return;
    updated.setPdfPath(pdf);
    if (!dbManager->updateRecord(updated)) {
        if (!pdf.isEmpty() && pdf != source)
            QFile::remove(pdf);
        showDatabaseError();
        return;
    }
    CustomAlertDialog("Booking updated",
                      isStudentEditFlow ? "Your changes have been saved and are pending approval."
                                        : "Booking changes saved.",
                      CustomAlertDialog::Info, this)
        .exec();
    if (isAdminEditMode) {
        currentRecord = StudentRecord();
        isAdminEditMode = false;
        clearUI();
        loadAdminData();
        ui->stackedWidget->setCurrentIndex(4);
    } else {
        resetFlow();
        ui->stackedWidget->setCurrentIndex(0);
    }
    updateCalendarHighlights();
}

void MainWindow::deleteData()
{
    if (currentRecord.getId() < 0 || (!isPasscodeBypassed && !checkPasscode()))
        return;
    if (CustomAlertDialog("Delete booking", "Permanently delete this booking?",
                          CustomAlertDialog::Question, this)
            .exec() != QDialog::Accepted)
        return;
    if (!dbManager->deleteRecord(currentRecord.getId())) {
        showDatabaseError();
        return;
    }
    if (isAdminEditMode) {
        currentRecord = StudentRecord();
        isAdminEditMode = false;
        clearUI();
        loadAdminData();
        ui->stackedWidget->setCurrentIndex(4);
    } else {
        resetFlow();
        ui->stackedWidget->setCurrentIndex(0);
    }
    updateCalendarHighlights();
}

bool MainWindow::checkPasscode()
{
    CustomInputDialog dialog("Authentication", "Enter your allocation key:", this);
    return dialog.exec() == QDialog::Accepted &&
           dialog.getInput().compare(currentRecord.getPasscode(), Qt::CaseInsensitive) == 0;
}
