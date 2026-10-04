#include "systemsettingsdialog.h"
#include "customalertdialog.h"
#include "dialogtheme.h"
#include "ui_systemsettingsdialog.h"
#include "workshopcatalog.h"

#include <QFileDialog>
#include <QImageReader>
#include <QSettings>

// Constructor: builds the settings dialog for the theme, the background
// picture and the administrator passcodes.
SystemSettingsDialog::SystemSettingsDialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::SystemSettingsDialog)
{
    ui->setupUi(this);
    applyDialogTheme(this);
    const bool dark = QSettings(QSettings::defaultFormat(), QSettings::UserScope,
                                "StudentWorkshopApp", "ThemeSettings")
                          .value("darkMode", false)
                          .toBool();
    ui->toggleThemeBtn->setText(dark ? tr("Switch to light mode") : tr("Switch to dark mode"));
    // One entry per workshop, plus one for the master passcode.
    for (const QString &category : workshopCategories())
        ui->workshopCombo->addItem(category, category);
    ui->workshopCombo->addItem(tr("System administrator"), "__master__");
    ui->workshopCombo->setSizeAdjustPolicy(QComboBox::AdjustToMinimumContentsLengthWithIcon);
    ui->workshopCombo->setMinimumContentsLength(18);
    ui->lblCat->setBuddy(ui->workshopCombo);
    ui->lblCode->setBuddy(ui->newPasscodeInput);
    ui->newPasscodeInput->setMaxLength(64);
    ui->newPasscodeInput->setClearButtonEnabled(true);
    ui->savePasscodeBtn->setEnabled(false);
    ui->closeBtn->setAutoDefault(false);
    ui->toggleThemeBtn->setAutoDefault(false);
    ui->changeWallpaperBtn->setAutoDefault(false);
    ui->savePasscodeBtn->setAutoDefault(false);
    // The Save button only works once the new passcode is at least 4 characters.
    connect(ui->newPasscodeInput, &QLineEdit::textChanged, this, [this](const QString &value) {
        ui->savePasscodeBtn->setEnabled(value.trimmed().size() >= 4);
    });
    // Changing the workshop clears the box so a passcode is never saved to the
    // wrong workshop by mistake.
    connect(ui->workshopCombo, &QComboBox::currentIndexChanged, this,
            [this]() { ui->newPasscodeInput->clear(); });
    connect(ui->newPasscodeInput, &QLineEdit::returnPressed, ui->savePasscodeBtn,
            &QPushButton::click);
    // Flip the saved theme, then tell MainWindow to repaint itself.
    connect(ui->toggleThemeBtn, &QPushButton::clicked, this, [this, dark]() {
        QSettings settings(QSettings::defaultFormat(), QSettings::UserScope, "StudentWorkshopApp",
                           "ThemeSettings");
        settings.setValue("darkMode", !dark);
        settings.sync();
        if (settings.status() != QSettings::NoError) {
            CustomAlertDialog(tr("Settings not saved"), tr("Unable to save the selected theme."),
                              CustomAlertDialog::Error, this)
                .exec();
            return;
        }
        emit themeChanged();
        accept();
    });
    // Pick a background picture and make sure Qt can really read it before saving it.
    connect(ui->changeWallpaperBtn, &QPushButton::clicked, this, [this]() {
        const QString path = QFileDialog::getOpenFileName(
            this, tr("Select background wallpaper"), QString(), tr("Images (*.png *.jpg *.jpeg)"));
        if (path.isEmpty())
            return;
        QImageReader reader(path);
        if (!reader.canRead()) {
            CustomAlertDialog(tr("Invalid image"),
                              tr("The selected file cannot be opened as an image."),
                              CustomAlertDialog::Warning, this)
                .exec();
            return;
        }
        emit wallpaperChanged(path);
    });
    // Save the new passcode for the chosen workshop, or for the system administrator.
    connect(ui->savePasscodeBtn, &QPushButton::clicked, this, [this]() {
        const QString code = ui->newPasscodeInput->text().trimmed();
        if (code.size() < 4)
            return;
        const QString category = ui->workshopCombo->currentData().toString();
        const QString key = category == "__master__" ? "master_passcode" : category + "_passcode";
        QSettings settings(QSettings::defaultFormat(), QSettings::UserScope, "StudentWorkshopApp",
                           "Passcodes");
        settings.setValue(key, code);
        settings.sync();
        if (settings.status() != QSettings::NoError) {
            CustomAlertDialog(tr("Passcode not saved"), tr("Unable to save the new passcode."),
                              CustomAlertDialog::Error, this)
                .exec();
            return;
        }
        ui->newPasscodeInput->clear();
        CustomAlertDialog(
            tr("Passcode updated"),
            tr("The passcode for %1 has been updated.").arg(ui->workshopCombo->currentText()),
            CustomAlertDialog::Info, this)
            .exec();
        ui->newPasscodeInput->setFocus();
    });
    connect(ui->closeBtn, &QPushButton::clicked, this, &QDialog::reject);
}
SystemSettingsDialog::~SystemSettingsDialog()
{
    delete ui;
}
