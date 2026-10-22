// තොරතුරු, දෝෂ සහ තහවුරු කිරීමේ පණිවිඩ පෙන්වයි. දිග පණිවිඩයට අනුව dialog එක ලොකු කරයි.
// #include වලින් මේ file එකට අවශ්‍ය Qt සහ project declarations ලබාගනී.
#include "customalertdialog.h"
#include "dialogtheme.h"
#include "ui_customalertdialog.h"

#include <QResizeEvent>

// Title/message සහ OK/Cancel සකසයි; message වර්ගයට ගැළපෙන පාට තෝරයි.
CustomAlertDialog::CustomAlertDialog(const QString &title, const QString &message, AlertType type,
                                     QWidget *parent)
    : QDialog(parent), ui(new Ui::CustomAlertDialog)
{
    ui->setupUi(this);
    setWindowTitle(title);
    ui->titleLbl->setText(title);
    ui->titleLbl->setWordWrap(true);
    ui->messageLbl->setText(message);
    // Message එක select කර copy කිරීමට ඉඩ දෙයි.
    ui->messageLbl->setTextInteractionFlags(Qt::TextSelectableByMouse |
                                            Qt::TextSelectableByKeyboard);
    // Yes/No තහවුරු කිරීමක් සඳහා පමණක් Cancel button පෙන්වයි.
    ui->cancelBtn->setVisible(type == Question);

    // Info blue, Warning amber, Error red වේ; මේ colour codes වෙනස් කළ හැක.
    const QString alertType = type == Warning ? "warning" : type == Error ? "error" : "info";
    applyDialogTheme(this, alertType);
    // දිග message එකට ගළපන විට මුල් dialog/control ප්‍රමාණ මෙතැනින් ලබාගනී.
    setProperty("_designerSize", size());
    for (QWidget *widget : {static_cast<QWidget *>(ui->titleLbl), static_cast<QWidget *>(ui->messageLbl),
                            static_cast<QWidget *>(ui->okBtn), static_cast<QWidget *>(ui->cancelBtn)})
        widget->setProperty("_designerRect", widget->geometry());

    // Question එකකට Enter නිසා නොදැනී Confirm නොවීමට Cancel මුලින් තෝරයි.
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

// මෙම dialog එකේ UI helper එක නිදහස් කරයි.
CustomAlertDialog::~CustomAlertDialog()
{
    delete ui;
}

// Allocation key වැනි text එක message එක තුළ ලොකු පැහැදිලි badge එකක් ලෙස පෙන්වයි.
void CustomAlertDialog::highlightText(const QString &text)
{
    if (text.isEmpty())
        return;
    // Text එක HTML වීමට පෙර &, < වැනි අකුරු ආරක්ෂිත text ලෙස හරවයි.
    const QString escapedText = text.toHtmlEscaped();
    // Highlight කරන key එකේ පාට, font size සහ bold බව මෙහි වෙනස් කරන්න.
    const QString badge = QStringLiteral("<span style=\"%1\">&nbsp;%2&nbsp;</span>")
                              .arg(ui->messageLbl->property("highlightCss").toString().toHtmlEscaped(),
                                   escapedText);
    QString html = ui->messageLbl->text().toHtmlEscaped();
    html.replace(escapedText, badge);
    // සාමාන්‍ය newline එක rich text හි line break එකකට හරවයි.
    html.replace(QLatin1Char('\n'), QStringLiteral("<br>"));
    ui->messageLbl->setTextFormat(Qt::RichText);
    ui->messageLbl->setText(html);
    fitMessage();
}

// දිග message එක නොකැපෙන ලෙස dialog උස සහ buttons වල ස්ථාන සකසයි.
void CustomAlertDialog::fitMessage()
{
    // Resize වැඩේ නැවත නැවත තමන්ම කැඳවීම නවතයි.
    if (!property("_designerSize").isValid() || property("_fittingMessage").toBool())
        return;
    setProperty("_fittingMessage", true);
    const QSize originalSize = property("_designerSize").toSize();
    const int extraWidth = qMax(0, width() - originalSize.width());
    QRect title = ui->titleLbl->property("_designerRect").toRect();
    QRect message = ui->messageLbl->property("_designerRect").toRect();
    title.setWidth(title.width() + extraWidth);
    // Title එකට තවත් උස අවශ්‍යදැයි අකුරු wrap වීමෙන් ගණනය කරයි.
    const int titleExtra = qMax(0, ui->titleLbl->heightForWidth(title.width()) - title.height());
    title.setHeight(title.height() + titleExtra);
    message.setWidth(message.width() + extraWidth);
    // Message එකේ සියලු පේළි පෙනීමට අවශ්‍ය අමතර උස ගනී.
    const int messageExtra = qMax(0, ui->messageLbl->heightForWidth(message.width()) - message.height());
    message.translate(0, titleExtra);
    message.setHeight(message.height() + messageExtra);
    ui->titleLbl->setGeometry(title);
    ui->messageLbl->setGeometry(message);
    // Title/message ලොකු වූ තරමට buttons පහළට සහ දකුණට ගෙන යයි.
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

// Dialog resize වූ විට title/message සහ buttons නැවත ගළපයි.
void CustomAlertDialog::resizeEvent(QResizeEvent *event)
{
    // Dialog resize වූ විට title/message සහ buttons නැවත ගළපයි.
    QDialog::resizeEvent(event);
    fitMessage();
}
