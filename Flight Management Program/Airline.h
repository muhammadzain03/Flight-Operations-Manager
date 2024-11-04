// FILE: Airline.h (organize and manage the flights associated with the airline)

#ifndef AIRLINE_H
#define AIRLINE_H

#include <iostream>
#include <vector>
#include "Flight.h" // Include Flight.h for Flight class

class Airline {
private:
    std::vector<Flight> flights;   // A vector to store instances of the 'Flight' class, representing the flights associated with the airline.
    std::string airlineName;        // String for Airline name

public:
    Airline(const std::string& name);                     // Constructor
    const std::vector<Flight>& get_flights() const;       // Getter function that returns a reference to the vector of flights.
    void addFlight(const Flight& newFlight);              // Method to add a new flight to the vector of flights.
    void displayFlightInfo() const;                       // Method to display information about all flights: seat maps and passenger information.
};

#endif // AIRLINE_H
