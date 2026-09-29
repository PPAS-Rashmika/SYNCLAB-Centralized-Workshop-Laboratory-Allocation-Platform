#include "custominputdialog.h"
#include "dialogtheme.h"
#include "ui_custominputdialog.h"

// Constructor: builds a dialog with one text box and OK / Cancel buttons.
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
    // OK stays greyed out until something has actually been typed.
    ui->okBtn->setEnabled(false);
    connect(ui->inputBox, &QLineEdit::textChanged, this,
            [this](const QString &text) { ui->okBtn->setEnabled(!text.trimmed().isEmpty()); });
    // Pressing Enter works the same as clicking OK.
    connect(ui->inputBox, &QLineEdit::returnPressed, this, [this]() {
        if (!getInput().isEmpty())
            accept();
    });
    connect(ui->okBtn, &QPushButton::clicked, this, &QDialog::accept);
    connect(ui->cancelBtn, &QPushButton::clicked, this, &QDialog::reject);
}

// Destructor
CustomInputDialog::~CustomInputDialog()
{
    delete ui;
}

// Returns what the user typed, with any spaces removed from both ends.
QString CustomInputDialog::getInput() const
{
    return ui->inputBox->text().trimmed();
}
