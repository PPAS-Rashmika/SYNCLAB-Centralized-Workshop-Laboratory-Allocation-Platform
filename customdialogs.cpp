#include "customdialogs.h"
#include "ui_customalertdialog.h"
#include "ui_custominputdialog.h"
#include "ui_adminsettingsdialog.h"
#include "ui_systemsettingsdialog.h"
#include "workshopcatalog.h"

#include <QFileDialog>
#include <QImageReader>
#include <QResizeEvent>
#include <QRegularExpression>
#include <QSettings>

namespace {
void applyDialogTheme(QDialog *dialog, const QString &accent = "#2563EB")
{
    const bool dark = QSettings(QSettings::defaultFormat(), QSettings::UserScope, "StudentWorkshopApp", "ThemeSettings")
                          .value("darkMode", false).toBool();
    const QString background = dark ? "#172033" : "#FFFFFF";
    const QString foreground = dark ? "#F1F5F9" : "#0F172A";
    const QString input = dark ? "#0F172A" : "#F8FAFC";
    const QString border = dark ? "#475569" : "#CBD5E1";

    dialog->setWindowFlags(Qt::Dialog | Qt::WindowTitleHint | Qt::WindowSystemMenuHint
                           | Qt::WindowCloseButtonHint);
    dialog->setMinimumWidth(420);
    dialog->setStyleSheet(QString(
        "QDialog { background-color: %1; color: %2; }"
        "QLabel { color: %2; background: transparent; }"
        "QLineEdit, QComboBox { background-color: %3; color: %2; border: 1px solid %4;"
        " border-radius: 7px; padding: 8px; font-size: 14px; min-height: 22px; }"
        "QLineEdit:focus, QComboBox:focus { border: 1px solid %5; }"
        "QComboBox QAbstractItemView { background-color: %3; color: %2;"
        " selection-background-color: %5; selection-color: white; border: 1px solid %4; }"
        "QPushButton { background-color: %5; color: white; border: 2px solid transparent;"
        " border-radius: 8px; font-size: 14px; font-weight: bold; min-height: 38px; padding: 2px 14px; }"
        "QPushButton:focus, QPushButton:default { border-color: %2; }"
        "QPushButton:hover { border-color: %4; }"
        "QPushButton:disabled { background-color: %4; color: %2; }"
        "QPushButton#cancelBtn, QPushButton#closeBtn { background-color: %3; color: %2; border-color: %4; }"
        "QGroupBox { color: %2; font-weight: bold; border: 1px solid %4;"
        " border-radius: 8px; margin-top: 16px; padding-top: 10px; }"
        "QGroupBox::title { subcontrol-origin: margin; left: 12px; padding: 0 4px; }"
    ).arg(background, foreground, input, border, accent));

    for (auto *button : dialog->findChildren<QPushButton *>())
        button->setCursor(Qt::PointingHandCursor);
    for (auto *label : dialog->findChildren<QLabel *>())
        label->setTextFormat(Qt::PlainText);
}

bool validOptionalEmail(const QString &email)
{
    static const QRegularExpression expression(
        R"(^[A-Z0-9.!#$%&'*+/=?^_\x60{|}~-]+@[A-Z0-9](?:[A-Z0-9-]*[A-Z0-9])?(?:\.[A-Z0-9](?:[A-Z0-9-]*[A-Z0-9])?)+$)",
        QRegularExpression::CaseInsensitiveOption);
    return email.isEmpty() || expression.match(email).hasMatch();
}
}

CustomAlertDialog::CustomAlertDialog(const QString &title, const QString &message,
                                     AlertType type, QWidget *parent)
    : QDialog(parent), ui(new Ui::CustomAlertDialog)
{
    ui->setupUi(this);
    setWindowTitle(title);
    ui->titleLbl->setText(title);
    ui->titleLbl->setWordWrap(true);
    ui->messageLbl->setText(message);
    ui->messageLbl->setTextInteractionFlags(Qt::TextSelectableByMouse | Qt::TextSelectableByKeyboard);
    ui->cancelBtn->setVisible(type == Question);

    QString accent = "#2563EB";
    if (type == Warning) accent = "#92400E";
    else if (type == Error) accent = "#B91C1C";
    applyDialogTheme(this, accent);

    // A confirmation must require an intentional acceptance, including from the keyboard.
    ui->okBtn->setAutoDefault(type != Question);
    ui->okBtn->setDefault(type != Question);
    ui->cancelBtn->setAutoDefault(type == Question);
    ui->cancelBtn->setDefault(type == Question);
    if (type == Question) {
        ui->okBtn->setText(tr("Confirm"));
        ui->cancelBtn->setFocus();
    } else {
        ui->okBtn->setFocus();
    }
    connect(ui->okBtn, &QPushButton::clicked, this, &QDialog::accept);
    connect(ui->cancelBtn, &QPushButton::clicked, this, &QDialog::reject);
}
CustomAlertDialog::~CustomAlertDialog() { delete ui; }

void CustomAlertDialog::resizeEvent(QResizeEvent *event)
{
    QDialog::resizeEvent(event);
    const QMargins margins = ui->verticalLayout->contentsMargins();
    const int availableWidth = qMax(1, width() - margins.left() - margins.right());
    ui->messageLbl->setMinimumHeight(ui->messageLbl->heightForWidth(availableWidth));
}

CustomInputDialog::CustomInputDialog(const QString &title, const QString &prompt, QWidget *parent)
    : QDialog(parent), ui(new Ui::CustomInputDialog)
{
    ui->setupUi(this);
    setWindowTitle(title);
    ui->titleLbl->setText(title);
    ui->titleLbl->setWordWrap(true);
    ui->promptLbl->setText(prompt);
    ui->promptLbl->setBuddy(ui->inputBox);
    applyDialogTheme(this);
    ui->inputBox->setMaxLength(128);
    ui->inputBox->setClearButtonEnabled(true);
    ui->inputBox->setFocus();
    ui->okBtn->setDefault(true);
    ui->okBtn->setEnabled(false);
    connect(ui->inputBox, &QLineEdit::textChanged, this, [this](const QString &text) {
        ui->okBtn->setEnabled(!text.trimmed().isEmpty());
    });
    connect(ui->inputBox, &QLineEdit::returnPressed, this, [this]() {
        if (!getInput().isEmpty()) accept();
    });
    connect(ui->okBtn, &QPushButton::clicked, this, &QDialog::accept);
    connect(ui->cancelBtn, &QPushButton::clicked, this, &QDialog::reject);
}
CustomInputDialog::~CustomInputDialog() { delete ui; }
QString CustomInputDialog::getInput() const { return ui->inputBox->text().trimmed(); }

AdminSettingsDialog::AdminSettingsDialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::AdminSettingsDialog)
{
    ui->setupUi(this);
    setWindowTitle(tr("Administration email settings"));
    applyDialogTheme(this);
    QSettings settings(QSettings::defaultFormat(), QSettings::UserScope, "StudentWorkshopApp", "AdminSettings");
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
            auto *invalid = !validOptionalEmail(dean) ? ui->deanEmailInput : ui->depHeadEmailInput;
            CustomAlertDialog(tr("Check email address"),
                              tr("Enter one valid email address in each field, or leave it blank."),
                              CustomAlertDialog::Warning, this).exec();
            invalid->setFocus();
            invalid->selectAll();
            return;
        }
        QSettings settings(QSettings::defaultFormat(), QSettings::UserScope, "StudentWorkshopApp", "AdminSettings");
        settings.setValue("dean_email", dean);
        settings.setValue("dep_head_email", head);
        settings.sync();
        if (settings.status() != QSettings::NoError) {
            CustomAlertDialog(tr("Settings not saved"), tr("Unable to save email settings."),
                              CustomAlertDialog::Error, this).exec();
            return;
        }
        accept();
    });
    connect(ui->cancelBtn, &QPushButton::clicked, this, &QDialog::reject);
}
AdminSettingsDialog::~AdminSettingsDialog() { delete ui; }

SystemSettingsDialog::SystemSettingsDialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::SystemSettingsDialog)
{
    ui->setupUi(this);
    setWindowTitle(tr("System settings"));
    applyDialogTheme(this);
    const bool dark = QSettings(QSettings::defaultFormat(), QSettings::UserScope, "StudentWorkshopApp", "ThemeSettings")
                          .value("darkMode", false).toBool();
    ui->toggleThemeBtn->setText(dark ? tr("Switch to light mode") : tr("Switch to dark mode"));
    ui->groupBox->setTitle(tr("Administrator passcodes"));
    for (const QString &category : workshopCategories())
        ui->workshopCombo->addItem(category, category);
    ui->workshopCombo->addItem(tr("System administrator"), "__master__");
    ui->workshopCombo->setSizeAdjustPolicy(QComboBox::AdjustToMinimumContentsLengthWithIcon);
    ui->workshopCombo->setMinimumContentsLength(18);
    ui->lblCat->setBuddy(ui->workshopCombo);
    ui->lblCode->setBuddy(ui->newPasscodeInput);
    ui->newPasscodeInput->setMaxLength(64);
    ui->newPasscodeInput->setPlaceholderText(tr("At least 4 characters"));
    ui->newPasscodeInput->setClearButtonEnabled(true);
    ui->savePasscodeBtn->setEnabled(false);
    ui->closeBtn->setAutoDefault(false);
    ui->toggleThemeBtn->setAutoDefault(false);
    ui->changeWallpaperBtn->setAutoDefault(false);
    ui->savePasscodeBtn->setAutoDefault(false);
    connect(ui->newPasscodeInput, &QLineEdit::textChanged, this, [this](const QString &value) {
        ui->savePasscodeBtn->setEnabled(value.trimmed().size() >= 4);
    });
    connect(ui->workshopCombo, &QComboBox::currentIndexChanged, this, [this]() {
        ui->newPasscodeInput->clear();
    });
    connect(ui->newPasscodeInput, &QLineEdit::returnPressed, ui->savePasscodeBtn, &QPushButton::click);
    connect(ui->toggleThemeBtn, &QPushButton::clicked, this, [this, dark]() {
        QSettings settings(QSettings::defaultFormat(), QSettings::UserScope, "StudentWorkshopApp", "ThemeSettings");
        settings.setValue("darkMode", !dark);
        settings.sync();
        if (settings.status() != QSettings::NoError) {
            CustomAlertDialog(tr("Settings not saved"), tr("Unable to save the selected theme."),
                              CustomAlertDialog::Error, this).exec();
            return;
        }
        emit themeChanged();
        accept();
    });
    connect(ui->changeWallpaperBtn, &QPushButton::clicked, this, [this]() {
        const QString path = QFileDialog::getOpenFileName(
            this, tr("Select background wallpaper"), QString(), tr("Images (*.png *.jpg *.jpeg)"));
        if (path.isEmpty()) return;
        QImageReader reader(path);
        if (!reader.canRead()) {
            CustomAlertDialog(tr("Invalid image"), tr("The selected file cannot be opened as an image."),
                              CustomAlertDialog::Warning, this).exec();
            return;
        }
        emit wallpaperChanged(path);
    });
    connect(ui->savePasscodeBtn, &QPushButton::clicked, this, [this]() {
        const QString code = ui->newPasscodeInput->text().trimmed();
        if (code.size() < 4) return;
        const QString category = ui->workshopCombo->currentData().toString();
        const QString key = category == "__master__" ? "master_passcode" : category + "_passcode";
        QSettings settings(QSettings::defaultFormat(), QSettings::UserScope, "StudentWorkshopApp", "Passcodes");
        settings.setValue(key, code);
        settings.sync();
        if (settings.status() != QSettings::NoError) {
            CustomAlertDialog(tr("Passcode not saved"), tr("Unable to save the new passcode."),
                              CustomAlertDialog::Error, this).exec();
            return;
        }
        ui->newPasscodeInput->clear();
        CustomAlertDialog(tr("Passcode updated"),
                          tr("The passcode for %1 has been updated.").arg(ui->workshopCombo->currentText()),
                          CustomAlertDialog::Info, this).exec();
        ui->newPasscodeInput->setFocus();
    });
    connect(ui->closeBtn, &QPushButton::clicked, this, &QDialog::reject);
}
SystemSettingsDialog::~SystemSettingsDialog() { delete ui; }
