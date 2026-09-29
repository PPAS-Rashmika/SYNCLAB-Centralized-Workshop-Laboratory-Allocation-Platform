#include "adminsettingsdialog.h"
#include "customalertdialog.h"
#include "dialogtheme.h"
#include "ui_adminsettingsdialog.h"

#include <QRegularExpression>
#include <QSettings>

namespace
{
// Empty addresses are allowed; non-empty addresses must be valid.
bool validOptionalEmail(const QString &email)
{
    static const QRegularExpression expression(
        R"(^[A-Z0-9.!#$%&'*+/=?^_\x60{|}~-]+@[A-Z0-9](?:[A-Z0-9-]*[A-Z0-9])?(?:\.[A-Z0-9](?:[A-Z0-9-]*[A-Z0-9])?)+$)",
        QRegularExpression::CaseInsensitiveOption);
    return email.isEmpty() || expression.match(email).hasMatch();
}
} // namespace

// Constructor: shows the two saved addresses and lets the administrator
// change them.
AdminSettingsDialog::AdminSettingsDialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::AdminSettingsDialog)
{
    ui->setupUi(this);
    setWindowTitle(tr("Administration email settings"));
    applyDialogTheme(this);
    QSettings settings(QSettings::defaultFormat(), QSettings::UserScope, "StudentWorkshopApp",
                       "AdminSettings");
    ui->deanEmailInput->setText(settings.value("dean_email").toString());
    ui->depHeadEmailInput->setText(settings.value("dep_head_email").toString());
    ui->lbl1->setBuddy(ui->deanEmailInput);
    ui->lbl2->setBuddy(ui->depHeadEmailInput);
    for (auto *input : {ui->deanEmailInput, ui->depHeadEmailInput}) {
        input->setMaxLength(254);
        input->setClearButtonEnabled(true);
        input->setPlaceholderText(tr("Optional email address"));
    }
    ui->saveBtn->setDefault(true);
    ui->deanEmailInput->setFocus();
    connect(ui->saveBtn, &QPushButton::clicked, this, [this]() {
        const QString dean = ui->deanEmailInput->text().trimmed();
        const QString head = ui->depHeadEmailInput->text().trimmed();
        if (!validOptionalEmail(dean) || !validOptionalEmail(head)) {
            // Move the cursor to whichever field is wrong.
            auto *invalid = !validOptionalEmail(dean) ? ui->deanEmailInput : ui->depHeadEmailInput;
            CustomAlertDialog(tr("Check email address"),
                              tr("Enter one valid email address in each field, or leave it blank."),
                              CustomAlertDialog::Warning, this)
                .exec();
            invalid->setFocus();
            invalid->selectAll();
            return;
        }
        QSettings settings(QSettings::defaultFormat(), QSettings::UserScope, "StudentWorkshopApp",
                           "AdminSettings");
        settings.setValue("dean_email", dean);
        settings.setValue("dep_head_email", head);
        settings.sync(); // write to disk now so we can check it worked
        if (settings.status() != QSettings::NoError) {
            CustomAlertDialog(tr("Settings not saved"), tr("Unable to save email settings."),
                              CustomAlertDialog::Error, this)
                .exec();
            return;
        }
        accept();
    });
    connect(ui->cancelBtn, &QPushButton::clicked, this, &QDialog::reject);
}

// Destructor
AdminSettingsDialog::~AdminSettingsDialog()
{
    delete ui;
}
