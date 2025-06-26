// This file contains the implementation for the `FlightDialog` class,
// which allows users to create new flights and modify existing flight details
// within the Flight Operations Manager application. It handles user input
// and updates flight data accordingly.

#include "gui/FlightDialog.h"
#include "ui_FlightDialog.h"
#include <QMessageBox>
#include <QPushButton>
#include <QShortcut>
#include <QShowEvent>
#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>

FlightDialog::FlightDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FlightDialog)
{
    ui->setupUi(this);
    ui->departureEdit->setDateTime(QDateTime::currentDateTime());
    // Tooltips for accessibility
    ui->flightNumberEdit->setToolTip(tr("Enter the flight number."));
    ui->originEdit->setToolTip(tr("Enter the origin airport/city."));
    ui->destinationEdit->setToolTip(tr("Enter the destination airport/city."));
    ui->departureEdit->setToolTip(tr("Select the departure date and time."));
    if (auto* okBtn = ui->buttonBox->button(QDialogButtonBox::Ok))
        okBtn->setToolTip(tr("Save flight details and close dialog (Enter)"));
    if (auto* cancelBtn = ui->buttonBox->button(QDialogButtonBox::Cancel))
        cancelBtn->setToolTip(tr("Cancel and close dialog (Esc)"));
    // Tab order
    setTabOrder(ui->flightNumberEdit, ui->originEdit);
    setTabOrder(ui->originEdit, ui->destinationEdit);
    setTabOrder(ui->destinationEdit, ui->departureEdit);
    setTabOrder(ui->departureEdit, ui->buttonBox);
    // Keyboard shortcuts
    (void)new QShortcut(QKeySequence(Qt::Key_Escape), this, SLOT(reject()));
    (void)new QShortcut(QKeySequence(Qt::Key_Return), this, SLOT(accept()));
    (void)new QShortcut(QKeySequence(Qt::Key_Enter), this, SLOT(accept()));

    // Connect button validation
    connect(ui->flightNumberEdit, &QLineEdit::textChanged, this, &FlightDialog::validateForm);
    connect(ui->originEdit, &QLineEdit::textChanged, this, &FlightDialog::validateForm);
    connect(ui->destinationEdit, &QLineEdit::textChanged, this, &FlightDialog::validateForm);

    validateForm();

    // A professional, dark theme that matches the main window
    setStyleSheet(R"(
        QDialog {
            background-color: #2c3e50; /* Midnight Blue */
            color: #ecf0f1;
            font-family: "Segoe UI";
        }

        QLabel {
            color: #ecf0f1;
            font-size: 13px;
            font-weight: 500;
        }

        QLineEdit, QDateTimeEdit {
            background-color: #34495e; /* Wet Asphalt */
            color: #ecf0f1;
            border: 1px solid #2c3e50;
            border-radius: 4px;
            padding: 8px 12px;
            font-size: 13px;
        }

        QLineEdit:focus, QDateTimeEdit:focus {
            border-color: #f39c12; /* Orange Accent */
        }

        QPushButton {
            background-color: #f39c12;
            color: #2c3e50;
            border: none;
            border-radius: 4px;
            padding: 10px 24px;
            font-weight: bold;
        }

        QPushButton:hover {
            background-color: #f1c40f;
        }

        QPushButton:pressed {
            background-color: #d35400;
        }
    )");

    // Button press animation for OK/Cancel
    if (auto* okBtn = ui->buttonBox->button(QDialogButtonBox::Ok)) {
        connect(okBtn, &QPushButton::pressed, [okBtn]() {
            QPropertyAnimation* anim = new QPropertyAnimation(okBtn, "geometry");
            QRect start = okBtn->geometry();
            QRect shrink = start.adjusted(2, 2, -2, -2);
            anim->setDuration(80);
            anim->setStartValue(start);
            anim->setEndValue(shrink);
            anim->setEasingCurve(QEasingCurve::OutQuad);
            QObject::connect(anim, &QPropertyAnimation::finished, [okBtn, start]() {
                okBtn->setGeometry(start);
            });
            anim->start(QAbstractAnimation::DeleteWhenStopped);
        });
    }
    if (auto* cancelBtn = ui->buttonBox->button(QDialogButtonBox::Cancel)) {
        connect(cancelBtn, &QPushButton::pressed, [cancelBtn]() {
            QPropertyAnimation* anim = new QPropertyAnimation(cancelBtn, "geometry");
            QRect start = cancelBtn->geometry();
            QRect shrink = start.adjusted(2, 2, -2, -2);
            anim->setDuration(80);
            anim->setStartValue(start);
            anim->setEndValue(shrink);
            anim->setEasingCurve(QEasingCurve::OutQuad);
            QObject::connect(anim, &QPropertyAnimation::finished, [cancelBtn, start]() {
                cancelBtn->setGeometry(start);
            });
            anim->start(QAbstractAnimation::DeleteWhenStopped);
        });
    }
}

FlightDialog::~FlightDialog()
{
    delete ui;
}

void FlightDialog::setFlight(const Flight* flight)
{
    if (flight) {
        ui->flightNumberEdit->setText(flight->getFlightNumber());
        ui->originEdit->setText(flight->getOrigin());
        ui->destinationEdit->setText(flight->getDestination());
        ui->departureEdit->setDateTime(flight->getDepartureTime());
    }
}

void FlightDialog::accept()
{
    if (!validateForm())
    {
        QMessageBox::warning(this, "Incomplete Information", "Please fill out all fields correctly.");
        return;
    }

    QDialog::accept();
}

QString FlightDialog::flightNumber() const
{
    return ui->flightNumberEdit->text();
}

QString FlightDialog::origin() const
{
    return ui->originEdit->text();
}

QString FlightDialog::destination() const
{
    return ui->destinationEdit->text();
}

QDateTime FlightDialog::departureTime() const
{
    return ui->departureEdit->dateTime();
}

bool FlightDialog::validateForm()
{
    bool enabled = !ui->flightNumberEdit->text().trimmed().isEmpty() &&
                   !ui->originEdit->text().trimmed().isEmpty() &&
                   !ui->destinationEdit->text().trimmed().isEmpty() &&
                   (ui->originEdit->text().trimmed() != ui->destinationEdit->text().trimmed());
    if (ui->buttonBox->button(QDialogButtonBox::Ok)) {
        ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(enabled);
    }
    // Live input validation feedback
    auto setError = [](QLineEdit* edit, bool error) {
        if (error)
            edit->setStyleSheet("border: 2px solid #e74c3c; background: #34495e; color: #ecf0f1;");
        else
            edit->setStyleSheet("");
    };
    setError(ui->flightNumberEdit, ui->flightNumberEdit->text().trimmed().isEmpty());
    setError(ui->originEdit, ui->originEdit->text().trimmed().isEmpty());
    setError(ui->destinationEdit, ui->destinationEdit->text().trimmed().isEmpty() || ui->originEdit->text().trimmed() == ui->destinationEdit->text().trimmed());
    return enabled;
}

void FlightDialog::showEvent(QShowEvent* event) {
    QDialog::showEvent(event);
    QGraphicsOpacityEffect* effect = new QGraphicsOpacityEffect(this);
    setGraphicsEffect(effect);
    QPropertyAnimation* anim = new QPropertyAnimation(effect, "opacity", this);
    anim->setDuration(220);
    anim->setStartValue(0.0);
    anim->setEndValue(1.0);
    anim->setEasingCurve(QEasingCurve::InOutQuad);
    anim->start(QAbstractAnimation::DeleteWhenStopped);
}
