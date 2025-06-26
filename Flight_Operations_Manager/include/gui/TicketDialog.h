// This header defines the `TicketDialog` class, which provides a dialog
// for booking and managing flight tickets within the Flight Operations Manager application.

#ifndef TICKETDIALOG_H
#define TICKETDIALOG_H

#include <QDialog>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include <QVBoxLayout>
#include "models/Ticket.h"
#include "models/Flight.h"
#include "models/Passenger.h"

class TicketDialog : public QDialog {
    Q_OBJECT

public:
    explicit TicketDialog(Flight* flight = nullptr, QWidget* parent = nullptr);
    void setFlight(Flight* flight);
    Ticket* getTicket() const { return ticket.get(); }

private slots:
    void onConfirm();
    void onCancel();
    void onClassChanged(int index);
    void updateFare();

// ... existing content above remains unchanged ...

protected:
    void showEvent(QShowEvent* event) override;

private:
    void setupUi();
    void createTicket();
    bool validateInputs();
    void updateAvailableSeats();  // ✅ Added

    std::unique_ptr<Ticket> ticket;
    Flight* flight;

    QLineEdit* passengerNameEdit;
    QLineEdit* phoneNumberEdit;
    QComboBox* seatClassCombo;
    QComboBox* seatNumberCombo;
    QLabel* fareLabel;
    QPushButton* confirmButton;
    QPushButton* cancelButton;
};

#endif // TICKETDIALOG_H