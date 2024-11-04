// FILE: Flight.cpp

#include "Flight.h"
#include <algorithm>
#include <iostream>
#include <cassert>
#include <fstream> 

// Constructor for Flight class
Flight::Flight(std::string name, int rows, int cols) {
    id = name;
    num_rows = rows;
    num_cols = cols;
    seatmap = std::vector<std::vector<Seat>>(rows, std::vector<Seat>(cols));
}

// Function to set passenger information for a new passenger on the flight
void Flight::Set_Passenger_Info(const std::string& fname, const std::string& lname, const std::string& phonenum, const std::string& seat, const std::string& id) {
    // Create a new Passenger object and assign it to a seat on the seat map
    Passenger temp(fname, lname, phonenum, seat, id);

    // Check if the seat is already occupied by another passenger
    size_t count = 0;
    while (count < passenger_list.size()) {
        assert(passenger_list[count].get_seatname() != seat);
        count++;
    }

    // Extract row number and column character from the seat string
    std::string::size_type i = 0;
    int row_num = 0;
    char col_char;
    while (i < seat.size()) {
        if (isdigit(seat[i])) {
            row_num = 10 * row_num + seat[i] - '0';
        }
        if (isalpha(seat[i])) {
            col_char = seat[i];
        }
        i++;
    }

    // Convert column character to column index (A -> 0, B -> 1, ...)
    int col_count = 0;
    char seat_name = 'A';
    while (1) {
        if (seat_name == col_char)
            break;
        seat_name++;
        col_count++;
    }

    // Assign the seat to the passenger
    if (row_num >= 10) {
        temp.set_assignedSeat(&(seatmap[row_num - 1][col_count]));
    } else {
        temp.set_assignedSeat(&(seatmap[row_num][col_count]));
    }

    // Add the passenger to the passenger list
    passenger_list.push_back(temp);
}

// Function to display information about all passengers on the flight
void Flight::Display_Passenger_Info() const {
    // Display passenger information (first name, last name, phone number, ID, and seat)
    std::vector<Passenger>::size_type r = 0;
    std::cout.width(20);
    std::cout << std::left << "First Name";
    std::cout.width(20);
    std::cout << std::left << "Last Name";
    std::cout.width(20);
    std::cout << std::left << "Phone Number";
    std::cout.width(20);
    std::cout << std::left << "ID";
    std::cout.width(20);
    std::cout << std::left << "Seat";
    std::cout << std::endl;
    std::cout << "**************************************************************************************\n";
    while (r < passenger_list.size()) {
        std::cout.width(20);
        std::cout << std::left << passenger_list[r].get_FirstName();
        std::cout.width(20);
        std::cout << std::left << passenger_list[r].get_LastName();
        std::cout.width(20);
        std::cout << std::left << passenger_list[r].get_phonenum();
        std::cout.width(20);
        std::cout << std::left << passenger_list[r].get_passenger_id();
        std::cout.width(20);
        std::cout << std::left << passenger_list[r].get_seatname() << std::endl;

        std::cout << std::endl;
        r++;
    }
}

// Function to display the seat map of the flight showing seat occupancy status
void Flight::Display_SeatMap() const {
    // Display the seat map indicating occupied (X) or unoccupied ([ ]) status of each seat
    std::cout << "Flight Seat Map:\n";
    std::cout << " ";
    for (int j = 0; j < num_cols; j++) {
        char seat = j + 'A';
        std::cout.width(6);
        std::cout << seat;
    }
    std::cout << std::endl;

    for (int i = 0; i < num_rows; i++) {
        std::cout.width(5);
        std::cout << std::left << i + 1;
        for (int j = 0; j < num_cols; j++) {
            bool status = seatmap[i][j].get_status();
            if (status == 0) {
                std::cout.width(6);
                std::cout << std::left << "[ ]";
            } else {
                std::cout.width(6);
                std::cout << std::left << "[X]";
            }
        }
        std::cout << std::endl;
    }
}

// Function to add a new passenger to the flight by prompting user input
void Flight::Add_Passenger() {
    // Prompt user to enter passenger information
    std::string firstName;
    std::string lastName;
    std::string phoneNumber;
    std::string seat_number;
    std::string passenger_id;

    std::cout << "Enter the passenger id: ";
    std::cin >> passenger_id;
    std::cout << "\n";

    std::cout << "Enter passenger first name: ";
    std::cin >> firstName;
    std::cout << "\n";

    std::cout << "Enter passenger last name: ";
    std::cin >> lastName;
    std::cout << "\n";

    std::cout << "Enter passenger phone number: ";
    std::cin >> phoneNumber;
    std::cout << "\n";

    std::cout << "Enter passenger's desired seat: ";
    std::cin >> seat_number;
    std::cout << "\n";

    // Set the passenger information using Set_Passenger_Info()
    Set_Passenger_Info(firstName, lastName, phoneNumber, seat_number, id);
}

// Function to remove a passenger from the flight based on their ID
void Flight::Remove_Passenger(int passengerID) {
    auto it = std::find_if(passenger_list.begin(), passenger_list.end(), [passengerID](const Passenger &p) {
        return p.get_passenger_id() == std::to_string(passengerID);
    });

    if (it != passenger_list.end()) {
        Seat* seat = it->get_assignedSeat(); // Use get_assignedSeat() to retrieve the Seat pointer
        seat->set_status(false);
        it->set_assignedSeat(nullptr); // Set assigned seat to nullptr
        passenger_list.erase(it);
        std::cout << "Passenger removed successfully!\n";
    } else {
        std::cout << "Passenger with ID " << passengerID << " not found.\n";
    }
}

// Function to save flight information (ID, rows, columns, passenger details) to a file
void Flight::saveFile() {
    std::ofstream out_stream("flight_info.txt");
    assert(!out_stream.fail());

    // Write flight information to the file
    out_stream.width(10);
    out_stream << std::left << id;
    out_stream.width(5);
    out_stream << std::left << num_rows;
    out_stream.width(5);
    out_stream << std::left << num_cols << std::endl;

    long unsigned int r = 0;
    while (r < passenger_list.size()) {
        // Write passenger details to the file
        out_stream.width(20);
        out_stream << std::left << passenger_list[r].get_FirstName();
        out_stream.width(20);
        out_stream << std::left << passenger_list[r].get_LastName();
        out_stream.width(20);
        out_stream << std::left << passenger_list[r].get_phonenum();
        out_stream.width(10);
        out_stream << std::left << passenger_list[r].get_seatname();
        out_stream.width(10);
        out_stream << std::left << passenger_list[r].get_passenger_id();

        r++;
        if (r != passenger_list.size()) {
            out_stream << std::endl;
        }
    }

    out_stream.close();
}
