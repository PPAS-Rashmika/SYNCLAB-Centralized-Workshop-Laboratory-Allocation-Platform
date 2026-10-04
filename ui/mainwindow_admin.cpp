// Handles administrator access, filters, table edits, email drafts, settings, and printing.
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
#include <QDesktopServices>
#include <QFileInfo>
#include <QPrintDialog>
#include <QPrinter>
#include <QSettings>
#include <QSignalBlocker>
#include <QStatusBar>
#include <QTextDocument>
#include <QUrlQuery>

void MainWindow::openSystemSettings()
{
    CustomInputDialog authentication("System settings", "Enter the master passcode:", this);
    if (authentication.exec() != QDialog::Accepted)
        return;
    if (authentication.getInput() != QSettings(QSettings::defaultFormat(), QSettings::UserScope,
                                               "StudentWorkshopApp", "Passcodes")
                                         .value("master_passcode", "0000")
                                         .toString()) {
        CustomAlertDialog("Access denied", "Incorrect master passcode.", CustomAlertDialog::Error,
                          this)
            .exec();
        return;
    }
    SystemSettingsDialog dialog(this);
    connect(&dialog, &SystemSettingsDialog::themeChanged, this, [this] {
        applyTheme(QSettings(QSettings::defaultFormat(), QSettings::UserScope, "StudentWorkshopApp",
                             "ThemeSettings")
                       .value("darkMode", false)
                       .toBool());
    });
    connect(&dialog, &SystemSettingsDialog::wallpaperChanged, this, [this](const QString &path) {
        QSettings settings(QSettings::defaultFormat(), QSettings::UserScope, "StudentWorkshopApp",
                           "ThemeSettings");
        settings.setValue("wallpaper", path);
        settings.sync();
        if (settings.status() != QSettings::NoError) {
            CustomAlertDialog("Settings not saved", "Unable to save the background setting.",
                              CustomAlertDialog::Error, this)
                .exec();
            return;
        }
        applyTheme(settings.value("darkMode", false).toBool());
    });
    dialog.exec();
}

void MainWindow::showRecordsWindow()
{
    CustomInputDialog authentication(
        "Administrator sign in", "Enter the passcode for " + workshopTitle(currentCategory) + ":",
        this);
    if (authentication.exec() != QDialog::Accepted)
        return;
    if (authentication.getInput() != QSettings(QSettings::defaultFormat(), QSettings::UserScope,
                                               "StudentWorkshopApp", "Passcodes")
                                         .value(currentCategory + "_passcode", "0000000")
                                         .toString()) {
        CustomAlertDialog("Access denied", "Incorrect administrator passcode.",
                          CustomAlertDialog::Error, this)
            .exec();
        return;
    }
    adminFilterDate = QDate();
    loadAdminData();
    stackedWidget->setCurrentIndex(4);
}

void MainWindow::adminFilterByDate(const QDate &date)
{
    if (adminFilterDate == date)
        return;
    adminFilterDate = date;
    loadAdminData();
}

void MainWindow::adminClearFilter()
{
    adminFilterDate = QDate();
    const QSignalBlocker blocker(adminUi->adminCalendar);
    adminUi->adminCalendar->setSelectedDate(QDate::currentDate());
    loadAdminData();
}

void MainWindow::openAdminEmailSettings()
{
    AdminSettingsDialog dialog(this);
    dialog.exec();
}

void MainWindow::refreshAdminActions()
{
    const bool selected =
        adminUi->mainTable->currentRow() >= 0 && !adminUi->mainTable->selectedItems().isEmpty();
    for (QPushButton *button :
         {adminUi->adminEditBtn, adminUi->adminDelBtn, adminUi->pendingEmailBtn, adminUi->rejectEmailBtn})
        button->setEnabled(selected);
}

void MainWindow::loadAdminData()
{
    const QList<StudentRecord> records =
        adminFilterDate.isValid() ? dbManager->getRecordsByDateAndCategory(
                                        adminFilterDate.toString(Qt::ISODate), currentCategory)
                                  : dbManager->getAllRecordsForCategory(currentCategory);
    if (!dbManager->getLastError().isEmpty()) {
        showDatabaseError();
        return;
    }
    const QSignalBlocker tableBlocker(adminUi->mainTable);
    adminUi->mainTable->setRowCount(0);
    for (int row = 0; row < records.size(); ++row) {
        const StudentRecord record = records[row];
        adminUi->mainTable->insertRow(row);
        const QStringList values = {record.getPasscode(),
                                    record.getCreatedAt(),
                                    record.getIndexNo(),
                                    record.getName(),
                                    record.getDepartment(),
                                    record.getEmail(),
                                    record.getPdfPath().isEmpty()
                                        ? "No attachment"
                                        : QFileInfo(record.getPdfPath()).fileName(),
                                    record.getRecordDate(),
                                    record.getTimeSlots(),
                                    record.getDescription(),
                                    record.getAdminRemarks(),
                                    record.getStatus()};
        for (int column = 0; column < values.size(); ++column) {
            auto *item = new QTableWidgetItem(values[column]);
            item->setToolTip(values[column]);
            adminUi->mainTable->setItem(row, column, item);
        }
        adminUi->mainTable->item(row, 0)->setData(Qt::UserRole, record.getId());
        adminUi->mainTable->item(row, 6)->setData(Qt::UserRole, record.getPdfPath());
        adminUi->mainTable->item(row, 6)->setToolTip(
            record.getPdfPath().isEmpty() ? "No PDF attached" : "Double-click to open the PDF");
        auto *remarks = new QLineEdit(record.getAdminRemarks());
        remarks->setMaxLength(1000);
        remarks->setPlaceholderText("Add a remark");
        remarks->setProperty("savedText", record.getAdminRemarks());
        connect(remarks, &QLineEdit::editingFinished, this, [this, remarks, id = record.getId()] {
            if (remarks->text() == remarks->property("savedText").toString())
                return;
            if (!dbManager->updateAdminRemarks(id, remarks->text())) {
                remarks->setText(remarks->property("savedText").toString());
                showDatabaseError();
            } else {
                remarks->setProperty("savedText", remarks->text());
            }
        });
        adminUi->mainTable->setCellWidget(row, 10, remarks);
        auto *status = new QComboBox();
        status->addItem("Pending", "Pending");
        status->addItem("Approved", "Approve");
        status->addItem("Rejected", "Reject");
        status->setCurrentIndex(qMax(0, status->findData(record.getStatus())));
        status->setProperty("savedIndex", status->currentIndex());
        connect(status, &QComboBox::currentIndexChanged, this,
                [this, status, id = record.getId()](int) {
                    if (!dbManager->updateRecordStatus(id, status->currentData().toString())) {
                        const QSignalBlocker blocker(status);
                        status->setCurrentIndex(status->property("savedIndex").toInt());
                        showDatabaseError();
                        return;
                    }
                    status->setProperty("savedIndex", status->currentIndex());
                    updateCalendarHighlights();
                    updateAdminCalendarHighlights();
                });
        adminUi->mainTable->setCellWidget(row, 11, status);
    }
    adminUi->adminWsDescLbl->setText(
        workshopDepartment(currentCategory) + "\n\n" + QString::number(records.size()) +
        (records.size() == 1 ? " booking" : " bookings") +
        (adminFilterDate.isValid() ? " on " + adminFilterDate.toString("d MMM yyyy")
                                   : " · All dates"));
    adminUi->adminShowAllBtn->setEnabled(adminFilterDate.isValid());
    refreshAdminActions();
    updateAdminCalendarHighlights();
}

void MainWindow::composeStatusEmail(const QString &status)
{
    const int row = adminUi->mainTable->currentRow();
    if (row < 0)
        return;
    const int id = adminUi->mainTable->item(row, 0)->data(Qt::UserRole).toInt();
    const auto records = dbManager->getAllRecordsForCategory(currentCategory);
    if (!dbManager->getLastError().isEmpty()) {
        showDatabaseError();
        return;
    }
    for (const auto &record : records) {
        if (record.getId() != id)
            continue;
        if (!dbManager->updateRecordStatus(id, status)) {
            showDatabaseError();
            return;
        }
        QUrl url("mailto:" + record.getEmail());
        QUrlQuery query;
        const bool pending = status == "Pending";
        query.addQueryItem("subject", "Workshop booking " +
                                          QString(pending ? "pending" : "rejected") + ": " +
                                          workshopTitle(currentCategory));
        query.addQueryItem("body",
                           "Dear " + record.getName() + ",\n\nYour booking for " +
                               workshopTitle(currentCategory) + " on " + record.getRecordDate() +
                               " is " + (pending ? "pending approval." : "rejected.") + "\nTime: " +
                               record.getTimeSlots() + "\nAllocation key: " + record.getPasscode() +
                               "\nRemarks: " + record.getAdminRemarks());
        if (pending) {
            QSettings settings(QSettings::defaultFormat(), QSettings::UserScope,
                               "StudentWorkshopApp", "AdminSettings");
            QStringList recipients;
            for (const QString &key : {QString("dean_email"), QString("dep_head_email")}) {
                const QString email = settings.value(key).toString();
                if (!email.isEmpty())
                    recipients.append(email);
            }
            if (!recipients.isEmpty())
                query.addQueryItem("cc", recipients.join(','));
        }
        url.setQuery(query);
        loadAdminData();
        updateCalendarHighlights();
        if (!QDesktopServices::openUrl(url))
            CustomAlertDialog(
                "Email draft unavailable",
                "The booking status was saved, but no email application could open the draft.",
                CustomAlertDialog::Warning, this)
                .exec();
        else
            statusBar()->showMessage(
                "Email draft opened. Review and send it in your email application.", 10000);
        return;
    }
}

void MainWindow::printAllRecords()
{
    const auto records = adminFilterDate.isValid()
                             ? dbManager->getRecordsByDateAndCategory(
                                   adminFilterDate.toString(Qt::ISODate), currentCategory)
                             : dbManager->getAllRecordsForCategory(currentCategory);
    if (!dbManager->getLastError().isEmpty()) {
        showDatabaseError();
        return;
    }
    QString html =
        "<h1>" + workshopTitle(currentCategory).toHtmlEscaped() + "</h1><p>" +
        (adminFilterDate.isValid() ? adminFilterDate.toString("d MMM yyyy") : "All dates") +
        "</p><table border='1' cellspacing='0' "
        "cellpadding='5'><tr><th>Date</th><th>Name</th><th>Index</th><th>Time "
        "slots</th><th>Status</th></tr>";
    for (const auto &record : records)
        html += "<tr><td>" + record.getRecordDate().toHtmlEscaped() + "</td><td>" +
                record.getName().toHtmlEscaped() + "</td><td>" +
                record.getIndexNo().toHtmlEscaped() + "</td><td>" +
                record.getTimeSlots().toHtmlEscaped() + "</td><td>" +
                record.getStatus().toHtmlEscaped() + "</td></tr>";
    html += "</table>";
    QPrinter printer(QPrinter::HighResolution);
    QPrintDialog dialog(&printer, this);
    if (dialog.exec() != QDialog::Accepted)
        return;
    QTextDocument document;
    document.setHtml(html);
    document.print(&printer);
}

void MainWindow::printSingleRecord(const StudentRecord &record)
{
    QPrinter printer(QPrinter::HighResolution);
    QPrintDialog dialog(&printer, this);
    if (dialog.exec() != QDialog::Accepted)
        return;
    QTextDocument document;
    document.setPlainText(workshopTitle(record.getCategory()) + "\n" + record.getName() + "\n" +
                          record.getIndexNo() + "\n" + record.getRecordDate() + "\n" +
                          record.getTimeSlots() + "\n" + record.getStatus());
    document.print(&printer);
}
