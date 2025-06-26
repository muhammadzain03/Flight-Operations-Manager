// FILE: Passenger.cpp

// This file contains the implementation for the `Passenger` class,
// which manages passenger data, including personal information and seat assignments,
// within the Flight Operations Manager application. It provides methods to create,
// retrieve, and update passenger records.

#include "../include/models/Passenger.h"
#include "../include/models/Seat.h"

Passenger::Passenger() : assignedSeat(nullptr) {}

Passenger::Passenger(const QString& firstName,
                    const QString& lastName,
                    const QString& phoneNumber,
                    const QString& email,
                    const QString& seatNumber)
    : firstName(firstName)
    , lastName(lastName)
    , phoneNumber(phoneNumber)
    , email(email)
    , seatNumber(seatNumber)
    , assignedSeat(nullptr)
{}

Passenger::~Passenger() {
    assignedSeat = nullptr;
}

Passenger::Passenger(const Passenger& other)
    : firstName(other.firstName)
    , lastName(other.lastName)
    , phoneNumber(other.phoneNumber)
    , email(other.email)
    , seatNumber(other.seatNumber)
    , assignedSeat(other.assignedSeat)
{}

Passenger& Passenger::operator=(const Passenger& other) {
    if (this != &other) {
        firstName = other.firstName;
        lastName = other.lastName;
        phoneNumber = other.phoneNumber;
        email = other.email;
        seatNumber = other.seatNumber;
        assignedSeat = other.assignedSeat;
    }
    return *this;
}

bool Passenger::setAssignedSeat(Seat* seat) {
    if (!seat) {
        assignedSeat = nullptr;
        return false;
    }
    assignedSeat = seat;
    return true;
}

bool Passenger::assignSeat(const std::string& seatNumber) {
    this->seatNumber = QString::fromStdString(seatNumber);
    return true;
}

void Passenger::unassignSeat() {
    seatNumber.clear();
    assignedSeat = nullptr;
}