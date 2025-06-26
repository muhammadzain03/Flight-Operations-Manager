// FILE: Seat.cpp

// This file contains the implementation for the `Seat` class,
// which represents an individual seat on a flight. It manages the seat's status
// (e.g., available, occupied, reserved, blocked) and its association with a passenger,
// providing methods for status changes and passenger assignment.

#include "models/Seat.h"
#include "models/Passenger.h"

using std::string;

Seat::Seat(const QString& number)
    : number(number), status(Status::Available), passenger(nullptr) {}

Seat::Seat(const QString& number, const QString& seatClass, int row, int col)
    : number(number), seatClass(seatClass), row(row), col(col), status(Status::Available), passenger(nullptr) {}

bool Seat::occupy(const std::string& passId) {
    if (isAvailable()) {
        status = Status::Occupied;
        return true;
    }
    return false;
}

void Seat::vacate() {
    clearPassenger();
}

bool Seat::setPassenger(Passenger* newPassenger) {
    if (this->passenger == newPassenger) return true;
    
    if (!isAvailable() && newPassenger != nullptr) {
        return false;  // Seat is not available
    }
    
    this->passenger = newPassenger;
    status = newPassenger ? Status::Occupied : Status::Available;
    return true;
}

void Seat::clearPassenger() {
    if (passenger) {
        passenger->unassignSeat();
    }
    passenger = nullptr;
    status = Status::Available;
}

bool Seat::reserve() {
    if (isAvailable()) {
        status = Status::Reserved;
        return true;
    }
    return false;
}

bool Seat::unreserve() {
    if (isReserved()) {
        status = Status::Available;
        return true;
    }
    return false;
}

bool Seat::block() {
    if (isAvailable() || isReserved()) {
        status = Status::Blocked;
        return true;
    }
    return false;
}

bool Seat::unblock() {
    if (isBlocked()) {
        status = Status::Available;
        return true;
    }
    return false;
}