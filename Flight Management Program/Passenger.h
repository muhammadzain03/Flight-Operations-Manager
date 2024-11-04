// FILE: Passenger.h (Represent individual passengers with their information)

#ifndef PASSENGER_H
#define PASSENGER_H

#include <iostream>
#include <string>
using namespace std;

class Seat;  // Forward declaration -  to avoid circular dependencies.

class Passenger {
private:
    string firstName;
    string lastName;
    string phoneNumber;
    string seat_number;
    string passenger_id;
    Seat* assignedSeat;  // The Passenger objects can hold information about individual passengers and their corresponding seats (via the assignedSeat pointer).

public:
    Passenger();          // Default Constructor
    Passenger(string fname, string lname, string phonenum, string seat, string p_id);

    // Getters:
    string get_FirstName() const;       // Returns first_name
    string get_LastName() const;        // Returns last_name
    string get_phonenum() const;       // Returns phone_number
    string get_passenger_id() const;      // Returns the passenger ID
    string get_seatname() const;                 // Returns Passenger_id
    Seat* get_assignedSeat() const;  // Getter for assignedSeat

    void set_assignedSeat(Seat* a);    // Setter function - sets the assigned seat to the passenger	
};

#endif // PASSENGER_H


// This code provides a blueprint for managing passenger details and their assigned seats within a larger system. 
