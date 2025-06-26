// This file contains the implementation for the `Ticket` class,
// which represents a flight ticket in the Flight Operations Manager application.
// It stores details such as the associated flight, passenger, seat number,
// and ticket class, and provides methods for managing ticket information.

#include "models/Ticket.h"
#include "models/Passenger.h"
#include "models/Flight.h"
#include <QDateTime>

Ticket::Ticket(const QString& ticketNumber,
               Passenger* passenger,
               Flight* flight,
               Class ticketClass)
    : ticketNumber(ticketNumber)
    , passenger(passenger)
    , flight(flight)
    , status(Status::Reserved)
    , ticketClass(ticketClass)
    , fare(0.0)
    , bookingTime(QDateTime::currentDateTime())
    , baggage(0)
{
    // Calculate fare based on ticket class
    switch (ticketClass) {
        case Class::FirstClass:
            fare = 1000.0;  // Base fare for first class
            break;
        case Class::Business:
            fare = 500.0;   // Base fare for business class
            break;
        case Class::Economy:
        default:
            fare = 200.0;   // Base fare for economy class
            break;
    }
}

bool Ticket::checkIn() {
    if (status != Status::Confirmed) {
        return false;
    }
    status = Status::CheckedIn;
    return true;
}

bool Ticket::cancel() {
    if (status == Status::CheckedIn) {
        return false;  // Cannot cancel after check-in
    }
    status = Status::Cancelled;
    return true;
}

bool Ticket::upgrade(Class newClass) {
    if (newClass <= ticketClass) {
        return false;  // Can only upgrade to a higher class
    }

    double oldFare = fare;
    switch (newClass) {
        case Class::FirstClass:
            fare = 1000.0;
            break;
        case Class::Business:
            fare = 500.0;
            break;
        default:
            return false;
    }

    ticketClass = newClass;
    return true;
} 