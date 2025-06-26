// FILE: Passenger.h (Represent individual passengers with their information)

// This header defines the `Passenger` class, which represents passenger information
// within the Flight Operations Manager application. It includes details such as
// name, contact information, and seat assignment, and provides methods for managing
// passenger data.

#ifndef PASSENGER_H
#define PASSENGER_H

#include <QString>

class Seat;  // Forward declaration -  to avoid circular dependencies.

class Passenger {
public:
    // Constructors/Destructor
    Passenger();
    Passenger(const QString& firstName, const QString& lastName, 
             const QString& phoneNumber, const QString& email = QString(),
             const QString& seatNumber = QString());
    ~Passenger();
    
    // Copy constructor and assignment operator
    Passenger(const Passenger& other);
    Passenger& operator=(const Passenger& other);

    // Getters
    QString getFirstName() const { return firstName; }
    QString getLastName() const { return lastName; }
    QString getFullName() const { return firstName + " " + lastName; }
    QString getPhoneNumber() const { return phoneNumber; }
    QString getEmail() const { return email; }
    QString getSeatNumber() const { return seatNumber; }
    Seat* getAssignedSeat() const { return assignedSeat; }

    // Setters
    void setFirstName(const QString& name) { firstName = name; }
    void setLastName(const QString& name) { lastName = name; }
    void setPhoneNumber(const QString& number) { phoneNumber = number; }
    void setEmail(const QString& email) { this->email = email; }
    void setSeatNumber(const QString& number) { seatNumber = number; }
    bool setAssignedSeat(Seat* seat);

    // Seat operations
    bool assignSeat(const std::string& seatNumber);
    void unassignSeat();
    bool isSeated() const { return !seatNumber.isEmpty(); }

    // Utility function to check if seat is assigned
    bool hasSeat() const { return !seatNumber.isEmpty(); }

private:
    QString firstName;
    QString lastName;
    QString phoneNumber;
    QString email;
    QString seatNumber;
    Seat* assignedSeat;  // The Passenger objects can hold information about individual passengers and their corresponding seats
};

#endif // PASSENGER_H


// This code provides a blueprint for managing passenger details and their assigned seats within a larger system. 
