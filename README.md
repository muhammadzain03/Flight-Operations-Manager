# ✈️ Flight Manager

The Flight Management Program is a comprehensive C++ application for managing flights, passengers, seats, and airline information. Designed to efficiently organize essential airline data, this program provides a streamlined approach to handling various airline and passenger management operations.

## 📂 Project Structure

The main files in this project include:

- **Airline.h / Airline.cpp**: Defines the Airline class, handling airline-specific operations and attributes.
- **Flight.h / Flight.cpp**: Manages individual flight details and operations, including flight numbers, destinations, and schedules.
- **Passenger.h / Passenger.cpp**: Manages passenger details, including name, ID, and assigned flight.
- **Seat.h / Seat.cpp**: Manages seat arrangements within a flight, including availability and seat assignments.
- **Main.cpp**: The main driver file to execute the program, integrating all classes to perform airline operations.
- **flight_info.txt**: A data file with sample flight details, used to initialize the program with predefined data.

## 🚀 Getting Started

To run the project locally:

1. **Clone or Download** this repository.
2. **Compile the Code**: Use a C++ compiler, such as `g++`, to compile the code. Run the following command:

   ```bash
   g++ Main.cpp Flight.cpp Passenger.cpp Seat.cpp Airline.cpp -o FlightManagement
   ./FlightManagement
   ```

## ✨ Features

- **Airline Management**: Supports managing multiple airlines, including details such as fleet information and operational status.
- **Flight Management**: Manages flights for each airline, including schedules, routes, and availability.
- **Passenger Records**: Handles passenger information for flights, allowing you to add, update, and view passenger details.
- **Seat Allocation**: Efficiently assigns and manages seats for passengers on each flight.

## 📄 License

This project is licensed under the MIT License. See the LICENSE file for more details.

---

Feel free to explore, modify, and expand this program to suit your needs. Contributions are welcome!
