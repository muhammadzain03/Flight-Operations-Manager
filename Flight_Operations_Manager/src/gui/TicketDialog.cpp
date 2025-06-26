// This file contains the implementation for the `TicketDialog` class,
// which enables users to book new tickets and view/modify existing ticket details
// within the Flight Operations Manager application. It integrates with pricing
// and passenger data to facilitate ticket management.

#include "gui/TicketDialog.h"
#include <QMessageBox>
#include <QGridLayout>
#include <QDateTime>
#include <QShortcut>
#include <QShowEvent>
#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>

TicketDialog::TicketDialog(Flight* flight, QWidget* parent)
    : QDialog(parent)
    , flight(flight)
{
    setupUi();
    if (flight) {
        updateAvailableSeats();
    }
    // Apply the modern, glossy stylesheet
    setStyleSheet(R"(
        QDialog {
            background-color: #2c3e50;
            color: #ecf0f1;
            font-family: 'Segoe UI';
        }
        QLabel {
            color: #ecf0f1;
            font-size: 13px;
            font-weight: 500;
        }
        QLineEdit, QComboBox {
            background-color: #34495e;
            color: #ecf0f1;
            border: 1px solid #2c3e50;
            border-radius: 4px;
            padding: 8px 12px;
            font-size: 13px;
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
}

void TicketDialog::setupUi() {
    auto* layout = new QGridLayout(this);

    // Create input fields
    passengerNameEdit = new QLineEdit(this);
    phoneNumberEdit = new QLineEdit(this);
    seatClassCombo = new QComboBox(this);
    seatNumberCombo = new QComboBox(this);
    fareLabel = new QLabel(this);
    confirmButton = new QPushButton(tr("Confirm"), this);
    cancelButton = new QPushButton(tr("Cancel"), this);

    // Setup seat class combo
    seatClassCombo->addItem(tr("Economy"), static_cast<int>(Ticket::Class::Economy));
    seatClassCombo->addItem(tr("Business"), static_cast<int>(Ticket::Class::Business));
    seatClassCombo->addItem(tr("First Class"), static_cast<int>(Ticket::Class::FirstClass));

    // Add widgets to layout
    int row = 0;
    layout->addWidget(new QLabel(tr("Passenger Name:")), row, 0);
    layout->addWidget(passengerNameEdit, row++, 1);

    layout->addWidget(new QLabel(tr("Phone Number:")), row, 0);
    layout->addWidget(phoneNumberEdit, row++, 1);

    layout->addWidget(new QLabel(tr("Seat Class:")), row, 0);
    layout->addWidget(seatClassCombo, row++, 1);

    layout->addWidget(new QLabel(tr("Seat Number:")), row, 0);
    layout->addWidget(seatNumberCombo, row++, 1);

    layout->addWidget(new QLabel(tr("Fare:")), row, 0);
    layout->addWidget(fareLabel, row++, 1);

    auto* buttonLayout = new QHBoxLayout;
    buttonLayout->addWidget(confirmButton);
    buttonLayout->addWidget(cancelButton);
    layout->addLayout(buttonLayout, row, 0, 1, 2);

    // Connect signals
    connect(seatClassCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &TicketDialog::onClassChanged);
    connect(confirmButton, &QPushButton::clicked, this, &TicketDialog::onConfirm);
    connect(cancelButton, &QPushButton::clicked, this, &TicketDialog::onCancel);

    // Button press animation for Confirm/Cancel
    connect(confirmButton, &QPushButton::pressed, [this]() {
        QPropertyAnimation* anim = new QPropertyAnimation(confirmButton, "geometry");
        QRect start = confirmButton->geometry();
        QRect shrink = start.adjusted(2, 2, -2, -2);
        anim->setDuration(80);
        anim->setStartValue(start);
        anim->setEndValue(shrink);
        anim->setEasingCurve(QEasingCurve::OutQuad);
        QObject::connect(anim, &QPropertyAnimation::finished, [this, start]() {
            confirmButton->setGeometry(start);
        });
        anim->start(QAbstractAnimation::DeleteWhenStopped);
    });
    connect(cancelButton, &QPushButton::pressed, [this]() {
        QPropertyAnimation* anim = new QPropertyAnimation(cancelButton, "geometry");
        QRect start = cancelButton->geometry();
        QRect shrink = start.adjusted(2, 2, -2, -2);
        anim->setDuration(80);
        anim->setStartValue(start);
        anim->setEndValue(shrink);
        anim->setEasingCurve(QEasingCurve::OutQuad);
        QObject::connect(anim, &QPropertyAnimation::finished, [this, start]() {
            cancelButton->setGeometry(start);
        });
        anim->start(QAbstractAnimation::DeleteWhenStopped);
    });

    setWindowTitle(tr("Book Ticket"));
    updateFare();

    // Tooltips for accessibility
    passengerNameEdit->setToolTip(tr("Enter the passenger's full name."));
    phoneNumberEdit->setToolTip(tr("Enter the passenger's phone number."));
    seatClassCombo->setToolTip(tr("Select the seat class."));
    seatNumberCombo->setToolTip(tr("Select the seat number."));
    fareLabel->setToolTip(tr("Displays the fare for the selected seat/class."));
    confirmButton->setToolTip(tr("Confirm ticket booking (Enter)"));
    cancelButton->setToolTip(tr("Cancel and close dialog (Esc)"));

    // Tab order
    setTabOrder(passengerNameEdit, phoneNumberEdit);
    setTabOrder(phoneNumberEdit, seatClassCombo);
    setTabOrder(seatClassCombo, seatNumberCombo);
    setTabOrder(seatNumberCombo, confirmButton);
    setTabOrder(confirmButton, cancelButton);

    // Keyboard shortcuts
    (void)new QShortcut(QKeySequence(Qt::Key_Escape), this, SLOT(onCancel()));
    (void)new QShortcut(QKeySequence(Qt::Key_Return), this, SLOT(onConfirm()));
    (void)new QShortcut(QKeySequence(Qt::Key_Enter), this, SLOT(onConfirm()));
}

void TicketDialog::setFlight(Flight* newFlight) {
    flight = newFlight;
    if (flight) {
        updateAvailableSeats();
    }
}

void TicketDialog::onConfirm() {
    if (!validateInputs()) {
        return;
    }

    createTicket();
    accept();
}

void TicketDialog::onCancel() {
    reject();
}

void TicketDialog::onClassChanged(int index) {
    updateFare();
    updateAvailableSeats();
}

void TicketDialog::updateFare() {
    double baseFare = 0.0;
    switch (static_cast<Ticket::Class>(seatClassCombo->currentData().toInt())) {
        case Ticket::Class::FirstClass:
            baseFare = 1000.0;
            break;
        case Ticket::Class::Business:
            baseFare = 500.0;
            break;
        case Ticket::Class::Economy:
        default:
            baseFare = 200.0;
            break;
    }

    fareLabel->setText(tr("$%1").arg(baseFare, 0, 'f', 2));
}

void TicketDialog::updateAvailableSeats() {
    if (!flight) return;

    seatNumberCombo->clear();
    auto availableSeats = flight->getAvailableSeats();
    for (const auto& seat : availableSeats) {
        seatNumberCombo->addItem(seat);
    }
}

bool TicketDialog::validateInputs() {
    bool valid = true;
    auto setError = [](QLineEdit* edit, bool error) {
        if (error)
            edit->setStyleSheet("border: 2px solid #e74c3c; background: #34495e; color: #ecf0f1;");
        else
            edit->setStyleSheet("");
    };
    setError(passengerNameEdit, passengerNameEdit->text().isEmpty());
    setError(phoneNumberEdit, phoneNumberEdit->text().isEmpty());
    if (passengerNameEdit->text().isEmpty()) {
        QMessageBox::warning(this, tr("Validation Error"),
                           tr("Please enter passenger name."));
        valid = false;
    }
    if (phoneNumberEdit->text().isEmpty()) {
        QMessageBox::warning(this, tr("Validation Error"),
                           tr("Please enter phone number."));
        valid = false;
    }
    if (seatNumberCombo->currentText().isEmpty()) {
        QMessageBox::warning(this, tr("Validation Error"),
                           tr("Please select a seat."));
        valid = false;
    }
    return valid;
}

void TicketDialog::createTicket() {
    QString ticketNumber = QString("TKT%1").arg(QDateTime::currentDateTime()
                                               .toString("yyyyMMddhhmmss"));
    
    auto passenger = std::make_unique<Passenger>(
        passengerNameEdit->text(),
        "",  // Last name will be extracted from full name
        phoneNumberEdit->text(),
        seatNumberCombo->currentText()
    );

    ticket = std::make_unique<Ticket>(
        ticketNumber,
        passenger.get(),
        flight,
        static_cast<Ticket::Class>(seatClassCombo->currentData().toInt())
    );

    if (flight) {
        flight->addPassenger(std::move(passenger));
    }
}

void TicketDialog::showEvent(QShowEvent* event) {
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