// Passcode හෝ index වැනි එක් text අගයක් ඉල්ලන dialog එකේ වැඩ මෙහි ඇත.
// #include වලින් මේ file එකට අවශ්‍ය Qt සහ project declarations ලබාගනී.
#include "custominputdialog.h"
#include "dialogtheme.h"
#include "ui_custominputdialog.h"

// එක් text box එකක් සහ OK/Cancel buttons සකසයි.
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
    // Input එකේ උපරිම දිගයි; passcode පෙන්වීමේ UI එක custominputdialog.ui හි වෙනස් කරන්න.
    ui->inputBox->setMaxLength(128);
    ui->inputBox->setClearButtonEnabled(true);
    ui->inputBox->setFocus();
    ui->okBtn->setDefault(true);
    // හිස් text එකක් සමඟ OK කිරීමට ඉඩ නොදී typing කළ විට පමණක් enable කරයි.
    ui->okBtn->setEnabled(false);
    connect(ui->inputBox, &QLineEdit::textChanged, this,
            [this](const QString &text) { ui->okBtn->setEnabled(!text.trimmed().isEmpty()); });
    // Enter එබූ විට හිස් නැති input එකක් නම් OK කළා සේ dialog එක පිළිගනී.
    connect(ui->inputBox, &QLineEdit::returnPressed, this, [this]() {
        if (!getInput().isEmpty())
            accept();
    });
    connect(ui->okBtn, &QPushButton::clicked, this, &QDialog::accept);
    connect(ui->cancelBtn, &QPushButton::clicked, this, &QDialog::reject);
}

// Input dialog එකේ UI helper එක නිදහස් කරයි.
CustomInputDialog::~CustomInputDialog()
{
    delete ui;
}

// ඇතුළත් කළ text එකේ මුල/අග spaces ඉවත් කර ලබාදෙයි.
QString CustomInputDialog::getInput() const
{
    return ui->inputBox->text().trimmed();
}
