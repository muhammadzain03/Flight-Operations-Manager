// This file contains the implementation for the `MainWindow` class,
// which serves as the central user interface for the Flight Operations Manager application.
// It orchestrates interactions between various GUI components, manages flight and passenger data,
// and integrates with the database backend.

#include "gui/MainWindow.h"
#include "ui_mainwindow.h"
#include "gui/FlightDialog.h"
#include "gui/PassengerDialog.h"
#include "database/Database.h"
#include <QMessageBox>
#include <QDateTime>
#include <QFileDialog>
#include <QTextEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QInputDialog>
#include <QHeaderView>
#include <QDebug>
#include <QShortcut>
#include <QAction>
#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , airline(std::make_unique<Airline>("Sample Airline"))
{
    ui->setupUi(this);
    setupUI();
    
    // A professional, dark theme with blue and gold accents
    setStyleSheet(R"(
        QMainWindow {
            background-color: #2c3e50; /* Midnight Blue */
        }
        QMenuBar {
            background-color: #34495e; /* Wet Asphalt */
            color: #ecf0f1; /* Clouds */
        }
        QMenuBar::item:selected {
            background-color: #4a627a;
        }
        QMenu {
            background-color: #34495e;
            color: #ecf0f1;
            border: 1px solid #2c3e50;
        }
        QMenu::item:selected {
            background-color: #f39c12; /* Orange Accent */
            color: #2c3e50;
        }
        QToolBar {
            background-color: #34495e;
            border: none;
        }
        QListView, QTableWidget {
            background-color: #34495e;
            color: #ecf0f1;
            border: 1px solid #2c3e50;
            border-radius: 4px;
            font-size: 13px;
        }
        QHeaderView::section {
            background-color: #4a627a;
            color: #ecf0f1;
            padding: 5px;
            border: none;
            font-weight: bold;
        }
        QListView::item:hover, QTableWidget::item:hover {
            background-color: #4a627a;
        }
        QListView::item:selected, QTableWidget::item:selected {
            background-color: #f39c12;
            color: #2c3e50;
        }
        QCalendarWidget {
            background-color: #2c3e50;
            color: #ecf0f1;
            border: 1px solid #f39c12;
            border-radius: 8px;
        }
        QCalendarWidget QAbstractItemView {
            background-color: #34495e;
            color: #ecf0f1;
            selection-background-color: #f39c12;
            selection-color: #2c3e50;
        }
        QCalendarWidget QWidget#qt_calendar_navigationbar {
            background-color: #34495e;
        }
        QCalendarWidget QToolButton {
            background-color: #f39c12;
            color: #2c3e50;
            border-radius: 4px;
            font-weight: bold;
        }
        QCalendarWidget QToolButton:hover {
            background-color: #f1c40f;
        }
        QCalendarWidget QToolButton:pressed {
            background-color: #d35400;
        }
    )");

    // Tooltips for accessibility
    ui->flightList->setToolTip(tr("List of all flights. Select a flight to view or manage its passengers."));
    ui->passengerTable->setToolTip(tr("Table of passengers for the selected flight. Select a row to edit or remove a passenger."));
    if (auto* action = ui->actionAddPassenger) action->setToolTip(tr("Add a new passenger to the selected flight."));
    if (auto* action = ui->actionEditPassenger) action->setToolTip(tr("Edit the selected passenger's details."));
    if (auto* action = ui->actionRemovePassenger) action->setToolTip(tr("Remove the selected passenger from the flight."));
    if (auto* action = ui->actionSearchPassenger) action->setToolTip(tr("Search for a passenger by name or phone number."));
    if (auto* action = ui->actionNewFlight) action->setToolTip(tr("Create a new flight."));
    if (auto* action = ui->actionEditFlight) action->setToolTip(tr("Edit the selected flight's details."));
    if (auto* action = ui->actionDeleteFlight) action->setToolTip(tr("Delete the selected flight and all its passengers."));
    if (auto* action = ui->actionSaveData) action->setToolTip(tr("Save all flight and passenger data to a file."));
    if (auto* action = ui->actionLoadData) action->setToolTip(tr("Load flight and passenger data from a file."));
    if (auto* action = ui->actionExportData) action->setToolTip(tr("Export all data to a CSV file."));
    if (auto* action = ui->actionFlightReport) action->setToolTip(tr("Generate a report for the selected flight."));
    if (auto* action = ui->actionPassengerReport) action->setToolTip(tr("Generate a passenger report for the selected flight."));
    if (auto* action = ui->actionRevenueReport) action->setToolTip(tr("Generate a revenue report for all flights."));
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::setupUI() {
    // Set up flight list
    ui->flightList->setSelectionMode(QAbstractItemView::SingleSelection);
    
    // Set up passenger table
    ui->passengerTable->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->passengerTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->passengerTable->horizontalHeader()->setStretchLastSection(true);
    ui->passengerTable->verticalHeader()->setVisible(false);
    
    // Set up seat map
    // TODO: Initialize seat map widget
    
    updateFlightList();
    updatePassengerTable();
}

void MainWindow::updateFlightList() {
    ui->flightList->clear();
    
    for (const auto& flight : airline->getFlights()) {
        QString displayText = QString("%1 - %2 to %3 (%4)")
            .arg(flight->getFlightNumber())
            .arg(flight->getOrigin())
            .arg(flight->getDestination())
            .arg(flight->getDepartureTime().toString("yyyy-MM-dd hh:mm"));
        ui->flightList->addItem(displayText);
    }
}

void MainWindow::updatePassengerTable() {
    ui->passengerTable->clearContents();
    ui->passengerTable->setRowCount(0);
    
    QListWidgetItem* item = ui->flightList->currentItem();
    if (!item) return;
    
    QString flightNumber = item->text().split(" - ").first();
    Flight* flight = airline->getFlight(flightNumber);
    if (!flight) return;
    
    auto passengers = flight->getPassengers();
    ui->passengerTable->setRowCount(passengers.size());
    
    for (size_t i = 0; i < passengers.size(); ++i) {
        Passenger* passenger = passengers[i];
        ui->passengerTable->setItem(i, 0, new QTableWidgetItem(passenger->getFirstName()));
        ui->passengerTable->setItem(i, 1, new QTableWidgetItem(passenger->getLastName()));
        ui->passengerTable->setItem(i, 2, new QTableWidgetItem(passenger->getPhoneNumber()));
        ui->passengerTable->setItem(i, 3, new QTableWidgetItem(passenger->getSeatNumber()));
    }
}

void MainWindow::updateSeatMap() {
    // TODO: Update seat map visualization
}

Flight* MainWindow::getCurrentFlight() const {
    int currentRow = ui->flightList->currentRow();
    if (currentRow >= 0 && currentRow < static_cast<int>(airline->getFlights().size())) {
        return airline->getFlights()[currentRow].get();
    }
    return nullptr;
}

Passenger* MainWindow::getCurrentPassenger() const {
    int currentRow = ui->passengerTable->currentRow();
    auto currentFlight = getCurrentFlight();
    if (currentFlight && currentRow >= 0 && 
        currentRow < static_cast<int>(currentFlight->getPassengers().size())) {
        return currentFlight->getPassengers()[currentRow];
    }
    return nullptr;
}

void MainWindow::on_actionNewFlight_triggered() {
    FlightDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        auto flight = std::make_unique<Flight>(
            dialog.flightNumber(),
            dialog.origin(),
            dialog.destination(),
            dialog.departureTime()
        );
        if (flight) {
            airline->addFlight(std::move(flight));
            updateFlightList();
            ui->flightList->clearSelection();
        }
    }
}

void MainWindow::on_actionEditFlight_triggered() {
    QListWidgetItem* item = ui->flightList->currentItem();
    if (!item) {
        QMessageBox::warning(this, tr("No Selection"), tr("Please select a flight to edit."));
        return;
    }
    QString flightNumber = item->text().split(" - ").first();
    Flight* flight = airline->getFlight(flightNumber);
    if (!flight) {
        QMessageBox::critical(this, tr("Error"), tr("Selected flight not found."));
        return;
    }
    FlightDialog dialog(this);
    dialog.setFlight(flight);
    if (dialog.exec() == QDialog::Accepted) {
        auto newFlight = std::make_unique<Flight>(
            dialog.flightNumber(),
            dialog.origin(),
            dialog.destination(),
            dialog.departureTime()
        );
        if (newFlight) {
            // Preserve passengers from the old flight
            for (auto* p : flight->getAllPassengers()) {
                newFlight->addPassenger(std::make_unique<Passenger>(*p));
            }
            airline->updateFlight(flightNumber, std::move(newFlight));
            updateFlightList();
            updatePassengerTable();
            ui->flightList->clearSelection();
        }
    }
}

void MainWindow::on_actionDeleteFlight_triggered() {
    QListWidgetItem* item = ui->flightList->currentItem();
    if (!item) {
        QMessageBox::warning(this, tr("No Selection"), tr("Please select a flight to delete."));
        return;
    }
    QString flightNumber = item->text().split(" - ").first();
    Flight* flight = airline->getFlight(flightNumber);
    if (!flight) {
        QMessageBox::critical(this, tr("Error"), tr("Selected flight not found."));
        return;
    }
    QString msg = tr("Are you sure you want to delete flight <b>%1</b>?<br><br>" \
                    "<b>Origin:</b> %2<br>"
                    "<b>Destination:</b> %3<br>"
                    "<b>Departure:</b> %4<br>"
                    "<b>Passengers:</b> %5<br><br>"
                    "This will also remove all passengers booked on this flight.")
        .arg(flight->getFlightNumber())
        .arg(flight->getOrigin())
        .arg(flight->getDestination())
        .arg(flight->getDepartureTime().toString("yyyy-MM-dd hh:mm"))
        .arg(flight->getAllPassengers().size());
    QDialog confirmDialog(this);
    confirmDialog.setWindowTitle(tr("Confirm Deletion"));
    QVBoxLayout* layout = new QVBoxLayout(&confirmDialog);
    QLabel* label = new QLabel(msg, &confirmDialog);
    label->setTextFormat(Qt::RichText);
    layout->addWidget(label);
    QDialogButtonBox* box = new QDialogButtonBox(QDialogButtonBox::Yes | QDialogButtonBox::No, &confirmDialog);
    layout->addWidget(box);
    connect(box, &QDialogButtonBox::accepted, &confirmDialog, &QDialog::accept);
    connect(box, &QDialogButtonBox::rejected, &confirmDialog, &QDialog::reject);
    // Fade-in effect
    QGraphicsOpacityEffect* effect = new QGraphicsOpacityEffect(&confirmDialog);
    confirmDialog.setGraphicsEffect(effect);
    QPropertyAnimation* anim = new QPropertyAnimation(effect, "opacity", &confirmDialog);
    anim->setDuration(180);
    anim->setStartValue(0.0);
    anim->setEndValue(1.0);
    anim->setEasingCurve(QEasingCurve::InOutQuad);
    anim->start(QAbstractAnimation::DeleteWhenStopped);
    if (confirmDialog.exec() == QDialog::Accepted) {
        airline->removeFlight(flightNumber);
        updateFlightList();
        updatePassengerTable();
        ui->flightList->clearSelection();
    }
}

void MainWindow::on_actionAddPassenger_triggered() {
    QListWidgetItem* item = ui->flightList->currentItem();
    if (!item) {
        QMessageBox::warning(this, tr("No Selection"), tr("Please select a flight to add a passenger to."));
        return;
    }
    QString flightNumber = item->text().split(" - ").first();
    Flight* flight = airline->getFlight(flightNumber);
    if (!flight) {
        QMessageBox::critical(this, tr("Error"), tr("Selected flight not found."));
        return;
    }
    PassengerDialog dialog(this);
    dialog.setFlight(flight);
    if (dialog.exec() == QDialog::Accepted) {
        std::unique_ptr<Passenger> passenger(dialog.getPassenger());
        if (passenger) {
            flight->addPassenger(std::move(passenger));
            updatePassengerTable();
            ui->passengerTable->clearSelection();
        }
    }
}

void MainWindow::on_actionEditPassenger_triggered() {
    QTableWidgetItem* item = ui->passengerTable->currentItem();
    if (!item) {
        QMessageBox::warning(this, tr("No Selection"), tr("Please select a passenger to edit."));
        return;
    }
    QListWidgetItem* flightItem = ui->flightList->currentItem();
    if (!flightItem) {
        QMessageBox::critical(this, tr("Error"), tr("No flight selected."));
        return;
    }
    QString flightNumber = flightItem->text().split(" - ").first();
    Flight* flight = airline->getFlight(flightNumber);
    if (!flight) {
        QMessageBox::critical(this, tr("Error"), tr("Selected flight not found."));
        return;
    }
    QString seatNumber = ui->passengerTable->item(item->row(), 3)->text();
    Passenger* passenger = flight->getPassenger(seatNumber);
    if (!passenger) {
        QMessageBox::critical(this, tr("Error"), tr("Selected passenger not found."));
        return;
    }
    PassengerDialog dialog(this);
    dialog.setFlight(flight);
    dialog.setPassenger(passenger);
    if (dialog.exec() == QDialog::Accepted) {
        std::unique_ptr<Passenger> newPassenger(dialog.getPassenger());
        if (newPassenger) {
            flight->removePassenger(seatNumber);
            flight->addPassenger(std::move(newPassenger));
            updatePassengerTable();
            ui->passengerTable->clearSelection();
        }
    }
}

void MainWindow::on_actionRemovePassenger_triggered() {
    QTableWidgetItem* item = ui->passengerTable->currentItem();
    if (!item) {
        QMessageBox::warning(this, tr("No Selection"), tr("Please select a passenger to remove."));
        return;
    }
    QListWidgetItem* flightItem = ui->flightList->currentItem();
    if (!flightItem) {
        QMessageBox::critical(this, tr("Error"), tr("No flight selected."));
        return;
    }
    QString flightNumber = flightItem->text().split(" - ").first();
    Flight* flight = airline->getFlight(flightNumber);
    if (!flight) {
        QMessageBox::critical(this, tr("Error"), tr("Selected flight not found."));
        return;
    }
    QString seatNumber = ui->passengerTable->item(item->row(), 3)->text();
    Passenger* passenger = flight->getPassenger(seatNumber);
    if (!passenger) {
        QMessageBox::critical(this, tr("Error"), tr("Selected passenger not found."));
        return;
    }
    QString msg = tr("Are you sure you want to remove passenger <b>%1 %2</b> (Seat: %3)?<br><br>" \
                    "<b>Phone:</b> %4")
        .arg(passenger->getFirstName())
        .arg(passenger->getLastName())
        .arg(passenger->getSeatNumber())
        .arg(passenger->getPhoneNumber());
    QDialog confirmDialog(this);
    confirmDialog.setWindowTitle(tr("Confirm Removal"));
    QVBoxLayout* layout = new QVBoxLayout(&confirmDialog);
    QLabel* label = new QLabel(msg, &confirmDialog);
    label->setTextFormat(Qt::RichText);
    layout->addWidget(label);
    QDialogButtonBox* box = new QDialogButtonBox(QDialogButtonBox::Yes | QDialogButtonBox::No, &confirmDialog);
    layout->addWidget(box);
    connect(box, &QDialogButtonBox::accepted, &confirmDialog, &QDialog::accept);
    connect(box, &QDialogButtonBox::rejected, &confirmDialog, &QDialog::reject);
    // Fade-in effect
    QGraphicsOpacityEffect* effect = new QGraphicsOpacityEffect(&confirmDialog);
    confirmDialog.setGraphicsEffect(effect);
    QPropertyAnimation* anim = new QPropertyAnimation(effect, "opacity", &confirmDialog);
    anim->setDuration(180);
    anim->setStartValue(0.0);
    anim->setEndValue(1.0);
    anim->setEasingCurve(QEasingCurve::InOutQuad);
    anim->start(QAbstractAnimation::DeleteWhenStopped);
    if (confirmDialog.exec() == QDialog::Accepted) {
        flight->removePassenger(seatNumber);
        updatePassengerTable();
        ui->passengerTable->clearSelection();
    }
}

void MainWindow::on_actionSearchPassenger_triggered() {
    bool ok = false;
    QString query = QInputDialog::getText(this, tr("Search Passenger"), tr("Enter name or phone number:"), QLineEdit::Normal, QString(), &ok);
    if (!ok || query.trimmed().isEmpty()) return;
    auto results = airline->searchPassengers(query.trimmed());
    if (results.empty()) {
        QMessageBox::information(this, tr("No Results"), tr("No passengers found matching '%1'.").arg(query));
        return;
    }
    QString msg = tr("Found %1 passenger(s):\n\n").arg(results.size());
    for (const auto* p : results) {
        msg += QString("%1 %2, Phone: %3, Seat: %4\n").arg(p->getFirstName()).arg(p->getLastName()).arg(p->getPhoneNumber()).arg(p->getSeatNumber());
    }
    QMessageBox::information(this, tr("Search Results"), msg);
}

void MainWindow::on_actionBookTicket_triggered() {
    // Redirect to add passenger
    on_actionAddPassenger_triggered();
}

void MainWindow::on_actionCancelBooking_triggered() {
    // Redirect to remove passenger
    on_actionRemovePassenger_triggered();
}

void MainWindow::on_actionChangeBooking_triggered() {
    // Redirect to edit passenger
    on_actionEditPassenger_triggered();
}

void MainWindow::on_actionFlightReport_triggered() {
    Flight* flight = getCurrentFlight();
    if (!flight) {
        QMessageBox::warning(this, tr("No Selection"), tr("Please select a flight to generate report."));
        return;
    }
    
    QString report = QString("Flight Report\n\n"
                           "Flight Number: %1\n"
                           "Origin: %2\n"
                           "Destination: %3\n"
                           "Departure Time: %4\n"
                           "Total Seats: %5\n"
                           "Occupied Seats: %6\n"
                           "Available Seats: %7\n"
                           "Occupancy Rate: %8%")
                        .arg(flight->getFlightNumber())
                        .arg(flight->getOrigin())
                        .arg(flight->getDestination())
                        .arg(flight->getDepartureTime().toString("yyyy-MM-dd hh:mm"))
                        .arg(flight->getRows() * flight->getCols())
                        .arg(flight->getAllPassengers().size())
                        .arg(flight->getRows() * flight->getCols() - flight->getAllPassengers().size())
                        .arg(flight->getAllPassengers().size() * 100.0 / (flight->getRows() * flight->getCols()), 0, 'f', 1);
    
    generateReport("Flight Report", report);
}

void MainWindow::on_actionPassengerReport_triggered() {
    Flight* flight = getCurrentFlight();
    if (!flight) {
        QMessageBox::warning(this, tr("No Selection"), tr("Please select a flight to generate passenger report."));
        return;
    }
    
    QString report = QString("Passenger Report - Flight %1\n\n").arg(flight->getFlightNumber());
    
    for (const auto& passenger : flight->getAllPassengers()) {
        report += QString("Name: %1 %2\n"
                        "Phone: %3\n"
                        "Seat: %4\n\n")
                     .arg(passenger->getFirstName())
                     .arg(passenger->getLastName())
                     .arg(passenger->getPhoneNumber())
                     .arg(passenger->getSeatNumber());
    }
    
    generateReport("Passenger Report", report);
}

void MainWindow::on_actionRevenueReport_triggered() {
    double totalRevenue = 0.0;
    int totalPassengers = 0;
    
    QString report = "Revenue Report\n\n";
    
    for (const auto& flight : airline->getFlights()) {
        double flightRevenue = flight->calculateRevenue();
        int passengerCount = flight->getAllPassengers().size();
        
        report += QString("Flight %1:\n"
                        "Passengers: %2\n"
                        "Revenue: $%3\n\n")
                     .arg(flight->getFlightNumber())
                     .arg(passengerCount)
                     .arg(flightRevenue, 0, 'f', 2);
        
        totalRevenue += flightRevenue;
        totalPassengers += passengerCount;
    }
    
    report += QString("\nTotal Passengers: %1\n"
                     "Total Revenue: $%2\n"
                     "Average Revenue per Passenger: $%3")
                 .arg(totalPassengers)
                 .arg(totalRevenue, 0, 'f', 2)
                 .arg(totalPassengers > 0 ? totalRevenue / totalPassengers : 0.0, 0, 'f', 2);
    
    generateReport("Revenue Report", report);
}

void MainWindow::on_actionSaveData_triggered() {
    QString filename = QFileDialog::getSaveFileName(this,
        tr("Save Flight Data"), "",
        tr("Flight Data (*.json);;All Files (*)"));
    
    if (filename.isEmpty()) return;
    
    if (!filename.endsWith(".json")) {
        filename += ".json";
    }
    
    if (Database::saveData(filename, airline->getFlights())) {
        QMessageBox::information(this, tr("Success"), tr("Data saved successfully."));
    } else {
        QMessageBox::critical(this, tr("Error"), tr("Failed to save data."));
    }
}

void MainWindow::on_actionLoadData_triggered() {
    QString filename = QFileDialog::getOpenFileName(this,
        tr("Load Flight Data"), "",
        tr("Flight Data (*.json);;All Files (*)"));
    
    if (filename.isEmpty()) return;
    
    std::vector<std::unique_ptr<Flight>> flights;
    if (Database::loadData(filename, flights)) {
        airline->setFlights(std::move(flights));
        updateFlightList();
        updatePassengerTable();
        QMessageBox::information(this, tr("Success"), tr("Data loaded successfully."));
    } else {
        QMessageBox::critical(this, tr("Error"), tr("Failed to load data."));
    }
}

void MainWindow::on_actionExportData_triggered() {
    QString filename = QFileDialog::getSaveFileName(this,
        tr("Export Data"), "",
        tr("CSV Files (*.csv);;All Files (*)"));
    
    if (filename.isEmpty()) return;
    
    if (!filename.endsWith(".csv")) {
        filename += ".csv";
    }
    
    if (Database::exportToCSV(filename, airline->getFlights())) {
        QMessageBox::information(this, tr("Success"), tr("Data exported successfully."));
    } else {
        QMessageBox::critical(this, tr("Error"), tr("Failed to export data."));
    }
}

void MainWindow::generateReport(const QString& title, const QString& content) {
    QDialog dialog(this);
    dialog.setWindowTitle(title);
    dialog.setMinimumSize(400, 300);

    // Apply the modern, glossy stylesheet to the report dialog
    dialog.setStyleSheet(R"(
        QDialog {
            background-color: #2c3e50;
            color: #ecf0f1;
            font-family: 'Segoe UI';
        }
        QTextEdit {
            background-color: #34495e;
            color: #ecf0f1;
            border: 1px solid #2c3e50;
            border-radius: 4px;
            font-size: 13px;
        }
        QPushButton {
            background-color: #f39c12;
            color: #2c3e50;
            border: none;
            border-radius: 4px;
            padding: 10px 24px;
            font-weight: bold;
        }
        QPushButton:hover {
            background-color: #f1c40f;
        }
        QPushButton:pressed {
            background-color: #d35400;
        }
    )");

    QVBoxLayout* layout = new QVBoxLayout(&dialog);
    QTextEdit* textEdit = new QTextEdit(&dialog);
    textEdit->setReadOnly(true);
    textEdit->setPlainText(content);
    layout->addWidget(textEdit);
    QPushButton* saveButton = new QPushButton(tr("Save Report"), &dialog);
    connect(saveButton, &QPushButton::clicked, [&]() {
        QString filename = QFileDialog::getSaveFileName(&dialog,
            tr("Save Report"), "",
            tr("Text Files (*.txt);;All Files (*)"));
        if (!filename.isEmpty()) {
            if (!filename.endsWith(".txt")) {
                filename += ".txt";
            }
            QFile file(filename);
            if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
                QTextStream out(&file);
                out << content;
                QMessageBox::information(&dialog, tr("Success"), tr("Report saved successfully."));
            } else {
                QMessageBox::critical(&dialog, tr("Error"), tr("Failed to save report."));
            }
        }
    });
    layout->addWidget(saveButton);
    QPushButton* closeButton = new QPushButton(tr("Close"), &dialog);
    connect(closeButton, &QPushButton::clicked, &dialog, &QDialog::accept);
    layout->addWidget(closeButton);
    dialog.exec();
}

void MainWindow::updateStatusBar() {
    QString status = QString("Flights: %1 | Total Passengers: %2 | Total Revenue: $%3")
                        .arg(airline->getFlights().size())
                        .arg([this]() {
                            int total = 0;
                            for (const auto& flight : airline->getFlights()) {
                                total += flight->getAllPassengers().size();
                            }
                            return total;
                        }())
                        .arg([this]() {
                            double total = 0.0;
                            for (const auto& flight : airline->getFlights()) {
                                total += flight->calculateRevenue();
                            }
                            return total;
                        }(), 0, 'f', 2);
    
    statusBar()->showMessage("Your message here...");
}