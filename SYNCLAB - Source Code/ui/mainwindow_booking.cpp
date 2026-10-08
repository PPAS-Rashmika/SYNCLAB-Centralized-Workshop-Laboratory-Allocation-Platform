// Reads the form, applies booking rules, and handles save, update, and delete actions.
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
#include "bookingrules.h"
#include "attachmentstorage.h"
#include <QFile>

void MainWindow::clearUI()
{
    formUi->nameInput->clear();
    formUi->indexInput->clear();
    formUi->deptInput->setCurrentIndex(0);
    formUi->emailInput->clear();
    formUi->pdfInput->clear();
    formUi->descriptionInput->clear();
    setTimeSlotsInUI({});
}

void MainWindow::setFormButtonsMode(bool edit)
{
    formUi->submitBtn->setVisible(!edit);
    formUi->updateBtn->setVisible(edit);
    formUi->deleteBtn->setVisible(edit);
    formUi->confirmTitleLbl->setText(edit ? "Edit your booking" : "Finalize your booking");
}

void MainWindow::loadRecordIntoUI(const StudentRecord &record)
{
    currentRecord = record;
    const QDate date = QDate::fromString(record.getRecordDate(), Qt::ISODate);
    // Old bookings remain viewable/deletable, but resubmission requires a current date.
    calanderUi->calendar->setMinimumDate(
        qMin(date.isValid() ? date : QDate::currentDate(), QDate::currentDate()));
    calanderUi->calendar->setSelectedDate(date);
    onCalendarSelectionChanged();
    setTimeSlotsInUI(record.getTimeSlots());
    formUi->nameInput->setText(record.getName());
    formUi->indexInput->setText(record.getIndexNo());
    formUi->deptInput->setCurrentIndex(0);
    formUi->deptInput->setCurrentText(record.getDepartment());
    formUi->emailInput->setText(record.getEmail());
    formUi->pdfInput->setText(record.getPdfPath());
    formUi->descriptionInput->setPlainText(record.getDescription());
    formUi->summaryDateLbl->setText(workshopTitle(currentCategory) + "  |  " +
                                 date.toString("dddd, d MMMM yyyy"));
    formUi->summaryTimeLbl->setText(record.getTimeSlots());
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
    calanderUi->backTo1Btn->setText("Back to dashboard");
    calanderUi->adminBtn2->hide();
    stackedWidget->setCurrentIndex(3);
}

StudentRecord MainWindow::readFormRecord()
{
    StudentRecord record = currentRecord;
    record.setName(formUi->nameInput->text().trimmed());
    record.setIndexNo(formUi->indexInput->text().trimmed());
    record.setDepartment(formUi->deptInput->currentText());
    record.setEmail(formUi->emailInput->text().trimmed());
    record.setCategory(currentCategory);
    record.setWorkshopId(currentWorkshopId);
    record.setRecordDate(calanderUi->calendar->selectedDate().toString(Qt::ISODate));
    record.setTimeSlots(getSelectedTimeSlotsList().join(", "));
    record.setDescription(formUi->descriptionInput->toPlainText().trimmed());
    return record;
}

bool MainWindow::validateForm()
{
    const BookingRules::Field field =
        BookingRules::validate(readFormRecord(), formUi->deptInput->currentIndex() > 0);
    if (field == BookingRules::Field::None) {
        return true;
    }
    CustomAlertDialog("Check booking details", BookingRules::validationMessage(field),
                      CustomAlertDialog::Warning, this)
        .exec();
    // Only the UI controller is responsible for keyboard focus and page changes.
    switch (field) {
    case BookingRules::Field::Name:
        formUi->nameInput->setFocus();
        break;
    case BookingRules::Field::Index:
        formUi->indexInput->setFocus();
        break;
    case BookingRules::Field::Department:
        formUi->deptInput->setFocus();
        break;
    case BookingRules::Field::Email:
        formUi->emailInput->setFocus();
        break;
    case BookingRules::Field::Schedule:
        stackedWidget->setCurrentIndex(2);
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
    const QString source = formUi->pdfInput->text();
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
    stackedWidget->setCurrentIndex(0);
}

void MainWindow::updateData()
{
    if (currentRecord.getId() < 0 || (!isPasscodeBypassed && !checkPasscode()) || !validateForm())
        return;
    StudentRecord updated = readFormRecord();
    BookingRules::prepareUpdatedBooking(updated, isStudentEditFlow);
    const QString source = formUi->pdfInput->text();
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
        stackedWidget->setCurrentIndex(4);
    } else {
        resetFlow();
        stackedWidget->setCurrentIndex(0);
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
        stackedWidget->setCurrentIndex(4);
    } else {
        resetFlow();
        stackedWidget->setCurrentIndex(0);
    }
    updateCalendarHighlights();
}

bool MainWindow::checkPasscode()
{
    CustomInputDialog dialog("Authentication", "Enter your allocation key:", this);
    return dialog.exec() == QDialog::Accepted &&
           dialog.getInput().compare(currentRecord.getPasscode(), Qt::CaseInsensitive) == 0;
}
