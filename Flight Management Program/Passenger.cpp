// FILE: Passenger.cpp

#include "Passenger.h"

Passenger::Passenger() {
    firstName = "";
    lastName = "";
    phoneNumber = "";
    seat_number = "";
    passenger_id = "";
    assignedSeat = nullptr; // Initialize the pointer to nullptr
}

Passenger::Passenger(string fname, string lname, string phonenum, string seat, string p_id) {
    firstName = fname;
    lastName = lname;
    phoneNumber = phonenum;
    seat_number = seat;
    passenger_id = p_id;
    assignedSeat = nullptr; // Initialize the pointer to nullptr
}

// Getter functions
string Passenger::get_FirstName() const {
    return firstName;
}

string Passenger::get_LastName() const {
    return lastName;
}

string Passenger::get_phonenum() const {
    return phoneNumber;
}

string Passenger::get_passenger_id() const {
    return passenger_id;
}

string Passenger::get_seatname() const {
    return seat_number;
}

Seat* Passenger::get_assignedSeat() const {
    return assignedSeat;
}

void Passenger::set_assignedSeat(Seat* a) {
    assignedSeat = a;
}