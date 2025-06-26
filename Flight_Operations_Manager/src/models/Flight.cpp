// This file contains the implementation for the `Flight` class,
// which models a flight in the Flight Operations Manager application. It manages
// flight details like route, schedule, and most importantly, the seat map
// with functionalities to get seat statuses, book, cancel, block, and unblock seats.

#include "models/Flight.h"
#include <map>
#include <algorithm>    // for std::count_if, std::find_if
#include <stdexcept>    // for exception safety
#include <QDebug>       // for qDebug()
#include <QStringList>
#include <QMap>
#include <QRandomGenerator>

// -----------------------------------------------------------------------------
// Boeing 777-300ER seat-map layout configuration
// -----------------------------------------------------------------------------
namespace {
    // Total rows and max columns in the aircraft
    constexpr int kRows = 64;
    constexpr int kCols = 10;

    // Custom seat-letter mapping for rows with non-standard layouts:
    //   First class (1–7):         A D G L
    //   Business / Premium (8–18): A B D E F G J L
    //   Economy (19–64):           A B C D E F G H J L
    const std::map<int, QStringList> kRowSeatLetters = {
        {1, {"A","D","G","L"}},   {2, {"A","D","G","L"}},
        {3, {"A","D","G","L"}},   {4, {"A","D","G","L"}},
        {5, {"A","D","G","L"}},   {6, {"A","D","G","L"}},
        {7, {"A","D","G","L"}},
        {8, {"A","B","D","E","F","G","J","L"}}, {9, {"A","B","D","E","F","G","J","L"}},
        {10,{"A","B","D","E","F","G","J","L"}}, {11,{"A","B","D","E","F","G","J","L"}},
        {12,{"A","B","D","E","F","G","J","L"}}, {13,{"A","B","D","E","F","G","J","L"}},
        {14,{"A","B","D","E","F","G","J","L"}}, {15,{"A","B","D","E","F","G","J","L"}},
        {16,{"A","B","D","E","F","G","J","L"}}, {17,{"A","B","D","E","F","G","J","L"}},
        {18,{"A","B","D","E","F","G","J","L"}}
    };

    /** Return the seat-letters list for a given 1-based row. */
    QStringList getSeatLettersForRow(int row) {
        auto it = kRowSeatLetters.find(row);
        if (it != kRowSeatLetters.end())
            return it->second;
        // Economy default
        return {"A","B","C","D","E","F","G","H","J","L"};
    }
}

// -----------------------------------------------------------------------------
// Flight Implementation
// -----------------------------------------------------------------------------

Flight::Flight(const QString& flightNumber,
               const QString& origin,
               const QString& destination,
               const QDateTime& departureTime,
               double basePrice)
    : flightNumber(flightNumber)
    , origin(origin)
    , destination(destination)
    , departureTime(departureTime)
    , rows(kRows)
    , cols(kCols)
    , basePrice(basePrice)
{
    initializeSeats();
}

void Flight::initializeSeats() {
    seats.clear();
    QRandomGenerator* gen = QRandomGenerator::global();

    for (int r = 1; r <= kRows; ++r) {
        QStringList letters = getSeatLettersForRow(r);
        QString seatClass;
        double price;
        
        // Determine seat class and price based on row and basePrice
        if (r <= 7) {
            seatClass = "First";
            // e.g., for a 500 base: 1500 + (0 to 499)
            price = (basePrice * 3.0) + gen->bounded(static_cast<int>(basePrice)); 
        } else if (r <= 11) {
            seatClass = "Business";
            // e.g., for a 500 base: 1000 + (0 to 249)
            price = (basePrice * 2.0) + gen->bounded(static_cast<int>(basePrice / 2.0));
        } else if (r <= 18) {
            seatClass = "Premium";
            // e.g., for a 500 base: 750 + (0 to 199)
            price = (basePrice * 1.5) + gen->bounded(static_cast<int>(basePrice / 2.5));
        } else {
            seatClass = "Economy";
            // e.g., for a 500 base: 500 + (0 to 99)
            price = basePrice + gen->bounded(static_cast<int>(basePrice / 5.0));
        }
        
        for (const QString& letter : letters) {
            QString sn = QString::number(r) + letter;
            auto seat = std::make_unique<Seat>(sn, seatClass, r, letters.indexOf(letter));
            seat->setPrice(price);
            // The seat class is already set in the constructor, but this is fine.
            seat->setSeatClass(seatClass); 
            seats[sn] = std::move(seat);
        }
    }
    rows = kRows;
    cols = kCols;
}

QString Flight::generateSeatNumber(int row, int col) const {
    // row/col are zero-based
    QStringList letters = getSeatLettersForRow(row+1);
    if (col < letters.size())
        return QString::number(row+1) + letters[col];
    return {};
}

// -----------------------------------------------------------------------------
// Validation & Availability
// -----------------------------------------------------------------------------

bool Flight::isValidSeatNumber(const std::string& seatNumber) const {
    return seats.find(QString::fromStdString(seatNumber)) != seats.end();
}

bool Flight::isSeatValid(const QString& seatNumber) const {
    return seats.find(seatNumber) != seats.end();
}

bool Flight::isSeatAvailable(const QString& seatNumber) const {
    auto it = seats.find(seatNumber);
    return it != seats.end() && it->second->isAvailable();
}

bool Flight::isSeatOccupied(const QString& seatNumber) const {
    auto it = seats.find(seatNumber);
    return it != seats.end() && it->second->isOccupied();
}

// -----------------------------------------------------------------------------
// Booking & Cancellation
// -----------------------------------------------------------------------------

bool Flight::assignSeat(Passenger* passenger, const QString& seatNumber) {
    if (!passenger || !isSeatValid(seatNumber)) return false;
    auto it = seats.find(seatNumber);
    if (it == seats.end() || !it->second->isAvailable()) return false;
    if (it->second->setPassenger(passenger)) {
        passenger->setSeatNumber(seatNumber);
        return true;
    }
    return false;
}

bool Flight::unassignSeat(const QString& seatNumber) {
    if (!isSeatValid(seatNumber)) return false;
    auto it = seats.find(seatNumber);
    it->second->clearPassenger();
    return true;
}

bool Flight::bookSeat(const std::string& seatNumber, const std::string& passengerId) {
    return assignSeat(nullptr, QString::fromStdString(seatNumber));
}

bool Flight::cancelSeat(const std::string& seatNumber) {
    return unassignSeat(QString::fromStdString(seatNumber));
}

// -----------------------------------------------------------------------------
// Passenger Management
// -----------------------------------------------------------------------------

bool Flight::addPassenger(std::unique_ptr<Passenger> passenger) {
    if (!passenger) return false;
    QString sn = passenger->getSeatNumber();
    if (!sn.isEmpty() && !assignSeat(passenger.get(), sn))
        return false;
    passengers.push_back(std::move(passenger));
    return true;
}

bool Flight::removePassenger(const QString& seatNumber) {
    if (seatNumber.isEmpty()) return false;
    auto it = std::find_if(passengers.begin(), passengers.end(),
        [&](auto& p){ return p && p->getSeatNumber() == seatNumber; });
    if (it == passengers.end()) return false;
    unassignSeat(seatNumber);
    passengers.erase(it);
    return true;
}

Passenger* Flight::getPassenger(const QString& seatNumber) const {
    if (seatNumber.isEmpty()) return nullptr;
    auto it = std::find_if(passengers.begin(), passengers.end(),
        [&](auto& p){ return p && p->getSeatNumber() == seatNumber; });
    return it != passengers.end() ? it->get() : nullptr;
}

// -----------------------------------------------------------------------------
// Seat Layout Management (DEPRECATED - layout is now handled by seat map widget)
// -----------------------------------------------------------------------------

QMap<QString, QString> Flight::getPassengerSeats() const {
    QMap<QString, QString> passengerSeats;
    for(const auto& p : passengers) {
        if (p && p->hasSeat()) {
            passengerSeats[p->getSeatNumber()] = p->getFirstName() + " " + p->getLastName();
        }
    }
    return passengerSeats;
}

void Flight::setSeatLayout(const QVector<SeatRow>& layout) {
    seatLayout = layout;
}

std::vector<QString> Flight::getAvailableSeats() const {
    std::vector<QString> available;
    available.reserve(seats.size());
    for (auto& pr : seats) {
        if (pr.second->isAvailable())
            available.push_back(pr.first);
    }
    return available;
}

// -----------------------------------------------------------------------------
// Reporting & Utilities
// -----------------------------------------------------------------------------

int Flight::getOccupiedSeats() const {
    return std::count_if(seats.begin(), seats.end(),
        [](auto& pr){ return pr.second->isOccupied(); });
}

void Flight::displaySeatMap() const {
    // qDebug() << "\nSeat Map for Flight" << flightNumber;
    // qDebug() << generateSeatMap();
}

void Flight::displayPassengerInfo() const {
    // qDebug() << "\nPassenger Info for Flight" << flightNumber;
    for (auto& pr : seats) {
        if (pr.second->isOccupied()) {
            auto* p = pr.second->getPassenger();
            // qDebug() << "Seat:" << pr.first << "Passenger:" << (p ? p->getPhoneNumber() : "(none)");
        }
    }
}

std::vector<Passenger*> Flight::getPassengers() const {
    std::vector<Passenger*> out;
    for (auto& pr : seats)
        if (pr.second->isOccupied())
            out.push_back(pr.second->getPassenger());
    return out;
}

std::vector<Passenger*> Flight::getAllPassengers() const {
    std::vector<Passenger*> out;
    out.reserve(passengers.size());
    for (auto& p : passengers)
        out.push_back(p.get());
    return out;
}

QString Flight::generateSeatMap() const {
    QString out;
    // Column headers: 1 .. kCols
    out += "   ";
    for (int c = 1; c <= cols; ++c)
        out += QString("%1 ").arg(c, 2);
    out += '\n';
    // Each row with [X]/[ ] blocks
    for (int r = 1; r <= rows; ++r) {
        out += QString("%1 ").arg(r, 2);
        for (auto& letter : getSeatLettersForRow(r)) {
            QString sn = QString::number(r) + letter;
            out += (seats.count(sn) && seats.at(sn)->isOccupied()) ? "[X]" : "[ ]";
        }
        out += '\n';
    }
    return out;
}

bool Flight::reserveSeat(const QString& seatNumber) {
    auto it = seats.find(seatNumber);
    return it != seats.end() && it->second->reserve();
}

bool Flight::cancelReservation(const QString& seatNumber) {
    auto it = seats.find(seatNumber);
    return it != seats.end() && it->second->unreserve();
}

bool Flight::blockSeat(const QString& seatNumber) {
    auto it = seats.find(seatNumber);
    return it != seats.end() && it->second->block();
}

bool Flight::unblockSeat(const QString& seatNumber) {
    auto it = seats.find(seatNumber);
    return it != seats.end() && it->second->unblock();
}

double Flight::calculateRevenue() const {
    const double BASE_PRICE = 100.0;
    double total = 0;
    for (auto& pr : seats)
        if (pr.second->isOccupied())
            total += BASE_PRICE;
    return total;
}
