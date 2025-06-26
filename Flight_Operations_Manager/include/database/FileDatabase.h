#ifndef FILE_DATABASE_H
#define FILE_DATABASE_H

// This header defines the `FileDatabase` class, an implementation of the abstract `Database` interface.
// It provides functionalities for persisting and retrieving flight and passenger data
// to and from local files, typically in a JSON format.

#include <QString>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <memory>
#include <vector>
#include "models/Flight.h"
#include "models/Passenger.h"

/**
 * @brief The FileDatabase class handles file-based storage (JSON/CSV) for the flight management system
 */
class FileDatabase {
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

#endif // FILE_DATABASE_H 