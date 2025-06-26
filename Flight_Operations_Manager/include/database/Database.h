// This header defines the `Database` class, which provides an abstract interface
// for database operations within the Flight Operations Manager application.
// It outlines common functionalities for saving, loading, updating, and deleting
// flight and passenger data, intended to be implemented by specific database backends (e.g., SQLite, File-based).

#ifndef DATABASE_H
#define DATABASE_H

#include <QString>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <memory>
#include <vector>
#include "models/Flight.h"
#include "models/Passenger.h"

class Database {
public:
    static bool saveData(const QString& filename, const std::vector<std::unique_ptr<Flight>>& flights);
    static bool loadData(const QString& filename, std::vector<std::unique_ptr<Flight>>& flights);
    static bool exportToCSV(const QString& filename, const std::vector<std::unique_ptr<Flight>>& flights);

private:
    static QJsonObject flightToJson(const Flight* flight);
    static QJsonObject passengerToJson(const Passenger* passenger);
    static std::unique_ptr<Flight> jsonToFlight(const QJsonObject& json);
    static std::unique_ptr<Passenger> jsonToPassenger(const QJsonObject& json);
};

#endif // DATABASE_H 