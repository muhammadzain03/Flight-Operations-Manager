// This file contains the implementation for the `Airline` class,
// which represents an airline entity in the Flight Operations Manager application.
// It includes methods for managing airline properties like name, IATA code, and contact.

#include "models/Airline.h"
#include <algorithm>

Airline::Airline(const QString& name) : name(name) {}

Flight* Airline::getFlight(const QString& id) const {
    auto it = std::find_if(flights.begin(), flights.end(),
                           [&](const auto& flight) {
                               return flight->getFlightNumber() == id;
                           });
    return it != flights.end() ? it->get() : nullptr;
}

bool Airline::addFlight(std::unique_ptr<Flight> flight) {
    if (!flight) return false;
    if (flight->getFlightNumber().isEmpty()) return false;
    if (getFlight(flight->getFlightNumber())) return false;
    flights.push_back(std::move(flight));
    return true;
}

void Airline::removeFlight(const QString& id) {
    if (id.isEmpty()) return;
    flights.erase(
        std::remove_if(flights.begin(), flights.end(),
                       [&](const auto& flight) {
                           return flight && flight->getFlightNumber() == id;
                       }),
        flights.end());
}

bool Airline::updateFlight(const QString& flightNumber, std::unique_ptr<Flight> newFlight) {
    if (!newFlight || flightNumber.isEmpty() || newFlight->getFlightNumber().isEmpty()) return false;
    auto it = std::find_if(flights.begin(), flights.end(),
                           [&](const auto& f) {
                               return f && f->getFlightNumber() == flightNumber;
                           });
    if (it == flights.end()) return false;
    *it = std::move(newFlight);
    return true;
}

void Airline::setFlights(std::vector<std::unique_ptr<Flight>>&& newFlights) {
    flights = std::move(newFlights);
}

std::vector<Flight*> Airline::searchFlights(const QString& query) const {
    std::vector<Flight*> results;
    QString lowercaseQuery = query.toLower();

    for (const auto& flight : flights) {
        if (flight->getFlightNumber().toLower().contains(lowercaseQuery) ||
            flight->getOrigin().toLower().contains(lowercaseQuery) ||
            flight->getDestination().toLower().contains(lowercaseQuery)) {
            results.push_back(flight.get());
        }
    }

    return results;
}

std::vector<Passenger*> Airline::searchPassengers(const QString& query) const {
    std::vector<Passenger*> results;
    QString lowercaseQuery = query.toLower();

    for (const auto& flight : flights) {
        for (const auto& passenger : flight->getAllPassengers()) {
            if (passenger->getFirstName().toLower().contains(lowercaseQuery) ||
                passenger->getLastName().toLower().contains(lowercaseQuery) ||
                passenger->getPhoneNumber().contains(query)) {
                results.push_back(passenger);
            }
        }
    }

    return results;
}

bool Airline::addPassenger(std::unique_ptr<Passenger> passenger, const QString& flightNumber) {
    if (auto flight = getFlight(flightNumber)) {
        if (passenger) {
            return flight->addPassenger(std::move(passenger));
        }
    }
    return false;
}

bool Airline::removePassenger(const QString& passengerId, const QString& flightNumber) {
    if (auto flight = getFlight(flightNumber)) {
        return flight->removePassenger(passengerId);
    }
    return false;
}

Passenger* Airline::getPassenger(const QString& passengerId, const QString& flightNumber) const {
    if (auto flight = getFlight(flightNumber)) {
        return flight->getPassenger(passengerId);
    }
    return nullptr;
}

std::vector<Passenger*> Airline::getAllPassengers() const {
    std::vector<Passenger*> allPassengers;
    for (const auto& flight : flights) {
        auto passengers = flight->getAllPassengers();
        allPassengers.insert(allPassengers.end(), passengers.begin(), passengers.end());
    }
    return allPassengers;
}

bool Airline::bookSeat(const QString& flightNumber, const QString& passengerId, const QString& seatNumber) {
    if (auto flight = getFlight(flightNumber)) {
        if (auto passenger = flight->getPassenger(passengerId)) {
            return flight->assignSeat(passenger, seatNumber);
        }
    }
    return false;
}

bool Airline::cancelBooking(const QString& flightNumber, const QString& passengerId) {
    if (auto flight = getFlight(flightNumber)) {
        if (auto passenger = flight->getPassenger(passengerId)) {
            passenger->unassignSeat();
            return true;
        }
    }
    return false;
}

bool Airline::changeBooking(const QString& flightNumber, const QString& passengerId, const QString& newSeatNumber) {
    if (auto flight = getFlight(flightNumber)) {
        if (auto passenger = flight->getPassenger(passengerId)) {
            QString oldSeat = passenger->getSeatNumber();
            passenger->unassignSeat();
            if (!flight->assignSeat(passenger, newSeatNumber)) {
                flight->assignSeat(passenger, oldSeat); // restore
                return false;
            }
            return true;
        }
    }
    return false;
}

std::vector<QString> Airline::getAvailableSeats(const QString& flightNumber) const {
    if (auto flight = getFlight(flightNumber)) {
        return flight->getAvailableSeats();
    }
    return std::vector<QString>();
}

void Airline::displayFlightInfo() const {
    for (const auto& flight : flights) {
        // qDebug() << flight->getFlightNumber();
    }
}

// Seat validation method (multi-letter row support)
bool Airline::isSeatValid(const QString& flightId, const QString& seatNumber) const {
    Flight* flight = getFlight(flightId);
    if (!flight) return false;

    // Extract row label (letters) and column number
    QString rowLabel;
    int i = 0;
    while (i < seatNumber.length() && seatNumber[i].isLetter()) {
        rowLabel += seatNumber[i];
        ++i;
    }

    int col = QStringView(seatNumber).mid(i).toInt() - 1;

    // Convert row label to numeric index (e.g., A=0, Z=25, AA=26, etc.)
    int row = 0;
    for (int j = 0; j < rowLabel.length(); ++j) {
        QChar ch = rowLabel[j];
        row = row * 26 + (ch.unicode() - 'A' + 1);
    }
    row -= 1;

    return row >= 0 && row < flight->getRows() &&
           col >= 0 && col < flight->getCols();
}

const std::vector<std::unique_ptr<Flight>>& Airline::getFlights() const {
    return flights;
}

