// This file contains the implementation for the `Database` class,
// providing core functionalities for managing flight and passenger data
// within the Flight Operations Manager application. It interacts with an SQLite database
// to perform save, load, update, and delete operations for various data models.

#include "database/Database.h"
#include <QFile>
#include <QTextStream>
#include <QDateTime>

bool Database::saveData(const QString& filename, const std::vector<std::unique_ptr<Flight>>& flights) {
    QJsonArray flightsArray;
    for (const auto& flight : flights) {
        flightsArray.append(flightToJson(flight.get()));
    }

    QJsonObject root;
    root["flights"] = flightsArray;
    
    QJsonDocument doc(root);
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly)) {
        return false;
    }
    
    file.write(doc.toJson());
    return true;
}

bool Database::loadData(const QString& filename, std::vector<std::unique_ptr<Flight>>& flights) {
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly)) {
        return false;
    }
    
    QByteArray data = file.readAll();
    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (doc.isNull()) {
        return false;
    }
    
    QJsonObject root = doc.object();
    QJsonArray flightsArray = root["flights"].toArray();
    
    flights.clear();
    for (const QJsonValue& value : flightsArray) {
        if (auto flight = jsonToFlight(value.toObject())) {
            flights.push_back(std::move(flight));
        }
    }
    
    return true;
}

bool Database::exportToCSV(const QString& filename, const std::vector<std::unique_ptr<Flight>>& flights) {
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return false;
    }
    
    QTextStream out(&file);
    
    // Write header
    out << "Flight Number,Origin,Destination,Departure Time,Passenger Name,Seat Number\n";
    
    // Write data
    for (const auto& flight : flights) {
        QString flightNumber = flight->getFlightNumber();
        QString origin = flight->getOrigin();
        QString destination = flight->getDestination();
        QString departureTime = flight->getDepartureTime().toString("yyyy-MM-dd hh:mm");
        
        for (const auto& passenger : flight->getAllPassengers()) {
            out << QString("%1,%2,%3,%4,%5,%6\n")
                   .arg(flightNumber)
                   .arg(origin)
                   .arg(destination)
                   .arg(departureTime)
                   .arg(passenger->getFullName())
                   .arg(passenger->getSeatNumber());
        }
    }
    
    return true;
}

QJsonObject Database::flightToJson(const Flight* flight) {
    QJsonObject obj;
    obj["flightNumber"] = flight->getFlightNumber();
    obj["origin"] = flight->getOrigin();
    obj["destination"] = flight->getDestination();
    obj["departureTime"] = flight->getDepartureTime().toString(Qt::ISODate);
    obj["rows"] = flight->getRows();
    obj["cols"] = flight->getCols();
    
    QJsonArray passengersArray;
    for (const auto& passenger : flight->getAllPassengers()) {
        passengersArray.append(passengerToJson(passenger));
    }
    obj["passengers"] = passengersArray;
    
    return obj;
}

QJsonObject Database::passengerToJson(const Passenger* passenger) {
    QJsonObject obj;
    obj["firstName"] = passenger->getFirstName();
    obj["lastName"] = passenger->getLastName();
    obj["phoneNumber"] = passenger->getPhoneNumber();
    obj["seatNumber"] = passenger->getSeatNumber();
    return obj;
}

std::unique_ptr<Flight> Database::jsonToFlight(const QJsonObject& json) {
    QString flightNumber = json["flightNumber"].toString();
    QString origin = json["origin"].toString();
    QString destination = json["destination"].toString();
    QDateTime departureTime = QDateTime::fromString(json["departureTime"].toString(), Qt::ISODate);
    
    auto flight = std::make_unique<Flight>(flightNumber, origin, destination, departureTime);
    
    QJsonArray passengersArray = json["passengers"].toArray();
    for (const QJsonValue& value : passengersArray) {
        if (auto passenger = jsonToPassenger(value.toObject())) {
            flight->addPassenger(std::move(passenger));
        }
    }
    
    return flight;
}

std::unique_ptr<Passenger> Database::jsonToPassenger(const QJsonObject& json) {
    QString firstName = json["firstName"].toString();
    QString lastName = json["lastName"].toString();
    QString phoneNumber = json["phoneNumber"].toString();
    QString seatNumber = json["seatNumber"].toString();
    
    return std::make_unique<Passenger>(firstName, lastName, phoneNumber, seatNumber);
} 