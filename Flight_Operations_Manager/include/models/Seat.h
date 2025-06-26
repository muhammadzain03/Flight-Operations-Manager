// This header defines the `Seat` class, representing an individual seat
// on a flight within the Flight Operations Manager application. It manages
// seat status (available, occupied, reserved, blocked) and passenger assignments.

// FILE: Seat.h (represent individual seats on the flight)

#ifndef SEAT_H
#define SEAT_H

#include <QString>

class Passenger;  // Forward declaration

class Seat {
public:
    enum class Status {
        Available,
        Occupied,
        Reserved,
        Blocked
    };

    explicit Seat(const QString& number = QString());
    Seat(const QString& number, const QString& seatClass, int row, int col);
    ~Seat() = default;
    
    // Getters
    QString getNumber() const { return number; }
    bool isOccupied() const { return status == Status::Occupied; }
    bool isAvailable() const { return status == Status::Available; }
    bool isReserved() const { return status == Status::Reserved; }
    bool isBlocked() const { return status == Status::Blocked; }
    Status getStatus() const { return status; }
    Passenger* getPassenger() const { return passenger; }
    QString getSeatClass() const { return seatClass; }
    double getPrice() const { return price; }
    
    // Setters
    void setNumber(const QString& number) { this->number = number; }
    void setSeatClass(const QString& seatClass) { this->seatClass = seatClass; }
    void setPrice(double price) { this->price = price; }
    bool setPassenger(Passenger* passenger);
    void clearPassenger();
    
    // Status management
    bool reserve();
    bool unreserve();
    bool block();
    bool unblock();

    // Legacy methods for compatibility
    bool occupy(const std::string& name);
    void vacate();

private:
    QString number;
    QString seatClass;
    double price{0.0};
    int row = 0;
    int col = 0;
    Status status{Status::Available};
    Passenger* passenger{nullptr};
};

#endif // SEAT_H