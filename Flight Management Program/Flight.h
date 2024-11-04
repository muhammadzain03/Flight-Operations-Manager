// FILE: Flight.h (Manage the list of passengers and seats)

#ifndef FLIGHT_H
#define FLIGHT_H

#include <vector>
#include <string>
#include "Passenger.h"  // Include Passenger.h for Passenger class
#include "Seat.h"       // Include Seat.h for Seat class

class Flight {
private:
    std::vector<std::vector<Seat>> seatmap;  // 2D vector to represent the seat map of the flight

public:
    std::string id;  // ID of the flight
    int num_rows;    // Number of rows in the seat map
    int num_cols;    // Number of columns in the seat map
    std::vector<Passenger> passenger_list;  // Vector to store Passenger objects
	
    // Constructor to initialize the Flight object with a given name, number of rows, and columns
    Flight(std::string name, int rows, int cols);
    
    // Function to set passenger information for a new passenger on the flight
    void Set_Passenger_Info(const std::string& fname, const std::string& lname, const std::string& phonenum, const std::string& seat, const std::string& id);
    
    // Function to display information about all passengers on the flight
    void Display_Passenger_Info() const;
    
    // Function to display the seat map of the flight showing seat occupancy status
    void Display_SeatMap() const;
    
    // Function to add a new passenger to the flight by prompting user input
    void Add_Passenger();
    
    // Function to remove a passenger from the flight based on their ID
    void Remove_Passenger(int passengerID);
	
    // Function to save flight information (ID, rows, columns, passenger details) to a file
	void saveFile();
};

#endif // FLIGHT_H
