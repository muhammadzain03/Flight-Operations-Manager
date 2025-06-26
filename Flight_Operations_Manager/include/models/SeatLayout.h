// SeatLayout.h
// This header defines seat-related structures used throughout the application

#ifndef SEAT_LAYOUT_H
#define SEAT_LAYOUT_H

#include <QString>
#include <QVector>
#include <QMap>
#include <QRect>

// Represents a single seat in the layout
struct SeatInfo {
    bool isSeat = false;
    QString seatNumber;
    QString seatClass;  // "First", "Business", "Economy"
    int row = 0;
    int col = 0;
    bool isAisle = false;
    double price = 0.0;
};

// Represents a row of seats in the cabin
struct SeatRow {
    QString className;  // Class name (e.g., "First", "Business", "Economy")
    QVector<SeatInfo> seats;
};

// Forward declaration for SeatVisualInfo which is defined in SeatMapWidget.h
struct SeatVisualInfo;

#endif // SEAT_LAYOUT_H
