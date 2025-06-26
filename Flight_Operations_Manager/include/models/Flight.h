// This header defines the `Flight` class, representing a single flight
// within the Flight Operations Manager application. It encapsulates flight details
// such as flight number, origin, destination, departure/arrival times, and manages
// the associated seat map and passenger bookings.

#ifndef FLIGHT_H
#define FLIGHT_H

#include <QString>
#include <QDateTime>
#include <vector>
#include <memory>
#include <map>
#include <stdexcept>
#include "models/Seat.h"
#include "models/Passenger.h"
#include "models/SeatLayout.h"

class Flight {
public:
    Flight();
    Flight(const QString& flightNumber,
           const QString& origin,
           const QString& destination,
           const QDateTime& departureTime,
           double basePrice = 500.0);
    ~Flight() = default;

    // Getters
    QString getFlightNumber() const { return flightNumber; }
    QString getOrigin() const { return origin; }
    QString getDestination() const { return destination; }
    QDateTime getDepartureTime() const { return departureTime; }
    int getRows() const { return rows; }
    int getCols() const { return cols; }
    double getBasePrice() const { return basePrice; }

    // Seat management
    bool assignSeat(Passenger* passenger, const QString& seatNumber);
    bool unassignSeat(const QString& seatNumber);
    bool isSeatAvailable(const QString& seatNumber) const;
    bool isSeatValid(const QString& seatNumber) const;
    bool isSeatOccupied(const QString& seatNumber) const;

    // Booking-related
    bool bookSeat(const std::string& seat, const std::string& passenger);
    bool cancelSeat(const std::string& seat);
    bool isValidSeatNumber(const std::string& seat) const;

    // Seat mapping utilities
    QString seatNumberToString(int row, int col) const;
    std::pair<int, int> seatStringToPosition(const QString& seat) const;
    
    // Seat layout management
    QVector<SeatRow> getSeatLayout() const { return seatLayout; }
    QMap<QString, QString> getPassengerSeats() const;
    void setSeatLayout(const QVector<SeatRow>& layout);
    void generateDefaultLayout();

    std::vector<QString> getAvailableSeats() const;
    int getOccupiedSeats() const;
    void displayPassengerInfo() const;
    std::vector<Passenger*> getPassengers() const;  // ✅ Added

    // Passenger management
    bool addPassenger(std::unique_ptr<Passenger> passenger);
    bool removePassenger(const QString& seatNumber);
    Passenger* getPassenger(const QString& seatNumber) const;
    std::vector<Passenger*> getAllPassengers() const;
    void displaySeatMap() const;
    Seat* getSeat(const QString& seatNumber);
    QString generateSeatMap() const;
    bool reserveSeat(const QString& seatNumber);
    bool cancelReservation(const QString& seatNumber);
    bool blockSeat(const QString& seatNumber);
    bool unblockSeat(const QString& seatNumber);
    double calculateRevenue() const;

    const std::map<QString, std::unique_ptr<Seat>>& getSeats() const { return seats; }

private:
    void initializeSeats();
    QString generateSeatNumber(int row, int col) const;

    QString flightNumber;
    QString origin;
    QString destination;
    QDateTime departureTime;
    int rows;
    int cols;
    double basePrice;

    std::map<QString, std::unique_ptr<Seat>> seats;
    std::vector<std::unique_ptr<Passenger>> passengers;
    QVector<SeatRow> seatLayout;
};

#endif // FLIGHT_H