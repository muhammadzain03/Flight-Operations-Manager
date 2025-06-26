// This header defines the `FlightStatus` enum and related utilities,
// representing the various operational statuses a flight can have (e.g., Scheduled, Departed, Arrived, Cancelled).
// It helps in tracking and displaying the current state of flights in the Flight Operations Manager application.

#ifndef FLIGHTSTATUS_H
#define FLIGHTSTATUS_H

#include <QtCore/QString>
#include <QtCore/QDateTime>
#include <vector>
#include <memory>

class FlightStatus {
public:
    enum class Status {
        OnTime,
        Delayed,
        Boarding,
        Departed,
        Arrived,
        Cancelled,
        Diverted
    };

    struct StatusUpdate {
        Status status;
        QString reason;
        QDateTime timestamp;
        QString gate;
    };

    FlightStatus();
    
    // Status management
    void updateStatus(Status newStatus, const QString& reason = QString());
    void setDelay(int minutes, const QString& reason);
    void setGate(const QString& newGate);
    void cancel(const QString& reason);
    void divert(const QString& newDestination, const QString& reason);
    
    // Getters
    Status getCurrentStatus() const { return currentStatus; }
    QString getCurrentGate() const { return currentGate; }
    int getDelayMinutes() const { return delayMinutes; }
    QDateTime getEstimatedDeparture() const { return estimatedDeparture; }
    QDateTime getEstimatedArrival() const { return estimatedArrival; }
    const std::vector<StatusUpdate>& getStatusHistory() const { return statusHistory; }
    
    // Setters
    void setScheduledDeparture(const QDateTime& departure);
    void setScheduledArrival(const QDateTime& arrival);
    
    // Status checks
    bool isDelayed() const { return delayMinutes > 0; }
    bool isCancelled() const { return currentStatus == Status::Cancelled; }
    bool isDiverted() const { return currentStatus == Status::Diverted; }
    bool isBoarding() const { return currentStatus == Status::Boarding; }

private:
    Status currentStatus;
    QString currentGate;
    int delayMinutes;
    QString divertedTo;
    QDateTime scheduledDeparture;
    QDateTime scheduledArrival;
    QDateTime estimatedDeparture;
    QDateTime estimatedArrival;
    std::vector<StatusUpdate> statusHistory;

    void addStatusUpdate(Status status, const QString& reason);
    void updateEstimatedTimes();
};

#endif // FLIGHTSTATUS_H 