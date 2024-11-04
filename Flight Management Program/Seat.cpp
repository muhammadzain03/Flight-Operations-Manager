// FILE: Seat.cpp

#include "Seat.h"

Seat::Seat() : seat(0), seatId(' '), status(false) {}

Seat::Seat(int seat, char seatId) : seat(seat), seatId(seatId), status(false) {}

// Getter implementations:
int Seat::get_seat() const {
    return seat;
}

char Seat::get_seatId() const {
    return seatId;
}

bool Seat::get_status() const {
    return status;
}

// Setter implementation: Allows external code to set the status of seats
void Seat::set_status(bool stat) {
    status = stat;
}