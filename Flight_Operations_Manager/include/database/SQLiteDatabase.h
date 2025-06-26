#ifndef SQLITE_DATABASE_H
#define SQLITE_DATABASE_H

// This header defines the `SQLiteDatabase` class, an implementation of the abstract `Database` interface.
// It provides functionalities for persisting and retrieving flight and passenger data
// to and from an SQLite database file.

#include <QString>
#include <QSqlDatabase>
#include <QSqlQuery>

/**
 * @brief The SQLiteDatabase class handles SQLite database storage for the flight management system
 */
class SQLiteDatabase {
public:
    static SQLiteDatabase& getInstance();
    bool connect();
    void disconnect();
    bool isConnected() const;

    bool saveFlight(const QString& flightId, const QString& flightData);
    bool updateFlight(const QString& flightId, const QString& flightData);
    QString loadFlight(const QString& flightId);
    bool deleteFlight(const QString& flightId);

    bool savePassenger(const QString& passengerId, const QString& passengerData);
    bool updatePassenger(const QString& passengerId, const QString& passengerData);
    QString loadPassenger(const QString& passengerId);
    bool deletePassenger(const QString& passengerId);

private:
    SQLiteDatabase();
    ~SQLiteDatabase();
    SQLiteDatabase(const SQLiteDatabase&) = delete;
    SQLiteDatabase& operator=(const SQLiteDatabase&) = delete;

    bool initTables();
    QSqlDatabase db;
    bool connected;
};

#endif // SQLITE_DATABASE_H 