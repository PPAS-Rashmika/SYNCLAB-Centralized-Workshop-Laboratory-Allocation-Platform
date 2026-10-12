// Theme, wallpaper සහ administrator passcodes වෙනස් කරන dialog එකේ වැඩ මෙහි ඇත.
// #include වලින් මේ file එකට අවශ්‍ය Qt සහ project declarations ලබාගනී.
#include "systemsettingsdialog.h"
#include "customalertdialog.h"
#include "dialogtheme.h"
#include "ui_systemsettingsdialog.h"
#include "workshopcatalog.h"

#include <QFileDialog>
#include <QImageReader>
#include <QSettings>

// Theme, wallpaper සහ passcode controls සකසා buttons වල වැඩ සම්බන්ධ කරයි.
SystemSettingsDialog::SystemSettingsDialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::SystemSettingsDialog)
{
    ui->setupUi(this);
    applyDialogTheme(this);
    // Save කළ theme අනුව toggle button text එක තෝරයි.
    const bool dark = QSettings(QSettings::defaultFormat(), QSettings::UserScope,
                                "StudentWorkshopApp", "ThemeSettings")
                          .value("darkMode", false)
                          .toBool();
    ui->toggleThemeBtn->setText(dark ? tr("Switch to light mode") : tr("Switch to dark mode"));
    // Workshops දහය සහ master passcode සඳහා වෙනම item එකක් combo එකට දමයි.
    for (const QString &category : workshopCategories())
        ui->workshopCombo->addItem(category, category);
    ui->workshopCombo->addItem(tr("System administrator"), "__master__");
    ui->workshopCombo->setSizeAdjustPolicy(QComboBox::AdjustToMinimumContentsLengthWithIcon);
    ui->workshopCombo->setMinimumContentsLength(18);
    ui->lblCat->setBuddy(ui->workshopCombo);
    ui->lblCode->setBuddy(ui->newPasscodeInput);
    // නව admin passcode එකේ උපරිම අකුරු ගණනයි.
    ui->newPasscodeInput->setMaxLength(64);
    ui->newPasscodeInput->setClearButtonEnabled(true);
    ui->savePasscodeBtn->setEnabled(false);
    ui->closeBtn->setAutoDefault(false);
    ui->toggleThemeBtn->setAutoDefault(false);
    ui->changeWallpaperBtn->setAutoDefault(false);
    ui->savePasscodeBtn->setAutoDefault(false);
    // Passcode අකුරු අවම 4ක් තිබේ නම් Save button enable කරයි.
    connect(ui->newPasscodeInput, &QLineEdit::textChanged, this, [this](const QString &value) {
        ui->savePasscodeBtn->setEnabled(value.trimmed().size() >= 4);
    });
    // Workshop එක මාරු කළ විට කලින් type කළ passcode ඉවත් කරයි.
    connect(ui->workshopCombo, &QComboBox::currentIndexChanged, this,
            [this]() { ui->newPasscodeInput->clear(); });
    connect(ui->newPasscodeInput, &QLineEdit::returnPressed, ui->savePasscodeBtn,
            &QPushButton::click);
    // Theme setting එක මාරු කර MainWindow වෙත themeChanged signal එක යවයි.
    connect(ui->toggleThemeBtn, &QPushButton::clicked, this, [this, dark]() {
        QSettings settings(QSettings::defaultFormat(), QSettings::UserScope, "StudentWorkshopApp",
                           "ThemeSettings");
        settings.setValue("darkMode", !dark);
        settings.sync();
        if (settings.status() != QSettings::NoError) {
            // Title/message සහ OK/Cancel සකසයි; message වර්ගයට ගැළපෙන පාට තෝරයි.
            CustomAlertDialog(tr("Settings not saved"), tr("Unable to save the selected theme."),
                              CustomAlertDialog::Error, this)
                .exec();
            return;
        }
        // Theme මාරු කළ බව MainWindow වෙත දන්වන signal එකයි.
        emit themeChanged();
        accept();
    });
    // Image file එක තෝරා Qt ට එය කියවිය හැකි නම් path එක signal එකෙන් යවයි.
    connect(ui->changeWallpaperBtn, &QPushButton::clicked, this, [this]() {
        const QString path = QFileDialog::getOpenFileName(
            this, tr("Select background wallpaper"), QString(), tr("Images (*.png *.jpg *.jpeg)"));
        if (path.isEmpty())
            return;
        QImageReader reader(path);
        if (!reader.canRead()) {
            // Title/message සහ OK/Cancel සකසයි; message වර්ගයට ගැළපෙන පාට තෝරයි.
            CustomAlertDialog(tr("Invalid image"),
                              tr("The selected file cannot be opened as an image."),
                              CustomAlertDialog::Warning, this)
                .exec();
            return;
        }
        // තෝරාගත් wallpaper path එක MainWindow වෙත දන්වන signal එකයි.
        emit wallpaperChanged(path);
    });
    // තෝරාගත් workshop/master එක සඳහා passcode save කරයි.
    connect(ui->savePasscodeBtn, &QPushButton::clicked, this, [this]() {
        const QString code = ui->newPasscodeInput->text().trimmed();
        if (code.size() < 4)
            return;
        const QString category = ui->workshopCombo->currentData().toString();
        // Master සඳහා master_passcode; workshops සඳහා category_passcode settings key භාවිත කරයි.
        const QString key = category == "__master__" ? "master_passcode" : category + "_passcode";
        QSettings settings(QSettings::defaultFormat(), QSettings::UserScope, "StudentWorkshopApp",
                           "Passcodes");
        settings.setValue(key, code);
        settings.sync();
        if (settings.status() != QSettings::NoError) {
            // Title/message සහ OK/Cancel සකසයි; message වර්ගයට ගැළපෙන පාට තෝරයි.
            CustomAlertDialog(tr("Passcode not saved"), tr("Unable to save the new passcode."),
                              CustomAlertDialog::Error, this)
                .exec();
            return;
        }
        ui->newPasscodeInput->clear();
        // Title/message සහ OK/Cancel සකසයි; message වර්ගයට ගැළපෙන පාට තෝරයි.
        CustomAlertDialog(
            tr("Passcode updated"),
            tr("The passcode for %1 has been updated.").arg(ui->workshopCombo->currentText()),
            CustomAlertDialog::Info, this)
            .exec();
        ui->newPasscodeInput->setFocus();
    });
    connect(ui->closeBtn, &QPushButton::clicked, this, &QDialog::reject);
}
// System settings dialog එකේ UI helper එක නිදහස් කරයි.
SystemSettingsDialog::~SystemSettingsDialog()
{
    delete ui;
}
