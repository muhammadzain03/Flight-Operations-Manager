// FILE: Airline.cpp

#include "Airline.h"
#include <iostream>

Airline::Airline(const std::string& name) {    // Constructor implementation
    airlineName = name;
}

const std::vector<Flight>& Airline::get_flights() const {  // Implementation of the getter function
    return flights;
}

void Airline::addFlight(const Flight& newFlight) {    //  Implementation of the method to add a new flight
    flights.push_back(newFlight);
}

void Airline::displayFlightInfo() const {
    for (const auto& flight : flights) {
        std::cout << "Flight Information:\n";
        flight.Display_SeatMap();
        flight.Display_Passenger_Info();
    }
}
