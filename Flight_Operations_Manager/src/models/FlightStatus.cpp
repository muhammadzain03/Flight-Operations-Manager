#include "models/FlightStatus.h"
#include <QDateTime>
#include <QDebug> // For debugging purposes, can be removed later

// This file contains the implementation for the `FlightStatus` related functionalities.
// It provides the logic for managing and tracking the operational status of flights,
// including delays, cancellations, and diversions, and maintaining a history of status updates.

FlightStatus::FlightStatus()
    : currentStatus(Status::OnTime)
    , currentGate(QString())
    , delayMinutes(0)
    , divertedTo(QString())
    , scheduledDeparture(QDateTime())
    , scheduledArrival(QDateTime())
    , estimatedDeparture(QDateTime())
    , estimatedArrival(QDateTime())
{
    // Initial status update
    addStatusUpdate(Status::OnTime, "Flight created, on time.");
}

void FlightStatus::addStatusUpdate(Status status, const QString& reason)
{
    StatusUpdate update;
    update.status = status;
    update.reason = reason;
    update.timestamp = QDateTime::currentDateTime();
    update.gate = currentGate; // Capture current gate at time of update
    statusHistory.push_back(update);
    currentStatus = status; // Update current status
}

void FlightStatus::updateEstimatedTimes()
{
    if (scheduledDeparture.isValid()) {
        estimatedDeparture = scheduledDeparture.addSecs(delayMinutes * 60);
    }
    if (scheduledArrival.isValid()) {
        estimatedArrival = scheduledArrival.addSecs(delayMinutes * 60);
    }
}

void FlightStatus::updateStatus(Status newStatus, const QString& reason)
{
    addStatusUpdate(newStatus, reason);
    updateEstimatedTimes();
}

void FlightStatus::setDelay(int minutes, const QString& reason)
{
    delayMinutes = minutes;
    if (delayMinutes > 0) {
        addStatusUpdate(Status::Delayed, reason);
    } else {
        // If delay is cleared or negative, revert to OnTime if no other status is set
        if (currentStatus == Status::Delayed) {
            addStatusUpdate(Status::OnTime, "Delay cleared.");
        }
    }
    updateEstimatedTimes();
}

void FlightStatus::setGate(const QString& newGate)
{
    currentGate = newGate;
    // Optionally add a status update if the gate change is significant or needs logging
    // addStatusUpdate(currentStatus, "Gate changed to " + newGate);
}

void FlightStatus::cancel(const QString& reason)
{
    addStatusUpdate(Status::Cancelled, reason);
}

void FlightStatus::divert(const QString& newDestination, const QString& reason)
{
    divertedTo = newDestination;
    addStatusUpdate(Status::Diverted, reason);
}

void FlightStatus::setScheduledDeparture(const QDateTime& departure)
{
    scheduledDeparture = departure;
    updateEstimatedTimes();
}

void FlightStatus::setScheduledArrival(const QDateTime& arrival)
{
    scheduledArrival = arrival;
    updateEstimatedTimes();
} 