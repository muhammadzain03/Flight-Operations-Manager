// FILE: Seat.h (represent individual seats on the flight)

#ifndef SEAT_H
#define SEAT_H

class Seat {
private:
    int seat;
    char seatId;
    bool status;

public:
    Seat();                       // Default Constructor
    Seat(int seat, char seatId);  // Parameterized Constructor

    int get_seat() const;          // Public getters -    
    char get_seatId() const;       // - retrieve the value of private data members
    bool get_status() const;       //

    void set_status(bool stat);     // Public setter - sets the status of the seats
};

#endif