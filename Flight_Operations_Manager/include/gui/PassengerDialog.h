// This header defines the `PassengerDialog` class, which provides a dialog
// for managing passenger details and seat selection within the Flight Operations Manager application.

#ifndef PASSENGERDIALOG_H
#define PASSENGERDIALOG_H

#include <QDialog>
#include <memory>
#include <QMap>
#include "models/Flight.h"
#include "models/Passenger.h"
#include "gui/SeatMapWidget.h"

namespace Ui {
class PassengerDialog;
}

/**
 * @brief Dialog for managing passenger information and seat selection
 * 
 * This dialog allows users to enter passenger details and select seats from
 * an interactive seat map. It supports both creating new passengers and
 * editing existing ones.
 */
class PassengerDialog : public QDialog {
    Q_OBJECT

public:
    explicit PassengerDialog(QWidget* parent = nullptr);
    ~PassengerDialog();

    // Getters and setters
    void setFlight(Flight* flight);
    void setPassenger(const Passenger* passenger);
    Passenger* getPassenger() const;
    
    // UI state management
    void updateSeatMap();
    void updatePassengerInfo();

public slots:
    void accept() override;
    void reject() override;

private slots:
    void validate();
    void onSeatSelected(const QString& seatNumber, double price, const QString& seatClass);
    void onTextChanged();

private:
    // UI components
    Ui::PassengerDialog* ui;
    SeatMapWidget* m_seatMapWidget;
    
    // Data models
    Flight* m_flight;
    std::unique_ptr<Passenger> m_passenger;
    QMap<QString, QRect> m_seatMap;
    
    // State
    QString m_selectedSeat;
    double m_selectedSeatPrice;
    QString m_selectedSeatClass;
    
    // Helper methods
    void setupUI();
    void setupConnections();
    void setupValidation();
    void populateFields();
    void clearFields();
    bool validateFields() const;
    void updateSeatAvailability();
    void updateButtonStates();
    
    // Seat map helper functions
    int drawCabinSection(const QString& sectionName, 
                        const QVector<SeatRow>& layout, 
                        int startRow, int rowCount,
                        int startY, int seatWidth, int seatHeight,
                        int rowSpacing, int leftSectionX, int rightSectionX,
                        int colSpacing, QMap<QString, SeatVisualInfo>& seatInfoMap) const;
    
    // Constants
    static constexpr int DIALOG_MIN_WIDTH = 1000;
    static constexpr int DIALOG_MIN_HEIGHT = 700;

protected:
    void showEvent(QShowEvent* event) override;
};

#endif // PASSENGERDIALOG_H