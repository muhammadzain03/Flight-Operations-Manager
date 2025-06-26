// This header defines the `FlightDialog` class, which provides a dialog
// for creating and editing flight details within the Flight Operations Manager application.

#ifndef FLIGHTDIALOG_H
#define FLIGHTDIALOG_H

#include <QDialog>
#include <memory>
#include "models/Flight.h"

namespace Ui {
class FlightDialog;
}

class QDoubleSpinBox; // Forward declaration

class FlightDialog : public QDialog {
    Q_OBJECT

public:
    explicit FlightDialog(QWidget* parent = nullptr);
    ~FlightDialog() override;

    void setFlight(const Flight* flight);
    
    // Data retrieval methods
    QString flightNumber() const;
    QString origin() const;
    QString destination() const;
    QDateTime departureTime() const;

protected:
    void accept() override;
    void showEvent(QShowEvent* event) override;

private slots:
    bool validateForm();

private:
    Ui::FlightDialog* ui;
};

#endif // FLIGHTDIALOG_H 