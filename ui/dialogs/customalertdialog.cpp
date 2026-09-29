#include "customalertdialog.h"
#include "dialogtheme.h"
#include "ui_customalertdialog.h"

#include <QResizeEvent>

CustomAlertDialog::CustomAlertDialog(const QString &title, const QString &message, AlertType type,
                                     QWidget *parent)
    : QDialog(parent), ui(new Ui::CustomAlertDialog)
{
    ui->setupUi(this);
    setWindowTitle(title);
    ui->titleLbl->setText(title);
    ui->titleLbl->setWordWrap(true);
    ui->messageLbl->setText(message);
    ui->messageLbl->setTextInteractionFlags(Qt::TextSelectableByMouse |
                                            Qt::TextSelectableByKeyboard);
    ui->cancelBtn->setVisible(type == Question);

    QString accent = "#2563EB";
    if (type == Warning)
        accent = "#92400E";
    else if (type == Error)
        accent = "#B91C1C";
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
CustomAlertDialog::~CustomAlertDialog()
{
    delete ui;
}

void CustomAlertDialog::resizeEvent(QResizeEvent *event)
{
    QDialog::resizeEvent(event);
    const QMargins margins = ui->verticalLayout->contentsMargins();
    const int availableWidth = qMax(1, width() - margins.left() - margins.right());
    ui->messageLbl->setMinimumHeight(ui->messageLbl->heightForWidth(availableWidth));
}
