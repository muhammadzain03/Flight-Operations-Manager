// This file contains the implementation for the `SQLiteDatabase` class.
// It provides methods for interacting with an SQLite database to store and retrieve
// flight and passenger data for the Flight Operations Manager application.

#include "database/SQLiteDatabase.h"
#include <QDebug>
#include <QDir>

SQLiteDatabase::SQLiteDatabase() : connected(false) {
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(QDir::currentPath() + "/flightmanagement.db");
}

SQLiteDatabase::~SQLiteDatabase() {
    if (connected) {
        disconnect();
    }
}

SQLiteDatabase& SQLiteDatabase::getInstance() {
    static SQLiteDatabase instance;
    return instance;
}

bool SQLiteDatabase::connect() {
    if (!connected) {
        connected = db.open();
        if (connected) {
            initTables();
        }
    }
    return connected;
}

void SQLiteDatabase::disconnect() {
    if (connected) {
        db.close();
        connected = false;
    }
}

bool SQLiteDatabase::isConnected() const {
    return connected;
}

bool SQLiteDatabase::initTables() {
    QSqlQuery query;
    
    // Create flights table
    bool success = query.exec(
        "CREATE TABLE IF NOT EXISTS flights ("
        "id TEXT PRIMARY KEY,"
        "data TEXT NOT NULL"
        ")"
    );
    
    // Create passengers table
    if (success) {
        success = query.exec(
            "CREATE TABLE IF NOT EXISTS passengers ("
            "id TEXT PRIMARY KEY,"
            "data TEXT NOT NULL"
            ")"
        );
    }
    
    return success;
}

bool SQLiteDatabase::saveFlight(const QString& flightId, const QString& flightData) {
    QSqlQuery query;
    query.prepare("INSERT OR REPLACE INTO flights (id, data) VALUES (?, ?)");
    query.addBindValue(flightId);
    query.addBindValue(flightData);
    return query.exec();
}

bool SQLiteDatabase::updateFlight(const QString& flightId, const QString& flightData) {
    return saveFlight(flightId, flightData);
}

QString SQLiteDatabase::loadFlight(const QString& flightId) {
    QSqlQuery query;
    query.prepare("SELECT data FROM flights WHERE id = ?");
    query.addBindValue(flightId);
    
    if (query.exec() && query.next()) {
        return query.value(0).toString();
    }
    return QString();
}

bool SQLiteDatabase::deleteFlight(const QString& flightId) {
    QSqlQuery query;
    query.prepare("DELETE FROM flights WHERE id = ?");
    query.addBindValue(flightId);
    return query.exec();
}

bool SQLiteDatabase::savePassenger(const QString& passengerId, const QString& passengerData) {
    QSqlQuery query;
    query.prepare("INSERT OR REPLACE INTO passengers (id, data) VALUES (?, ?)");
    query.addBindValue(passengerId);
    query.addBindValue(passengerData);
    return query.exec();
}

bool SQLiteDatabase::updatePassenger(const QString& passengerId, const QString& passengerData) {
    return savePassenger(passengerId, passengerData);
}

QString SQLiteDatabase::loadPassenger(const QString& passengerId) {
    QSqlQuery query;
    query.prepare("SELECT data FROM passengers WHERE id = ?");
    query.addBindValue(passengerId);
    
    if (query.exec() && query.next()) {
        return query.value(0).toString();
    }
    return QString();
}

bool SQLiteDatabase::deletePassenger(const QString& passengerId) {
    QSqlQuery query;
    query.prepare("DELETE FROM passengers WHERE id = ?");
    query.addBindValue(passengerId);
    return query.exec();
} 