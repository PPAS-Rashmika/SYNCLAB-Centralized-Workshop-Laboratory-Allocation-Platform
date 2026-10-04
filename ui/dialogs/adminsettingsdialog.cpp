// Lab/Workshop In-charge සහ Department/head emails දෙක පරීක්ෂා කර save කරයි.
// #include වලින් මේ file එකට අවශ්‍ය Qt සහ project declarations ලබාගනී.
#include "adminsettingsdialog.h"
#include "customalertdialog.h"
#include "dialogtheme.h"
#include "ui_adminsettingsdialog.h"

#include <QRegularExpression>
#include <QSettings>

// නම් නැති namespace එකේ helpers මේ file එක තුළට පමණක් සීමා වේ.
namespace
{
// Email එක හිස් නොවී නිවැරදි රටාවකට ගැළපේදැයි බලයි; validation pattern මෙහි වෙනස් කරන්න.
bool validRequiredEmail(const QString &email)
{
    static const QRegularExpression expression(
        R"(^[A-Z0-9.!#$%&'*+/=?^_\x60{|}~-]+@[A-Z0-9](?:[A-Z0-9-]*[A-Z0-9])?(?:\.[A-Z0-9](?:[A-Z0-9-]*[A-Z0-9])?)+$)",
        QRegularExpression::CaseInsensitiveOption);
    return !email.isEmpty() && expression.match(email).hasMatch();
}
}

// Save කළ emails දෙක පෙන්වා නිවැරදි emails දෙකම ලැබුණොත් පමණක් save කරයි.
AdminSettingsDialog::AdminSettingsDialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::AdminSettingsDialog)
{
    ui->setupUi(this);
    applyDialogTheme(this);
    QSettings settings(QSettings::defaultFormat(), QSettings::UserScope, "StudentWorkshopApp",
                       "AdminSettings");
    // dean_email පැරණි save key එකම භාවිත කර Lab/Workshop In-charge email එක කියවයි.
    ui->deanEmailInput->setText(settings.value("dean_email").toString());
    ui->depHeadEmailInput->setText(settings.value("dep_head_email").toString());
    ui->lbl1->setBuddy(ui->deanEmailInput);
    ui->lbl2->setBuddy(ui->depHeadEmailInput);
    // Emails දෙකට අකුරු 254 සීමාව සහ clear button එක යොදයි.
    for (auto *input : {ui->deanEmailInput, ui->depHeadEmailInput}) {
        input->setMaxLength(254);
        input->setClearButtonEnabled(true);
    }
    ui->saveBtn->setDefault(true);
    ui->deanEmailInput->setFocus();
    // Save එබූ විට emails දෙකම කියවා valid නම් පමණක් save කරයි.
    connect(ui->saveBtn, &QPushButton::clicked, this, [this]() {
        const QString incharge = ui->deanEmailInput->text().trimmed();
        const QString head = ui->depHeadEmailInput->text().trimmed();
        if (!validRequiredEmail(incharge) || !validRequiredEmail(head)) {
            // වැරදි email field එක තෝරා message පෙන්වා cursor එක එතැනට යවයි.
            auto *invalid = !validRequiredEmail(incharge) ? ui->deanEmailInput : ui->depHeadEmailInput;
            const QString field = invalid == ui->deanEmailInput
                                      ? tr("Lab / Workshop In-charge Email")
                                      : tr("Department or head email");
            const QString message = invalid->text().trimmed().isEmpty()
                                        ? tr("%1 is required.").arg(field)
                                        : tr("Enter a valid email address for %1.").arg(field);
            // Title/message සහ OK/Cancel සකසයි; message වර්ගයට ගැළපෙන පාට තෝරයි.
            CustomAlertDialog(tr("Check email address"),
                              message,
                              CustomAlertDialog::Warning, this)
                .exec();
            invalid->setFocus();
            invalid->selectAll();
            return;
        }
        QSettings settings(QSettings::defaultFormat(), QSettings::UserScope, "StudentWorkshopApp",
                           "AdminSettings");
        // In-charge සහ head emails QSettings හි save කරයි; field නම මාරු කළත් key එක රැකගනී.
        settings.setValue("dean_email", incharge);
        settings.setValue("dep_head_email", head);
        // Settings disk එකට ලියා සාර්ථකදැයි status() මගින් පරීක්ෂා කරයි.
        settings.sync();  
        if (settings.status() != QSettings::NoError) {
            // Title/message සහ OK/Cancel සකසයි; message වර්ගයට ගැළපෙන පාට තෝරයි.
            CustomAlertDialog(tr("Settings not saved"), tr("Unable to save email settings."),
                              CustomAlertDialog::Error, this)
                .exec();
            return;
        }
        accept();
    });
    connect(ui->cancelBtn, &QPushButton::clicked, this, &QDialog::reject);
}

// Email dialog එකේ UI helper එක නිදහස් කරයි.
AdminSettingsDialog::~AdminSettingsDialog()
{
    delete ui;
}
