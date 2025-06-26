// This header defines the `MainWindow` class, which represents the main window
// of the Flight Operations Manager application. It declares the UI elements
// and slots for interacting with flights, passengers, and other core functionalities.

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <memory>
#include "models/Airline.h"
#include "models/Flight.h"
#include "models/Passenger.h"
#include "gui/SeatMapWidget.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    // File menu
    void on_actionSaveData_triggered();
    void on_actionLoadData_triggered();
    void on_actionExportData_triggered();
    
    // Flight menu
    void on_actionNewFlight_triggered();
    void on_actionEditFlight_triggered();
    void on_actionDeleteFlight_triggered();
    
    // Passenger menu
    void on_actionAddPassenger_triggered();
    void on_actionEditPassenger_triggered();
    void on_actionRemovePassenger_triggered();
    void on_actionSearchPassenger_triggered();
    
    // Booking menu
    void on_actionBookTicket_triggered();
    void on_actionCancelBooking_triggered();
    void on_actionChangeBooking_triggered();
    
    // Reports menu
    void on_actionFlightReport_triggered();
    void on_actionPassengerReport_triggered();
    void on_actionRevenueReport_triggered();
    
    // View updates
    void updateFlightList();
    void updatePassengerTable();
    void updateSeatMap();
    void updateStatusBar();

private:
    Ui::MainWindow *ui;
    std::unique_ptr<Airline> airline;
    SeatMapWidget *seatMapWidget;
    QTimer *statusTimer;

    void setupUI();
    void loadSettings();
    void saveSettings();
    void createStatusBar();

    // Helper methods
    void showError(const QString& message);
    void showSuccess(const QString& message);
    bool confirmAction(const QString& action);

    // Data validation
    bool validateFlightData(const QString& id, int rows, int cols);
    bool validatePassengerData(const QString& name, const QString& phone);
    bool validateBookingData(const QString& flightId, const QString& seatNo);

    Flight* getCurrentFlight() const;
    Passenger* getCurrentPassenger() const;

    void generateReport(const QString& title, const QString& content);
};

#endif // MAINWINDOW_H