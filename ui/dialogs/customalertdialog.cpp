#include "customalertdialog.h"
#include "dialogtheme.h"
#include "ui_customalertdialog.h"

#include <QResizeEvent>

// Constructor: builds the dialog and colours it for the kind of message.
CustomAlertDialog::CustomAlertDialog(const QString &title, const QString &message, AlertType type,
                                     QWidget *parent)
    : QDialog(parent), ui(new Ui::CustomAlertDialog)
{
    ui->setupUi(this);
    setWindowTitle(title);
    ui->titleLbl->setText(title);
    ui->titleLbl->setWordWrap(true);
    ui->messageLbl->setText(message);
    // Let the user select and copy the message, which matters for error text.
    ui->messageLbl->setTextInteractionFlags(Qt::TextSelectableByMouse |
                                            Qt::TextSelectableByKeyboard);
    // Only a question gets a Cancel button.
    ui->cancelBtn->setVisible(type == Question);

    // Blue for information, amber for a warning, red for an error.
    QString accent = "#2563EB";
    if (type == Warning)
        accent = "#92400E";
    else if (type == Error)
        accent = "#B91C1C";
    applyDialogTheme(this, accent);
    setProperty("_designerSize", size());
    for (QWidget *widget : {static_cast<QWidget *>(ui->titleLbl), static_cast<QWidget *>(ui->messageLbl),
                            static_cast<QWidget *>(ui->okBtn), static_cast<QWidget *>(ui->cancelBtn)})
        widget->setProperty("_designerRect", widget->geometry());

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
    fitMessage();
}

// Destructor
CustomAlertDialog::~CustomAlertDialog()
{
    delete ui;
}

// Turns the message into rich text and draws every copy of "text" as a badge,
// so the student's allocation key stands out in the allocation-saved message.
// The message is escaped first, because a workshop name such as "Electrical
// Machines & Communications Laboratory" contains an ampersand that rich text
// would otherwise read as the start of a special character.
void CustomAlertDialog::highlightText(const QString &text)
{
    if (text.isEmpty())
        return;
    const QString escapedText = text.toHtmlEscaped();
    const QString badge = QStringLiteral("<span style=\"background-color:#DBEAFE; color:#1E3A8A;"
                                         " font-size:20pt; font-weight:bold;\">&nbsp;%1&nbsp;</span>")
                              .arg(escapedText);
    // Escaping happens before the badge is inserted, or the badge's own tags
    // would be escaped too. A newline only breaks a line in rich text as <br>.
    QString html = ui->messageLbl->text().toHtmlEscaped();
    html.replace(escapedText, badge);
    html.replace(QLatin1Char('\n'), QStringLiteral("<br>"));
    ui->messageLbl->setTextFormat(Qt::RichText);
    ui->messageLbl->setText(html);
    fitMessage();
}

// Dynamic messages can be longer than Designer's preview text. Expand only
// enough to fit them, retaining the authored short-message geometry.
void CustomAlertDialog::fitMessage()
{
    if (!property("_designerSize").isValid() || property("_fittingMessage").toBool())
        return;
    setProperty("_fittingMessage", true);
    const QSize originalSize = property("_designerSize").toSize();
    const int extraWidth = qMax(0, width() - originalSize.width());
    QRect title = ui->titleLbl->property("_designerRect").toRect();
    QRect message = ui->messageLbl->property("_designerRect").toRect();
    title.setWidth(title.width() + extraWidth);
    const int titleExtra = qMax(0, ui->titleLbl->heightForWidth(title.width()) - title.height());
    title.setHeight(title.height() + titleExtra);
    message.setWidth(message.width() + extraWidth);
    const int messageExtra = qMax(0, ui->messageLbl->heightForWidth(message.width()) - message.height());
    message.translate(0, titleExtra);
    message.setHeight(message.height() + messageExtra);
    ui->titleLbl->setGeometry(title);
    ui->messageLbl->setGeometry(message);
    for (QPushButton *button : {ui->okBtn, ui->cancelBtn}) {
        QRect rect = button->property("_designerRect").toRect();
        rect.translate(extraWidth, titleExtra + messageExtra);
        button->setGeometry(rect);
    }
    const int requiredHeight = originalSize.height() + titleExtra + messageExtra;
    setMinimumHeight(requiredHeight);
    if (height() < requiredHeight)
        resize(width(), requiredHeight);
    setProperty("_fittingMessage", false);
}

void CustomAlertDialog::resizeEvent(QResizeEvent *event)
{
    QDialog::resizeEvent(event);
    fitMessage();
}
