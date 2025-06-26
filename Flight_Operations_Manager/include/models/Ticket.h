// This header defines the `Ticket` class, which represents a flight ticket
// within the Flight Operations Manager application. It includes details such as
// the associated flight, passenger, seat number, and ticket class, and provides
// methods for managing ticket information.

#ifndef TICKET_H
#define TICKET_H

#include <QString>
#include <QDateTime>

class Passenger;
class Flight;

class Ticket {
public:
    enum class Status {
        Reserved,
        Confirmed,
        Cancelled,
        CheckedIn
    };

    enum class Class {
        Economy,
        Business,
        FirstClass
    };

    Ticket(const QString& ticketNumber,
           Passenger* passenger,
           Flight* flight,
           Class ticketClass = Class::Economy);

    // Getters
    QString getTicketNumber() const { return ticketNumber; }
    Passenger* getPassenger() const { return passenger; }
    Flight* getFlight() const { return flight; }
    Status getStatus() const { return status; }
    Class getTicketClass() const { return ticketClass; }
    double getFare() const { return fare; }
    QDateTime getBookingTime() const { return bookingTime; }
    bool isCheckedIn() const { return status == Status::CheckedIn; }

    // Setters and operations
    void setStatus(Status newStatus) { status = newStatus; }
    void setFare(double newFare) { fare = newFare; }
    bool checkIn();
    bool cancel();
    bool upgrade(Class newClass);

private:
    QString ticketNumber;
    Passenger* passenger;
    Flight* flight;
    Status status;
    Class ticketClass;
    double fare;
    QDateTime bookingTime;
    int baggage;  // Number of checked bags
};

#endif // TICKET_H 