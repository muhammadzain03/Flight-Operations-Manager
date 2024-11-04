// FILE: Main.cpp

#include <iostream>
#include <fstream>
#include <limits>
#include <string>
#include <cassert>
#include <cctype>
#include <vector>
#include <iterator>
#include <cstdlib>  // Use cstdlib instead of stdlib.h - C++ Standard Library version of the C Standard Library <stdlib.h> header.

#include "Passenger.h"
#include "Seat.h"
#include "Flight.h"
#include "Airline.h"

using namespace std;

// Function prototypes
int menu();
Flight populate_flight(const char* file);
void display_passengers(const Flight* flight);
void press_enter();

int main() {
    // Display program information
    cout << "Version: 1.0" << endl;
    cout << "Term Project - Flight Management Program in C++" << endl;
    cout << "Produced by: Muhammad Zain AND Arsal Zia" << endl;
    cout << "Year: 2023" << endl << endl;

    cout << "<<< Press Return To Continue >>>>" << endl;
    cout << "<<< Press Return To Continue >>>>" << endl;

    // Populate Flight object with data from file
    Flight f = populate_flight("flight_info.txt");

    int choice = 1;

    // Display menu and process user input
    while (choice != 0) {
        switch (menu()) {
            case 1:
                f.Display_SeatMap();
                press_enter();
                break;

            case 2:
                f.Display_Passenger_Info();
                press_enter();
                break;

            case 3: {
                f.Add_Passenger();
                press_enter();
                break;
            }

            case 4: {
                int passengerID;
                cout << "Enter passenger ID to remove: ";
                cin >> passengerID;  // Initialize passengerID with user input
                f.Remove_Passenger(passengerID);
                press_enter();
                break;
            }

            case 5:
                f.saveFile();
				cout << "\n[Data saved]\n";
                break;

            case 6:
                choice = 0;
                break;
        }
    }
    return 0;
}

// Function to display menu and get user choice
int menu() {
    int choice = -1;

    // Display menu options and validate user input
    cout << "Please select one of the following options:\n" << endl;
    cout << "1. Display flight Seat Map.\n" << endl;
    cout << "2. Display Passenger's Information.\n" << endl;
    cout << "3. Add Passenger\n" << endl;
    cout << "4. Display Passengers\n" << endl;
    cout << "5. Save File\n" << endl;
    cout << "6. Exit\n" << endl;

    while (!(cin >> choice) || choice < 0 || choice > 6) {
        cout << "Invalid input. Please enter a number between 0 and 6." << endl;
        cin.clear();  // Clear the error flag
        cin.ignore(numeric_limits<streamsize>::max(), '\n');  // Discard invalid input
    }

    return choice;
}

// Function to populate Flight object with data from a file
Flight populate_flight(const char* file) {
    ifstream in_stream(file);  

    if (in_stream.fail()) {
        cout << "Sorry, cannot open the file." << endl;
        exit(1);
    }

    // Read flight information from the file
    string flight_name;
    in_stream >> flight_name;

    int row, col;
    in_stream >> row >> col;

    // Create an Airline object and initialize Flight object A
    Airline a_line(flight_name);
    Flight A = a_line.get_flights()[0]; // Assuming get_flights() returns a vector and we are using the first flight

    int counter = 0;

    // Read passenger information from the file and add them to Flight object A
    while (!in_stream.eof()) {
        string first_name, last_name, phone_num, seat_num, id;
        string record;
        getline(in_stream, record);

        int i = 0;

        // Extract passenger information from each record in the file
        while (isalpha(record[i]) || isalpha(record[i + 1])) {
            first_name += record[i];
            i++;
        }

        // Extract last name
        while (!isalpha(record[i]))
            i++;

        while (isalpha(record[i]) || isalpha(record[i + 1])) {
            last_name += record[i];
            i++;
        }

        // Extract phone number
        while (!isdigit(record[i]))
            i++;

        while (isdigit(record[i]) || record[i] == '-') {
            phone_num += record[i];
            i++;
        }

        // Extract seat number
        while (!isdigit(record[i]))
            i++;

        while (isalnum(record[i])) {
            seat_num += record[i];
            i++;
        }

        // Extract passenger ID
        while (!isdigit(record[i]))
            i++;

        while (record[i] != '\0') {
            id += record[i];
            i++;
        }

        // Add the passenger to Flight object A
        if (counter != 0) {
            A.Set_Passenger_Info(first_name, last_name, phone_num, seat_num, id);
        }
        counter++;
    }

    return A;
}

// Function to display passengers' information from a Flight object
void display_passengers(const Flight* flight) {
    cout << "Passenger Information:\n\n";

    // Display information for each passenger in the flight
    for (const Passenger& passenger : flight->passenger_list) {
        cout << "Passenger ID: " << passenger.get_passenger_id() << endl;
        cout << "Name: " << passenger.get_FirstName() << " " << passenger.get_LastName() << endl;
        cout << "Phone Number: " << passenger.get_phonenum() << endl;
        cout << "Assigned Seat: " << passenger.get_seatname() << endl;
        cout << "--------------------------\n";
    }
}

// Function to wait for user to press Enter
void press_enter() {
    cout << "Press Enter to continue...";
    cin.ignore();
    cin.get();
}
