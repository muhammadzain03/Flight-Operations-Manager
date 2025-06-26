// This header defines the `Airline` class, representing airline-specific data
// and operations within the Flight Operations Manager application. It includes
// properties such as airline name, IATA code, and contact information.

#ifndef AIRLINE_H
#define AIRLINE_H

#include <QString>
#include <vector>
#include <memory>
#include "models/Flight.h"
#include "models/Passenger.h"

class Airline {
public:
    explicit Airline(const QString& name);

    // Flight management
    Flight* getFlight(const QString& id) const;
    bool addFlight(std::unique_ptr<Flight> flight);
    void removeFlight(const QString& id);
    bool updateFlight(const QString& flightNumber, std::unique_ptr<Flight> newFlight);
    void setFlights(std::vector<std::unique_ptr<Flight>>&& newFlights);
    std::vector<Flight*> searchFlights(const QString& query) const;

    // Passenger management
    bool addPassenger(std::unique_ptr<Passenger> passenger, const QString& flightNumber);
    bool removePassenger(const QString& passengerId, const QString& flightNumber);
    Passenger* getPassenger(const QString& passengerId, const QString& flightNumber) const;
    std::vector<Passenger*> getAllPassengers() const;
    std::vector<Passenger*> searchPassengers(const QString& query) const;

    // Booking operations
    bool bookSeat(const QString& flightNumber, const QString& passengerId, const QString& seatNumber);
    bool cancelBooking(const QString& flightNumber, const QString& passengerId);
    bool changeBooking(const QString& flightNumber, const QString& passengerId, const QString& newSeatNumber);

    // Utility
    std::vector<QString> getAvailableSeats(const QString& flightNumber) const;
    void displayFlightInfo() const;

    // NEW: Seat validation logic
    bool isSeatValid(const QString& flightId, const QString& seatNumber) const;
    const std::vector<std::unique_ptr<Flight>>& getFlights() const;

private:
    QString name;
    std::vector<std::unique_ptr<Flight>> flights;
};

#endif // AIRLINE_H
