#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "recordswindow.h"
#include <QMessageBox>
#include <QTextCharFormat>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QListView>
#include <QRandomGenerator>
#include <QInputDialog>
#include <QBrush>
#include <QColor>
#include <QDateTime>
#include <QFileDialog>
#include <QFile>
#include <QDir>
#include <QFileInfo>
#include <QCoreApplication>
#include <QGraphicsDropShadowEffect>
#include <QMap>
#include <QLineEdit>
#include <QSettings>
#include <QComboBox>
#include <QFrame>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    isPasscodeBypassed = false; 
    this->setWindowIcon(QIcon(QCoreApplication::applicationDirPath() + "/assets/app_icon.png"));

    dbManager = new SqliteDatabaseManager("student_data.db");
    if (!dbManager->createTable()) {
        QMessageBox::critical(this, "DB Error", dbManager->getLastError());
    }

    setupDynamicUI();
    
    bool isDark = QSettings("StudentWorkshopApp", "ThemeSettings").value("darkMode", true).toBool();
    applyTheme(isDark);
    
    connect(ui->backBtn, &QPushButton::clicked, this, &MainWindow::goBack);
    connect(ui->viewRecordsBtn, &QPushButton::clicked, this, &MainWindow::showRecordsWindow);
    connect(ui->calendar, &QCalendarWidget::clicked, this, &MainWindow::dateClicked);
    connect(ui->calendar, &QCalendarWidget::selectionChanged, this, &MainWindow::onCalendarSelectionChanged);
    connect(ui->submitBtn, &QPushButton::clicked, this, &MainWindow::submitData);
    connect(ui->updateBtn, &QPushButton::clicked, this, &MainWindow::updateData);
    connect(ui->deleteBtn, &QPushButton::clicked, this, &MainWindow::deleteData);
    connect(ui->systemSettingsBtn, &QPushButton::clicked, this, &MainWindow::openSystemSettings);
    
    connect(ui->browsePdfBtn, &QPushButton::clicked, this, [this](){
        QString filePath = QFileDialog::getOpenFileName(this, "Select PDF File", "", "PDF Documents (*.pdf)");
        if (!filePath.isEmpty()) {
            ui->pdfInput->setText(filePath);
        }
    });
}

MainWindow::~MainWindow()
{
    delete dbManager;
    delete ui;
}

void MainWindow::setupDynamicUI()
{
    ui->workshopsLayout->setAlignment(Qt::AlignCenter);
    ui->labsLayout->setAlignment(Qt::AlignCenter);

    QStandardItemModel *timeModel = new QStandardItemModel(4, 1, this);
    QStringList timeSlotsList = {"8.30 am - 10.30 am", "10.30 am - 12.30 pm", "1.30 pm - 3.30 pm", "3.30 pm - 5.30 pm"};
    for (int i = 0; i < timeSlotsList.size(); ++i) {
        QStandardItem *item = new QStandardItem(timeSlotsList[i]);
        item->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
        item->setCheckState(Qt::CheckState::Unchecked); 
        timeModel->setItem(i, 0, item);
    }
    ui->timeSlotInput->setModel(timeModel);
    
    QLineEdit *timeLineEdit = new QLineEdit(this);
    timeLineEdit->setReadOnly(true);
    timeLineEdit->setPlaceholderText("Select time slots...");
    ui->timeSlotInput->setLineEdit(timeLineEdit);
    
    connect(timeModel, &QStandardItemModel::itemChanged, this, &MainWindow::updateTimeSlotDisplayText);
    connect(ui->timeSlotInput->lineEdit(), &QLineEdit::textChanged, this, &MainWindow::updateTimeSlotDisplayText);
    
    QListView* listView = new QListView(ui->timeSlotInput);
    listView->setStyleSheet("QListView::item { padding: 8px; font-size: 13px; }");
    ui->timeSlotInput->setView(listView);
}

void MainWindow::applyTheme(bool isDark)
{
    // --- Wallpaper ගබඩාව පරීක්ෂා කිරීම ---
    QString wallpaperPath = QSettings("StudentWorkshopApp", "ThemeSettings").value("customWallpaper", "").toString();

    // 1st Interface Background සකස් කිරීම (Wallpaper එකක් ඇත්නම් එය යොදයි)
    if (!wallpaperPath.isEmpty() && QFile::exists(wallpaperPath)) {
        ui->page1->setStyleSheet(QString("QWidget#page1 { border-image: url('%1') 0 0 0 0 stretch stretch; }").arg(wallpaperPath));
    } else {
        if(isDark) {
            ui->page1->setStyleSheet("QWidget#page1 { background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #050B14, stop:0.5 #0F172A, stop:1 #020617); }");
        } else {
            ui->page1->setStyleSheet("QWidget#page1 { background-color: #F4F8FB; }");
        }
    }

    if(isDark) {
        ui->glassPanel->setStyleSheet(
            "QFrame#glassPanel {"
            "background-color: rgba(16, 25, 45, 180);"
            "border: 1px solid rgba(100, 200, 255, 40);"
            "border-radius: 20px;"
            "}"
        );
    } else {
        ui->glassPanel->setStyleSheet("QFrame#glassPanel { background-color: rgba(255, 255, 255, 200); border: 1px solid #CBD5E1; border-radius: 20px; }");
    }

    ui->bannerLabel->setStyleSheet(QString("background-color: transparent; color: %1; font-size: 16px; font-weight: 800; padding: 15px; letter-spacing: 5px;").arg(isDark ? "#38BDF8" : "#0A306D"));

    QString headerStyle = QString("font-size: 20px; font-weight: 800; color: %1; letter-spacing: 2px; background: transparent;").arg(isDark ? "#F8FAFC" : "#112A46");
    ui->label_workshops->setStyleSheet(headerStyle);
    ui->label_labs->setStyleSheet(headerStyle);
    
    QString lineStyle = QString("background-color: %1; max-height: 2px;").arg(isDark ? "rgba(255,255,255,50)" : "#D1D5DB");
    ui->line_w1->setStyleSheet(lineStyle);
    ui->line_w2->setStyleSheet(lineStyle);
    ui->line_l1->setStyleSheet(lineStyle);
    ui->line_l2->setStyleSheet(lineStyle);
    
    ui->systemSettingsBtn->setStyleSheet(QString("background-color: %1; color: %2; padding: 8px 15px; border-radius: 6px; font-weight: bold;").arg(isDark?"rgba(255,255,255,20)":"#E2E8F0", isDark?"#F8FAFC":"#0F172A"));

    // --- 2nd Interface Styles ---
    QString bg2 = isDark ? "#0B1120" : "#F4F8FB";
    QString cardBg = isDark ? "#1E293B" : "#FFFFFF";
    QString text = isDark ? "#F8FAFC" : "#0F172A";
    QString subText = isDark ? "#94A3B8" : "#5B6A7A";
    QString border = isDark ? "#334155" : "#CBD5E1";
    QString inputBg = isDark ? "#0F172A" : "#FFFFFF";
    QString btnBg = isDark ? "#38BDF8" : "#2563EB";
    QString btnText = isDark ? "#0B1120" : "#FFFFFF";

    ui->page2->setStyleSheet(QString("QWidget#page2 { background-color: %1; }").arg(bg2));

    ui->formHeader->setStyleSheet(QString("font-size: 16px; font-weight: bold; color: %1; margin-top: 15px; margin-bottom: 15px; text-transform: uppercase;").arg(isDark ? "#38BDF8" : text));

    QString labelStyle = QString("font-size: 12px; font-weight: bold; color: %1;").arg(text);
    ui->label->setStyleSheet(labelStyle);
    ui->label_2->setStyleSheet(labelStyle);
    ui->label_3->setStyleSheet(labelStyle);
    ui->label_5->setStyleSheet(labelStyle);
    ui->label_6->setStyleSheet(labelStyle);
    ui->label_email->setStyleSheet(labelStyle);
    ui->label_pdf->setStyleSheet(labelStyle);

    QString inputStyle = QString("padding: 0px 10px; min-height: 38px; border: 1px solid %1; border-radius: 6px; font-size: 14px; background-color: %2; color: %3;").arg(border, inputBg, text);
    ui->nameInput->setStyleSheet(inputStyle);
    ui->indexInput->setStyleSheet(inputStyle);
    ui->deptInput->setStyleSheet(inputStyle);
    ui->emailInput->setStyleSheet(inputStyle);
    ui->pdfInput->setStyleSheet(inputStyle);
    ui->timeSlotInput->setStyleSheet("QComboBox { " + inputStyle + "}");
    ui->timeSlotInput->lineEdit()->setStyleSheet(QString("background: transparent; border: none; color: %1; padding: 0px 5px; font-size: 14px;").arg(text));
    
    ui->descriptionInput->setStyleSheet(QString("border: 1px solid %1; border-radius: 6px; padding: 10px; font-size: 14px; background-color: %2; color: %3;").arg(border, inputBg, text));

    ui->infoCard->setStyleSheet(QString("QFrame { background-color: %1; border-radius: 10px; border: 1px solid %2; margin-top: 15px; }").arg(cardBg, border));
    ui->selectedNameLabel->setStyleSheet(QString("font-size: 18px; font-weight: 800; color: %1; border: none;").arg(text));
    ui->selectedDescLabel->setStyleSheet(QString("font-size: 13px; color: %1; border: none;").arg(subText));
    
    ui->backBtn->setStyleSheet(QString("background-color: %1; color: %2; padding: 10px 15px; border-radius: 5px; font-weight: bold; font-size: 13px;").arg(isDark?"#334155":"#E2E8F0", text));
    ui->viewRecordsBtn->setStyleSheet(QString("background-color: %1; color: %2; padding: 10px 15px; border-radius: 5px; font-weight: bold; font-size: 13px;").arg(btnBg, btnText));
    ui->browsePdfBtn->setStyleSheet("background-color: #64748B; color: white; padding: 0px 20px; min-height: 38px; border-radius: 6px; font-weight: bold; font-size: 13px;");
    
    ui->submitBtn->setStyleSheet(QString("background-color: %1; color: %2; padding: 0px; min-height: 45px; border-radius: 6px; font-weight: bold; font-size: 14px;").arg(btnBg, btnText));
    ui->updateBtn->setStyleSheet("background-color: #D97706; color: white; padding: 0px; min-height: 45px; border-radius: 6px; font-weight: bold; font-size: 14px;");
    ui->deleteBtn->setStyleSheet("background-color: #DC2626; color: white; padding: 0px; min-height: 45px; border-radius: 6px; font-weight: bold; font-size: 14px;");

    QString calStyle = isDark ? 
        "CustomCalendarWidget { background-color: #0B1120; color: #F8FAFC; }"
        "CustomCalendarWidget QWidget#qt_calendar_navigationbar { background-color: #1E293B; }"
        "CustomCalendarWidget QToolButton { color: #F8FAFC; font-weight: bold; background-color: transparent; }"
        "CustomCalendarWidget QToolButton::hover { background-color: #334155; border-radius: 4px; }"
        "CustomCalendarWidget QMenu { background-color: #1E293B; color: #F8FAFC; }"
        "CustomCalendarWidget QSpinBox { background-color: #1E293B; color: #F8FAFC; selection-background-color: #38BDF8; }"
        "CustomCalendarWidget QAbstractItemView { selection-background-color: #38BDF8; selection-color: #0B1120; background-color: #0B1120; color: #F8FAFC; border: 1px solid #334155; alternate-background-color: #1E293B; font-size: 13px; }"
        :
        "CustomCalendarWidget { background-color: #FFFFFF; color: #0F172A; }"
        "CustomCalendarWidget QWidget#qt_calendar_navigationbar { background-color: #F1F5F9; }"
        "CustomCalendarWidget QToolButton { color: #0F172A; font-weight: bold; background-color: transparent; }"
        "CustomCalendarWidget QToolButton::hover { background-color: #E2E8F0; border-radius: 4px; }"
        "CustomCalendarWidget QMenu { background-color: #FFFFFF; color: #0F172A; }"
        "CustomCalendarWidget QSpinBox { background-color: #FFFFFF; color: #0F172A; selection-background-color: #3B82F6; }"
        "CustomCalendarWidget QAbstractItemView { selection-background-color: #3B82F6; selection-color: #FFFFFF; background-color: #FFFFFF; color: #0F172A; border: 1px solid #CBD5E1; alternate-background-color: #F8FAFC; font-size: 13px; }";
    ui->calendar->setStyleSheet(calStyle);

    drawCards(isDark);
    updateCalendarHighlights(); 
}

void MainWindow::drawCards(bool isDark)
{
    QLayoutItem *child;
    while ((child = ui->workshopsLayout->takeAt(0)) != nullptr) {
        if (child->widget()) child->widget()->deleteLater();
        delete child;
    }
    while ((child = ui->labsLayout->takeAt(0)) != nullptr) {
        if (child->widget()) child->widget()->deleteLater();
        delete child;
    }
    
    QStringList workshops = {"Automotive", "Robotics", "Mechanical", "Painting", "Welding"};
    QStringList wsTitles = {"VEHICLE\nDESIGN", "ROBOTIC\nASSEMBLY", "GENERAL\nMECHANICAL", "SURFACE\nFINISHING", "PRECISION\nWELDING"};
    QStringList wsSubs = {"AUTOMOTIVE", "AUTOMATION", "HAND TOOLS", "COATING", "METAL FAB"};

    for(int i = 0; i < workshops.size(); i++) {
        ui->workshopsLayout->addWidget(createCard(workshops[i], wsTitles[i], wsSubs[i], isDark));
    }

    QStringList labs = {"Microscope", "Chemistry", "Coding", "Test Tubes", "Circuitry"};
    QStringList labTitles = {"BIO-IMAGING", "CHEMICAL\nANALYSIS", "SOFTWARE\nENGINEERING", "CIRCUIT\nASSEMBLY", "NEURAL\nCOMPUTATION"};
    QStringList labSubs = {"MICROSCOPY", "LAB-WARE", "CODE & AI", "ELECTRONICS", "DATA SCIENCE"};
    
    for(int i = 0; i < labs.size(); i++) {
        ui->labsLayout->addWidget(createCard(labs[i], labTitles[i], labSubs[i], isDark));
    }
}

QWidget* MainWindow::createCard(const QString &id, const QString &title, const QString &subtitle, bool isDark)
{
    QFrame *card = new QFrame(this);
    card->setFixedSize(190, 270); 
    
    if(isDark) {
        card->setStyleSheet(
            "QFrame { background-color: rgba(255, 255, 255, 15); border-radius: 15px; border: 1px solid rgba(255, 255, 255, 40); }"
            "QFrame:hover { background-color: rgba(255, 255, 255, 30); border: 1px solid rgba(56, 189, 248, 150); }"
        );
    } else {
        card->setStyleSheet(
            "QFrame { background-color: rgba(255, 255, 255, 200); border-radius: 15px; border: 1px solid #CBD5E1; }"
            "QFrame:hover { border: 1px solid #3B82F6; }"
        );
    }

    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect(card);
    shadow->setBlurRadius(20);
    shadow->setXOffset(0);
    shadow->setYOffset(6);
    shadow->setColor(isDark ? QColor(0, 0, 0, 100) : QColor(148, 163, 184, 40)); 
    card->setGraphicsEffect(shadow);
    
    QVBoxLayout *layout = new QVBoxLayout(card);
    layout->setContentsMargins(15, 20, 15, 20); 
    layout->setSpacing(8);
    layout->setAlignment(Qt::AlignTop | Qt::AlignHCenter);
    
    QLabel *imgBox = new QLabel(card);
    imgBox->setFixedSize(140, 110);
    imgBox->setAlignment(Qt::AlignCenter);
    
    QString grad = isDark ? "qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #0284C7, stop:1 #002855)" 
                          : "qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #2563EB, stop:1 #0284C7)";
    imgBox->setStyleSheet(QString("background: %1; border-radius: 12px; color: white; border: none;").arg(grad));
    
    QString imgName = id.toLower() + ".png"; 
    QString imgPath = QCoreApplication::applicationDirPath() + "/assets/" + imgName;
    QPixmap pix(imgPath);
    if (!pix.isNull()) {
        imgBox->setPixmap(pix.scaled(140, 110, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    } else {
        imgBox->setText("[ Image ]\n" + id);
    }
    
    QLabel *nameTag = new QLabel(title, card);
    nameTag->setAlignment(Qt::AlignCenter);
    nameTag->setStyleSheet(QString("font-size: 14px; font-weight: bold; color: %1; background: transparent; border: none;").arg(isDark?"#FFFFFF":"#0F172A"));

    QPushButton *btn = new QPushButton(subtitle, card);
    btn->setFixedSize(150, 36);
    
    if(isDark) {
        btn->setStyleSheet(
            "QPushButton { background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #06B6D4, stop:1 #3B82F6); color: white; border-radius: 18px; font-size: 11px; font-weight: bold; border: 1px solid rgba(255, 255, 255, 50); }"
            "QPushButton:hover { background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #3B82F6, stop:1 #06B6D4); }"
        );
    } else {
        btn->setStyleSheet(
            "QPushButton { background-color: #3B82F6; color: white; border-radius: 18px; font-size: 11px; font-weight: bold; border: none; }"
            "QPushButton:hover { background-color: #2563EB; }"
        );
    }

    connect(btn, &QPushButton::clicked, [this, id](){
        openSecondInterface(id);
    });

    layout->addWidget(imgBox, 0, Qt::AlignHCenter);
    layout->addWidget(nameTag, 0, Qt::AlignHCenter);
    layout->addStretch(); 
    layout->addWidget(btn, 0, Qt::AlignHCenter);
    
    return card;
}

void MainWindow::openSystemSettings()
{
    bool ok;
    QString enteredCode = QInputDialog::getText(this, "System Authentication", "Enter System Passcode:", QLineEdit::Password, "", &ok);
    if (!ok) return;
    
    if (enteredCode != "000000") {
        QMessageBox::critical(this, "Access Denied", "Incorrect System Passcode!");
        return;
    }

    QDialog dialog(this);
    dialog.setWindowTitle("System Settings");
    dialog.setMinimumWidth(450); // Width මදක් විශාල කර ඇත
    
    bool isDark = QSettings("StudentWorkshopApp", "ThemeSettings").value("darkMode", true).toBool();
    dialog.setStyleSheet(isDark ? "QDialog { background-color: #1E293B; color: #F8FAFC; } QLabel { color: #F8FAFC; font-weight: bold; } QComboBox, QLineEdit { background-color: #0F172A; color: #F8FAFC; padding: 5px; border: 1px solid #334155; border-radius: 4px; }" 
                                : "QDialog { background-color: #FFFFFF; color: #0F172A; } QLabel { color: #0F172A; font-weight: bold; } QComboBox, QLineEdit { padding: 5px; border: 1px solid #CBD5E1; border-radius: 4px; }");

    QVBoxLayout layout(&dialog);
    layout.setSpacing(15);
    layout.setContentsMargins(25, 25, 25, 25);

    // --- 1. Theme මාරු කිරීම ---
    QLabel *lblTheme = new QLabel("UI Theme Appearance:", &dialog);
    layout.addWidget(lblTheme);

    QPushButton *toggleBtn = new QPushButton(isDark ? "🌙 SWITCH TO LIGHT MODE" : "✨ SWITCH TO GLASS MODE", &dialog);
    toggleBtn->setStyleSheet(isDark ? "background-color: #F59E0B; color: #0F172A; font-weight: bold; padding: 10px; border-radius: 6px; font-size: 13px;" 
                                    : "background-color: #0F172A; color: #F8FAFC; font-weight: bold; padding: 10px; border-radius: 6px; font-size: 13px;");
    layout.addWidget(toggleBtn);

    QFrame *line1 = new QFrame(&dialog);
    line1->setFrameShape(QFrame::HLine);
    line1->setStyleSheet(isDark ? "background-color: #334155;" : "background-color: #CBD5E1;");
    layout.addWidget(line1);

    // --- 2. අලුතින් එක් කළ Wallpaper සැකසුම ---
    QLabel *lblWall = new QLabel("1st Interface Custom Wallpaper:", &dialog);
    layout.addWidget(lblWall);

    QHBoxLayout *wallLayout = new QHBoxLayout();
    QLineEdit *wallPathEdit = new QLineEdit(&dialog);
    wallPathEdit->setReadOnly(true);
    QString currentWall = QSettings("StudentWorkshopApp", "ThemeSettings").value("customWallpaper", "").toString();
    wallPathEdit->setText(currentWall.isEmpty() ? "Default Background" : currentWall);
    
    QPushButton *browseWallBtn = new QPushButton("Browse", &dialog);
    browseWallBtn->setStyleSheet("background-color: #3B82F6; color: white; font-weight: bold; padding: 6px 12px; border-radius: 4px;");
    
    QPushButton *clearWallBtn = new QPushButton("Clear", &dialog);
    clearWallBtn->setStyleSheet("background-color: #EF4444; color: white; font-weight: bold; padding: 6px 12px; border-radius: 4px;");

    wallLayout->addWidget(wallPathEdit);
    wallLayout->addWidget(browseWallBtn);
    wallLayout->addWidget(clearWallBtn);
    layout.addLayout(wallLayout);

    connect(browseWallBtn, &QPushButton::clicked, [&]() {
        QString filePath = QFileDialog::getOpenFileName(&dialog, "Select Wallpaper", "", "Images (*.png *.jpg *.jpeg)");
        if (!filePath.isEmpty()) {
            wallPathEdit->setText(filePath);
            QSettings("StudentWorkshopApp", "ThemeSettings").setValue("customWallpaper", filePath);
            bool currentDark = QSettings("StudentWorkshopApp", "ThemeSettings").value("darkMode", true).toBool();
            this->applyTheme(currentDark); // තෝරාගත් වහාම Wallpaper එක වෙනස් වේ
        }
    });

    connect(clearWallBtn, &QPushButton::clicked, [&]() {
        wallPathEdit->setText("Default Background");
        QSettings("StudentWorkshopApp", "ThemeSettings").setValue("customWallpaper", "");
        bool currentDark = QSettings("StudentWorkshopApp", "ThemeSettings").value("darkMode", true).toBool();
        this->applyTheme(currentDark); // තෝරාගත් වහාම Default එකට මාරු වේ
    });

    QFrame *line2 = new QFrame(&dialog);
    line2->setFrameShape(QFrame::HLine);
    line2->setStyleSheet(isDark ? "background-color: #334155;" : "background-color: #CBD5E1;");
    layout.addWidget(line2);

    // --- 3. Passcode වෙනස් කිරීම ---
    QLabel *lblPasscode = new QLabel("Change Workshop Admin Passcode:", &dialog);
    layout.addWidget(lblPasscode);

    QComboBox *workshopCombo = new QComboBox(&dialog);
    workshopCombo->addItems({"Automotive", "Robotics", "Mechanical", "Painting", "Welding", "Microscope", "Chemistry", "Coding", "Test Tubes", "Circuitry"});
    layout.addWidget(workshopCombo);

    QLineEdit *newPasscodeEdit = new QLineEdit(&dialog);
    newPasscodeEdit->setPlaceholderText("Enter new 7-digit passcode...");
    newPasscodeEdit->setEchoMode(QLineEdit::Password);
    layout.addWidget(newPasscodeEdit);

    QPushButton *savePassBtn = new QPushButton("Save New Passcode", &dialog);
    savePassBtn->setStyleSheet("background-color: #10B981; color: white; font-weight: bold; padding: 8px; border-radius: 4px;");
    layout.addWidget(savePassBtn);

    connect(savePassBtn, &QPushButton::clicked, [&]() {
        if (newPasscodeEdit->text().trimmed().isEmpty()) {
            QMessageBox::warning(&dialog, "Error", "Passcode cannot be empty!");
            return;
        }
        QSettings settings("StudentWorkshopApp", "Passcodes");
        settings.setValue(workshopCombo->currentText() + "_passcode", newPasscodeEdit->text().trimmed());
        QMessageBox::information(&dialog, "Success", workshopCombo->currentText() + " admin passcode updated successfully!");
        newPasscodeEdit->clear();
    });

    connect(toggleBtn, &QPushButton::clicked, [&]() {
        bool currentDark = QSettings("StudentWorkshopApp", "ThemeSettings").value("darkMode", true).toBool();
        QSettings("StudentWorkshopApp", "ThemeSettings").setValue("darkMode", !currentDark);
        this->applyTheme(!currentDark);
        dialog.accept();
    });

    QPushButton *closeBtn = new QPushButton("Close", &dialog);
    closeBtn->setStyleSheet("background-color: #64748B; color: white; font-weight: bold; padding: 8px; border-radius: 4px;");
    connect(closeBtn, &QPushButton::clicked, &dialog, &QDialog::accept);
    layout.addWidget(closeBtn);

    dialog.exec();
}

void MainWindow::updateTimeSlotDisplayText()
{
    QStringList selected = getSelectedTimeSlotsList();
    QString correctText = selected.isEmpty() ? "Select time slots..." : selected.join(", ");
    ui->timeSlotInput->lineEdit()->blockSignals(true);
    ui->timeSlotInput->lineEdit()->setText(correctText);
    ui->timeSlotInput->lineEdit()->blockSignals(false);
}

void MainWindow::onCalendarSelectionChanged()
{
    if (currentCategory.isEmpty()) return;

    QString dateStr = ui->calendar->selectedDate().toString("yyyy-MM-dd");
    QStringList bookedSlots = dbManager->getBookedTimeSlotsStringList(dateStr, currentCategory, currentRecord.getId());
    
    QStandardItemModel *model = qobject_cast<QStandardItemModel*>(ui->timeSlotInput->model());
    for (int i = 0; i < model->rowCount(); ++i) {
        QStandardItem *item = model->item(i);
        if (bookedSlots.contains(item->text())) {
            item->setEnabled(false); 
            item->setCheckState(Qt::CheckState::Unchecked); 
            item->setForeground(QBrush(Qt::gray)); 
        } else {
            item->setEnabled(true);
            item->setForeground(QBrush(Qt::black));
        }
    }
    updateTimeSlotDisplayText();
}

void MainWindow::openSecondInterface(const QString &category)
{
    currentCategory = category;
    
    QMap<QString, QString> wsIds = {
        {"Automotive", "WKS-AUTO-01"}, {"Robotics", "WKS-ROBO-02"},
        {"Mechanical", "WKS-MECH-03"}, {"Painting", "WKS-PNT-04"},
        {"Welding", "WKS-WELD-05"},    {"Microscope", "LAB-MICR-06"},
        {"Chemistry", "LAB-CHEM-07"},  {"Coding", "LAB-CODE-08"},
        {"Test Tubes", "LAB-TEST-09"}, {"Circuitry", "LAB-CIRC-10"}
    };
    currentWorkshopId = wsIds.value(category, "WKS-GEN-00");

    ui->formHeader->setText(category.toUpper() + " - STUDENT INFORMATION");
    ui->selectedNameLabel->setText(category.toUpper() + " WORKSHOP");

    QMap<QString, QString> descriptions = {
        {"Automotive", "Vehicle systems, maintenance, and automotive engineering technology."},
        {"Robotics", "Automation, robotic arms, and intelligent machine programming."},
        {"Mechanical", "Heavy machinery, tools, and mechanical component fabrication."},
        {"Painting", "Professional surface treatment, coatings, and material painting."},
        {"Welding", "Metal fabrication, joining processes, and industrial welding."},
        {"Microscope", "Nanomaterials analysis and microscopic optical evaluations."},
        {"Chemistry", "Organic chemistry, chemical synthesis, and molecular analysis."},
        {"Coding", "Software engineering, algorithms, data structures, and application development."},
        {"Test Tubes", "Sample preparation, fluid dynamics, and biochemical experimentation."},
        {"Circuitry", "PCB design, embedded systems, soldering, and analog/digital electronics."}
    };
    ui->selectedDescLabel->setText(descriptions.value(category, "Explore facilities, tools, and scheduled activities."));

    QString imgName = category.toLower() + ".png"; 
    QString imgPath = QCoreApplication::applicationDirPath() + "/assets/" + imgName;
    QPixmap pix(imgPath);
    
    if (!pix.isNull()) {
        ui->selectedImgLabel->setPixmap(pix.scaled(120, 90, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }
    
    updateCalendarHighlights();
    ui->stackedWidget->setCurrentIndex(1);
    dateClicked(ui->calendar->selectedDate());
}

void MainWindow::goBack()
{
    ui->stackedWidget->setCurrentIndex(0);
}

void MainWindow::showRecordsWindow()
{
    bool ok;
    QString enteredAdminCode = QInputDialog::getText(this, "Admin Authentication", "Enter Admin Passcode for " + currentCategory + ":", QLineEdit::Password, "", &ok);
    if (!ok) return; 
    
    QSettings settings("StudentWorkshopApp", "Passcodes");
    QString correctPasscode = settings.value(currentCategory + "_passcode", "0000000").toString();
    
    if (enteredAdminCode != correctPasscode) {
        QMessageBox::critical(this, "Access Denied", "Incorrect Admin Passcode!");
        return;
    }

    RecordsWindow *recordsWin = new RecordsWindow(dbManager, currentCategory, this);
    connect(recordsWin, &RecordsWindow::editRequested, this, &MainWindow::loadRecordForEdit);
    connect(recordsWin, &RecordsWindow::databaseUpdated, this, &MainWindow::onDatabaseUpdatedFromPopup);
    recordsWin->exec(); 
    delete recordsWin;
}

void MainWindow::loadRecordForEdit(const StudentRecord &record)
{
    loadRecordIntoUI(record);
    isPasscodeBypassed = true; 
}

void MainWindow::onDatabaseUpdatedFromPopup()
{
    updateCalendarHighlights();
    dateClicked(ui->calendar->selectedDate()); 
}

void MainWindow::clearUI()
{
    ui->nameInput->clear();
    ui->indexInput->clear();
    ui->deptInput->setCurrentIndex(0);
    ui->emailInput->clear();
    ui->pdfInput->clear();
    setTimeSlotsInUI(""); 
    ui->descriptionInput->clear();
    isPasscodeBypassed = false; 
    onCalendarSelectionChanged(); 
}

QStringList MainWindow::getSelectedTimeSlotsList()
{
    QStandardItemModel *model = qobject_cast<QStandardItemModel*>(ui->timeSlotInput->model());
    QStringList selectedSlots;
    for (int i = 0; i < model->rowCount(); ++i) {
        if (model->item(i)->checkState() == Qt::CheckState::Checked && model->item(i)->isEnabled()) {
            selectedSlots << model->item(i)->text();
        }
    }
    return selectedSlots;
}

void MainWindow::setTimeSlotsInUI(const QString &timeString)
{
    QStandardItemModel *model = qobject_cast<QStandardItemModel*>(ui->timeSlotInput->model());
    for (int i = 0; i < model->rowCount(); ++i) {
        if (!timeString.isEmpty() && timeString.contains(model->item(i)->text()) && model->item(i)->isEnabled()) {
            model->item(i)->setCheckState(Qt::CheckState::Checked); 
        } else {
            model->item(i)->setCheckState(Qt::CheckState::Unchecked);
        }
    }
    updateTimeSlotDisplayText();
}

bool MainWindow::checkPasscode()
{
    bool ok;
    QString enteredCode = QInputDialog::getText(this, "Authentication", "Enter Booking Passcode:", QLineEdit::Password, "", &ok);
    if (!ok) return false; 
    if (enteredCode != currentRecord.getPasscode()) {
        QMessageBox::critical(this, "Access Denied", "Incorrect Passcode!");
        return false;
    }
    return true;
}

QString MainWindow::savePdfFile(const QString &sourcePath, const QString &bookingId)
{
    if (sourcePath.isEmpty()) return "";
    QString uploadDirPath = QCoreApplication::applicationDirPath() + "/uploads/pdfs";
    if (sourcePath.startsWith(uploadDirPath)) return sourcePath;

    QDir dir(uploadDirPath);
    if (!dir.exists()) dir.mkpath("."); 

    QString destPath = dir.absoluteFilePath(bookingId + ".pdf");
    if (sourcePath == destPath) return destPath;
    if (QFile::exists(destPath)) QFile::remove(destPath);
    if (QFile::copy(sourcePath, destPath)) return destPath; 
    return sourcePath; 
}

void MainWindow::submitData()
{
    QStringList selectedSlots = getSelectedTimeSlotsList();
    if(ui->nameInput->text().isEmpty() || ui->indexInput->text().isEmpty() || selectedSlots.isEmpty()) {
        QMessageBox::warning(this, "Input Error", "Name, Index and Time Slot required!");
        return;
    }

    QString dateStr = ui->calendar->selectedDate().toString("yyyy-MM-dd");
    if (!dbManager->isTimeSlotAvailable(dateStr, currentCategory, selectedSlots)) {
        QMessageBox::critical(this, "Time Slot Booked", "Selected time slot is already booked!");
        return;
    }

    QString bkgId = "BKG-" + QDateTime::currentDateTime().toString("yyMMddHH") + QString::number(QRandomGenerator::global()->bounded(100, 999));
    QString securePdfPath = savePdfFile(ui->pdfInput->text(), bkgId);
    QString pinStr = QString::number(QRandomGenerator::global()->bounded(1000, 10000));
    QString finalTimeStr = selectedSlots.join(", ");
    QString currentTimestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm AP");

    StudentRecord newRecord;
    newRecord.setBookingId(bkgId);
    newRecord.setWorkshopId(currentWorkshopId);
    newRecord.setCategory(currentCategory);
    newRecord.setName(ui->nameInput->text());
    newRecord.setIndexNo(ui->indexInput->text());
    newRecord.setDepartment(ui->deptInput->currentText());
    newRecord.setEmail(ui->emailInput->text());
    newRecord.setPdfPath(securePdfPath); 
    newRecord.setRecordDate(dateStr); 
    newRecord.setTimeSlots(finalTimeStr);
    newRecord.setDescription(ui->descriptionInput->toPlainText());
    newRecord.setPasscode(pinStr);
    newRecord.setCreatedAt(currentTimestamp);
    newRecord.setStatus("Pending"); 

    if (dbManager->addRecord(newRecord)) {
        QMessageBox::information(this, "Success", QString("Data saved!\nBooking ID: %1\nPasscode: %2").arg(bkgId, pinStr));
        updateCalendarHighlights();
        currentRecord = StudentRecord();
        dateClicked(ui->calendar->selectedDate()); 
    } else {
        QMessageBox::critical(this, "Database Error", dbManager->getLastError());
    }
}

void MainWindow::updateData()
{
    if (currentRecord.getId() == -1) {
        QString dateStr = ui->calendar->selectedDate().toString("yyyy-MM-dd");
        QList<StudentRecord> records = dbManager->getRecordsByDateAndCategory(dateStr, currentCategory);
        if (records.isEmpty()) return;
        
        bool ok;
        QString enteredCode = QInputDialog::getText(this, "Authentication", "Enter your Booking Passcode to Update:", QLineEdit::Password, "", &ok);
        if (!ok || enteredCode.isEmpty()) return;
        
        StudentRecord foundRecord;
        bool found = false;
        for(const StudentRecord& r : records) {
            if(r.getPasscode() == enteredCode) { foundRecord = r; found = true; break; }
        }
        
        if(!found) { QMessageBox::critical(this, "Access Denied", "Incorrect Passcode or no matching booking found!"); return; }
        
        loadRecordIntoUI(foundRecord);
        isPasscodeBypassed = true; 
        QMessageBox::information(this, "Authenticated", "Booking loaded successfully.\nYou can now make changes and click 'SAVE CHANGES'.");
        return;
    }
    
    QStringList selectedSlots = getSelectedTimeSlotsList();
    if(selectedSlots.isEmpty()) { QMessageBox::warning(this, "Input Error", "Please select at least one time slot!"); return; }
    if(ui->nameInput->text().isEmpty() || ui->indexInput->text().isEmpty()) { QMessageBox::warning(this, "Input Error", "Name and Index are required!"); return; }

    QString dateStr = ui->calendar->selectedDate().toString("yyyy-MM-dd");
    if (!dbManager->isTimeSlotAvailable(dateStr, currentCategory, selectedSlots, currentRecord.getId())) {
        QMessageBox::critical(this, "Time Slot Booked", "Time slot taken by another booking!"); return;
    }

    if (!isPasscodeBypassed) { if (!checkPasscode()) return; }
    
    QString securePdfPath = savePdfFile(ui->pdfInput->text(), currentRecord.getBookingId());
    
    currentRecord.setName(ui->nameInput->text());
    currentRecord.setIndexNo(ui->indexInput->text());
    currentRecord.setDepartment(ui->deptInput->currentText());
    currentRecord.setEmail(ui->emailInput->text());
    currentRecord.setPdfPath(securePdfPath); 
    currentRecord.setRecordDate(dateStr);
    currentRecord.setTimeSlots(selectedSlots.join(", "));
    currentRecord.setDescription(ui->descriptionInput->toPlainText());

    if (dbManager->updateRecord(currentRecord)) {
        QMessageBox::information(this, "Updated", "Record updated successfully!");
        updateCalendarHighlights();
        dateClicked(ui->calendar->selectedDate()); 
    } else {
        QMessageBox::critical(this, "Database Error", dbManager->getLastError());
    }
}

void MainWindow::deleteData()
{
    if (currentRecord.getId() == -1) {
        QString dateStr = ui->calendar->selectedDate().toString("yyyy-MM-dd");
        QList<StudentRecord> records = dbManager->getRecordsByDateAndCategory(dateStr, currentCategory);
        if (records.isEmpty()) return;
        
        bool ok;
        QString enteredCode = QInputDialog::getText(this, "Authentication", "Enter your Booking Passcode to Delete:", QLineEdit::Password, "", &ok);
        if (!ok || enteredCode.isEmpty()) return;
        
        StudentRecord foundRecord;
        bool found = false;
        for(const StudentRecord& r : records) {
            if(r.getPasscode() == enteredCode) { foundRecord = r; found = true; break; }
        }
        
        if(!found) { QMessageBox::critical(this, "Access Denied", "Incorrect Passcode!"); return; }
        
        if (QMessageBox::question(this, "Confirm", "Permanently delete booking ID: " + foundRecord.getBookingId() + "?") == QMessageBox::StandardButton::Yes) {
            if (dbManager->deleteRecord(foundRecord.getId())) {
                QMessageBox::information(this, "Deleted", "Record deleted successfully!");
                updateCalendarHighlights();
                dateClicked(ui->calendar->selectedDate());
            }
        }
        return;
    }
    
    if (!isPasscodeBypassed) { if (!checkPasscode()) return; }

    if (QMessageBox::question(this, "Confirm", "Permanently delete this record?") == QMessageBox::StandardButton::Yes) {
        if (dbManager->deleteRecord(currentRecord.getId())) {
            QMessageBox::information(this, "Deleted", "Record deleted successfully!");
            updateCalendarHighlights();
            dateClicked(ui->calendar->selectedDate());
        }
    }
}

void MainWindow::updateCalendarHighlights()
{
    if (currentCategory.isEmpty()) return;
    QTextCharFormat defaultFormat;
    ui->calendar->setDateTextFormat(QDate(), defaultFormat); 

    QList<StudentRecord> allRecords = dbManager->getAllRecordsForCategory(currentCategory);
    QMap<QDate, QMap<QString, int>> dateCounts;
    QList<QDate> bookedDates;

    for (const StudentRecord &r : allRecords) {
        QDate d = QDate::fromString(r.getRecordDate(), "yyyy-MM-dd");
        if (d.isValid()) {
            if (!bookedDates.contains(d)) bookedDates.append(d);
            dateCounts[d][r.getStatus()]++;
        }
    }

    bool isDark = QSettings("StudentWorkshopApp", "ThemeSettings").value("darkMode", true).toBool();
    QTextCharFormat highlightFormat;
    highlightFormat.setBackground(isDark ? QColor("#0369A1") : QColor("#BAE6FD"));
    highlightFormat.setForeground(isDark ? QColor("#F8FAFC") : QColor("#0C4A6E"));
    highlightFormat.setFontWeight(QFont::Bold);

    for (const QDate &date : bookedDates) {
        ui->calendar->setDateTextFormat(date, highlightFormat);
    }
    ui->calendar->setStatusCounts(dateCounts);
}

void MainWindow::loadRecordIntoUI(const StudentRecord &record)
{
    currentRecord = record;
    onCalendarSelectionChanged(); 
    ui->nameInput->setText(currentRecord.getName());
    ui->indexInput->setText(currentRecord.getIndexNo());
    ui->deptInput->setCurrentText(currentRecord.getDepartment());
    ui->emailInput->setText(currentRecord.getEmail());
    ui->pdfInput->setText(currentRecord.getPdfPath());
    ui->calendar->setSelectedDate(QDate::fromString(currentRecord.getRecordDate(), "yyyy-MM-dd"));
    setTimeSlotsInUI(currentRecord.getTimeSlots());
    ui->descriptionInput->setText(currentRecord.getDescription());
    
    ui->submitBtn->hide();
    ui->updateBtn->show();
    ui->updateBtn->setText("SAVE CHANGES");
    ui->deleteBtn->show();
    ui->deleteBtn->setText("DELETE BOOKING");
}

void MainWindow::dateClicked(const QDate &date)
{
    isPasscodeBypassed = false; 
    currentRecord = StudentRecord(); 
    clearUI(); 
    
    QString dateStr = date.toString("yyyy-MM-dd");
    QList<StudentRecord> records = dbManager->getRecordsByDateAndCategory(dateStr, currentCategory);
    
    ui->submitBtn->show();
    ui->submitBtn->setText("SUBMIT"); 
    
    if (records.isEmpty()) {
        ui->updateBtn->hide();
        ui->deleteBtn->hide();
    } else {
        ui->updateBtn->show();
        ui->updateBtn->setText("UPDATE BOOKING");
        ui->deleteBtn->show();
        ui->deleteBtn->setText("DELETE BOOKING");
    }
}