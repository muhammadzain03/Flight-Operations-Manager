// PassengerDialog.cpp
// This file contains the implementation for the `PassengerDialog` class,
// which provides the user interface and logic for managing passenger details
// and seat selection in the Flight Operations Manager application. It integrates
// with the graphical seat map to allow interactive seat assignment.

// This file contains the implementation for the `PassengerDialog` class,
// which provides the user interface and logic for managing passenger details
// and seat selection in the Flight Operations Manager application.

#include "gui/PassengerDialog.h"
#include "ui_PassengerDialog.h"

#include <QMessageBox>
#include <QPushButton>
#include <QDialogButtonBox>
#include <QRegularExpression>
#include <QRegularExpressionValidator>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QFile>
#include <QDateTime>
#include <QDebug>
#include <QTimer>
#include <QGraphicsDropShadowEffect>
#include "models/Seat.h"
#include "models/Flight.h"
#include "models/Passenger.h"
#include "gui/SeatMapWidget.h"
#include "ui_PassengerDialog.h"
#include <QPropertyAnimation>
#include <QEasingCurve>
#include <random>
#include <QVBoxLayout>
#include <QFrame>
#include <QMessageBox>
#include <QShortcut>
#include <QShowEvent>
#include <QGraphicsOpacityEffect>
#include <QGraphicsScale>

static const char* EMAIL_PATTERN = "^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\\.[a-zA-Z]{2,}$";

void PassengerDialog::onTextChanged() {
    validate();
    // Live input validation feedback
    auto setError = [](QLineEdit* edit, bool error) {
        if (error)
            edit->setStyleSheet("border: 2px solid #e74c3c; background: #2c3e50; color: #ecf0f1;");
        else
            edit->setStyleSheet("");
    };
    setError(ui->firstNameEdit, !ui->firstNameEdit->hasAcceptableInput() || ui->firstNameEdit->text().trimmed().isEmpty());
    setError(ui->lastNameEdit, !ui->lastNameEdit->hasAcceptableInput() || ui->lastNameEdit->text().trimmed().isEmpty());
    setError(ui->phoneEdit, !ui->phoneEdit->hasAcceptableInput() || ui->phoneEdit->text().trimmed().isEmpty());
    setError(ui->emailEdit, !ui->emailEdit->text().isEmpty() && !QRegularExpression(EMAIL_PATTERN).match(ui->emailEdit->text()).hasMatch());
}

void PassengerDialog::updateSeatMap() {
    if (!m_flight || !m_seatMapWidget) return;

    QMap<QString, SeatVisualInfo> seatInfoMap;
    const auto& seats = m_flight->getSeats();

    // Layout properties
    const int seat_size = 32;
    const int seat_spacing = 6;
    const int aisle_width = 40;
    const int start_y = 120; // Pushed down for title/legend
    const int row_height = 40;
    const int map_width = 800; // The width of the area to center content in

    // Iterate over the seats map
    for (auto it = seats.cbegin(); it != seats.cend(); ++it) {
        const auto& seat = it->second;
        SeatVisualInfo info;
        info.seatId = seat->getNumber();
        info.seatClass = seat->getSeatClass();
        info.price = seat->getPrice();
        info.occupied = seat->isOccupied();

        int rowNum = info.seatId.chopped(1).toInt();
        char seatLetter = info.seatId.back().toUpper().toLatin1();
        
        int y = start_y + (rowNum - 1) * row_height;
        int x = 0;

        // Calculate x position based on class layout
        if (info.seatClass == "First") { // 1-2-1 layout for First Class
            int total_width = 4 * seat_size + 3 * seat_spacing + 2 * aisle_width;
            int start_x = (map_width - total_width) / 2;
            if (seatLetter == 'A') x = start_x;
            else if (seatLetter == 'D') x = start_x + 1 * (seat_size + seat_spacing) + aisle_width;
            else if (seatLetter == 'G') x = start_x + 2 * (seat_size + seat_spacing) + aisle_width;
            else if (seatLetter == 'L') x = start_x + 3 * (seat_size + seat_spacing) + 2 * aisle_width;
        } else if (info.seatClass == "Business") { // 2-4-2 layout
            int total_width = 8 * seat_size + 7 * seat_spacing + 2 * aisle_width;
            int start_x = (map_width - total_width) / 2;
            if (seatLetter <= 'B') x = start_x + (seatLetter - 'A') * (seat_size + seat_spacing);
            else if (seatLetter <= 'G') x = start_x + 2 * (seat_size + seat_spacing) + aisle_width + (seatLetter - 'D') * (seat_size + seat_spacing);
            else { // J, L block
                int base_x = start_x + 6 * (seat_size + seat_spacing) + 2 * aisle_width;
                if (seatLetter == 'J') x = base_x;
                else if (seatLetter == 'L') x = base_x + seat_size + seat_spacing;
            }
        } else { // Economy: 3-4-3 layout
            int total_width = 10 * seat_size + 9 * seat_spacing + 2 * aisle_width;
            int start_x = (map_width - total_width) / 2;
            if (seatLetter <= 'C') x = start_x + (seatLetter - 'A') * (seat_size + seat_spacing);
            else if (seatLetter <= 'G') x = start_x + 3 * (seat_size + seat_spacing) + aisle_width + (seatLetter - 'D') * (seat_size + seat_spacing);
            else { // H, J, L block
                int base_x = start_x + 7 * (seat_size + seat_spacing) + 2 * aisle_width;
                if (seatLetter == 'H') x = base_x;
                else if (seatLetter == 'J') x = base_x + seat_size + seat_spacing;
                else if (seatLetter == 'L') x = base_x + 2 * (seat_size + seat_spacing);
            }
        }

        info.rect = QRect(x, y, seat_size, seat_size);
        seatInfoMap[info.seatId] = info;
    }

    m_seatMapWidget->setSeatLayout(seatInfoMap);
}

void PassengerDialog::populateFields() {
    if (!m_passenger) {
        clearFields();
        return;
    }
    
    // Set basic information
    ui->firstNameEdit->setText(m_passenger->getFirstName());
    ui->lastNameEdit->setText(m_passenger->getLastName());
    ui->phoneEdit->setText(m_passenger->getPhoneNumber());
    
    // Set email if available
    ui->emailEdit->setText(m_passenger->getEmail());
    
    // Update seat selection
    if (m_passenger->hasSeat()) {
        m_selectedSeat = m_passenger->getSeatNumber();
        ui->seatNumberLabel->setText(m_selectedSeat);
        
        // Update seat map to show the selected seat
        if (m_seatMapWidget) {
            m_seatMapWidget->selectSeat(m_selectedSeat);
        }
    } else {
        ui->seatNumberLabel->setText("None");
        m_selectedSeat.clear();
        
        if (m_seatMapWidget) {
            m_seatMapWidget->clearSelection();
        }
    }
    
    // Validate the form
    validate();
}

void PassengerDialog::reject() {
    QDialog::reject();
}

// Animation constants
constexpr int ANIMATION_DURATION = 150;
constexpr int TOOLTIP_DELAY = 500; // ms

// Style constants
const char* STYLE_SELECTED_SEAT = "QLabel { background: #4CAF50; color: white; border-radius: 12px; padding: 4px 8px; }";
const char* STYLE_NO_SEAT = "QLabel { color: #ff6b6b; font-style: italic; }";

// Validation patterns
const char* NAME_PATTERN = "^[A-Za-z\\s'.-]{2,50}$";
const char* PHONE_PATTERN = "^[0-9+\\s-]{10,20}$";

PassengerDialog::PassengerDialog(QWidget* parent)
    : QDialog(parent)
    , ui(new Ui::PassengerDialog())
    , m_flight(nullptr)
    , m_passenger(nullptr)
    , m_selectedSeatPrice(0.0)
{
    ui->setupUi(this);

    // Create a container for the form to style it separately
    QFrame* formContainer = new QFrame();
    formContainer->setObjectName("formContainer");
    formContainer->setLayout(ui->formLayout);

    // Replace the old form layout with the new container
    // The QVBoxLayout takes ownership of the container
    delete ui->leftLayout->replaceWidget(ui->formLayout->parentWidget(), formContainer);
    
    // Get the seat map widget from the UI
    m_seatMapWidget = ui->seatMapWidget;
    
    // Configure the seat map widget
    m_seatMapWidget->setMinimumSize(600, 800);
    m_seatMapWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_seatMapWidget->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    m_seatMapWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_seatMapWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_seatMapWidget->setFrameStyle(QFrame::NoFrame);
    m_seatMapWidget->setBackgroundBrush(QBrush(QColor("#1e1e3f"))); // Darker, richer blue
    m_seatMapWidget->setAlignment(Qt::AlignCenter);
    m_seatMapWidget->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform | QPainter::TextAntialiasing);
    
    // Configure the scroll area
    ui->seatScrollArea->setWidgetResizable(true);
    ui->seatScrollArea->setFrameShape(QFrame::NoFrame);
    ui->seatScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    ui->seatScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    ui->seatScrollArea->setMinimumWidth(650);
    ui->seatScrollArea->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    ui->seatScrollArea->setStyleSheet("QScrollArea { background-color: #1e1e3f; border: none; }");
    
    // Connect signals
    connect(m_seatMapWidget, &SeatMapWidget::seatSelected,
            this, &PassengerDialog::onSeatSelected);
    connect(m_seatMapWidget, &SeatMapWidget::seatHovered, this, [this](const QString& seatNumber, double price, const QString& seatClass) {
        if (seatNumber.isEmpty()) {
            ui->seatLabel->clear();
        } else {
            ui->seatLabel->setText(QString("Seat %1 • %2 • $%3")
                .arg(seatNumber)
                .arg(seatClass)
                .arg(price, 0, 'f', 2));
        }
    });

    // Window & styling
    setMinimumSize(1200, 800);
    setSizeGripEnabled(true);
    setWindowFlags(windowFlags() | Qt::WindowMinMaxButtonsHint);

    // A professional, dark theme that matches the main window
    setStyleSheet(R"(
        PassengerDialog { 
            background-color: #2c3e50; /* Midnight Blue */
        }
        
        #formContainer {
            background-color: #34495e; /* Wet Asphalt */
            border-radius: 8px;
            padding: 20px;
        }

        #formContainer QLabel, QLabel#seatMapLabel { 
            color: #ecf0f1; /* Clouds */
            font-size: 13px;
            font-weight: 500;
        }
        
        QLineEdit { 
            background-color: #2c3e50;
            color: #ecf0f1; 
            border: 1px solid #4a627a;
            border-radius: 4px; 
            padding: 8px 12px;
            font-size: 13px;
        }
        
        QLineEdit:focus {
            border: 1px solid #f39c12; /* Orange Accent */
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
        
        QScrollBar:vertical {
            border: none;
            background: #2c3e50;
            width: 12px;
        }
        
        QScrollBar::handle:vertical {
            background: #4a627a;
            min-height: 25px;
            border-radius: 6px;
        }
    )");

    setupValidation();
    clearFields();

    // Tooltips for accessibility
    ui->firstNameEdit->setToolTip(tr("Enter the passenger's first name."));
    ui->lastNameEdit->setToolTip(tr("Enter the passenger's last name."));
    ui->phoneEdit->setToolTip(tr("Enter the passenger's phone number."));
    ui->emailEdit->setToolTip(tr("Enter the passenger's email address (optional)."));
    ui->seatNumberLabel->setToolTip(tr("Displays the selected seat number."));
    ui->seatLabel->setToolTip(tr("Displays the selected seat details."));
    if (auto* okBtn = ui->buttonBox->button(QDialogButtonBox::Ok))
        okBtn->setToolTip(tr("Save passenger details and close dialog (Enter)"));
    if (auto* cancelBtn = ui->buttonBox->button(QDialogButtonBox::Cancel))
        cancelBtn->setToolTip(tr("Cancel and close dialog (Esc)"));
    // Tab order
    setTabOrder(ui->firstNameEdit, ui->lastNameEdit);
    setTabOrder(ui->lastNameEdit, ui->phoneEdit);
    setTabOrder(ui->phoneEdit, ui->emailEdit);
    setTabOrder(ui->emailEdit, ui->buttonBox);
    // Keyboard shortcuts
    (void)new QShortcut(QKeySequence(Qt::Key_Escape), this, SLOT(reject()));
    (void)new QShortcut(QKeySequence(Qt::Key_Return), this, SLOT(accept()));
    (void)new QShortcut(QKeySequence(Qt::Key_Enter), this, SLOT(accept()));

    // Button press animation for OK/Cancel
    if (auto* okBtn = ui->buttonBox->button(QDialogButtonBox::Ok)) {
        connect(okBtn, &QPushButton::pressed, [okBtn]() {
            QPropertyAnimation* anim = new QPropertyAnimation(okBtn, "geometry");
            QRect start = okBtn->geometry();
            QRect shrink = start.adjusted(2, 2, -2, -2);
            anim->setDuration(80);
            anim->setStartValue(start);
            anim->setEndValue(shrink);
            anim->setEasingCurve(QEasingCurve::OutQuad);
            QObject::connect(anim, &QPropertyAnimation::finished, [okBtn, start]() {
                okBtn->setGeometry(start);
            });
            anim->start(QAbstractAnimation::DeleteWhenStopped);
        });
    }
    if (auto* cancelBtn = ui->buttonBox->button(QDialogButtonBox::Cancel)) {
        connect(cancelBtn, &QPushButton::pressed, [cancelBtn]() {
            QPropertyAnimation* anim = new QPropertyAnimation(cancelBtn, "geometry");
            QRect start = cancelBtn->geometry();
            QRect shrink = start.adjusted(2, 2, -2, -2);
            anim->setDuration(80);
            anim->setStartValue(start);
            anim->setEndValue(shrink);
            anim->setEasingCurve(QEasingCurve::OutQuad);
            QObject::connect(anim, &QPropertyAnimation::finished, [cancelBtn, start]() {
                cancelBtn->setGeometry(start);
            });
            anim->start(QAbstractAnimation::DeleteWhenStopped);
        });
    }
}

PassengerDialog::~PassengerDialog() {
    delete ui;
}

void PassengerDialog::setFlight(Flight* flight) {
    if (m_flight == flight) {
        return;
    }
    
    m_flight = flight;
    
    if (!m_flight) {
        QMessageBox::critical(this, tr("Error"), tr("No flight data provided."));
        reject();
        return;
    }
    
    // Update window title with flight info
    setWindowTitle(tr("Passenger Details - Flight %1").arg(m_flight->getFlightNumber()));
    
    // Make sure the seat map widget is properly initialized
    if (!m_seatMapWidget) {
        m_seatMapWidget = ui->seatMapWidget;
        
        if (m_seatMapWidget) {
            // Connect signals
            connect(m_seatMapWidget, &SeatMapWidget::seatSelected,
                   this, &PassengerDialog::onSeatSelected);
            
            // Set initial size
            m_seatMapWidget->setMinimumSize(800, 600);
        } else {
            return;
        }
    }
    
    // Update the seat map
    updateSeatMap();
    
    // Force update the widget
    if (m_seatMapWidget) {
        m_seatMapWidget->update();
        m_seatMapWidget->repaint();
        m_seatMapWidget->viewport()->update();
        
        // Ensure the widget is visible
        m_seatMapWidget->show();
    }
    
    // If we have a passenger, update their info
    if (m_passenger) {
        populateFields();
    }
}

void PassengerDialog::onSeatSelected(const QString& seatNumber, double price, const QString& seatClass) {
    m_selectedSeat = seatNumber;
    ui->seatNumberLabel->setText(QString("%1 • %2 • $%3").arg(seatNumber, seatClass, QString::number(price, 'f', 2)));
    validate();
}

void PassengerDialog::setupValidation() {
    QRegularExpression nameRx("^[A-Za-z\\s\\-]+$");
    ui->firstNameEdit->setValidator(
        new QRegularExpressionValidator(nameRx, this));
    ui->lastNameEdit->setValidator(
        new QRegularExpressionValidator(nameRx, this));

    QRegularExpression phoneRx("^\\+?[0-9]{10,15}$");
    ui->phoneEdit->setValidator(
        new QRegularExpressionValidator(phoneRx, this));
}

void PassengerDialog::clearFields() {
    ui->firstNameEdit->clear();
    ui->lastNameEdit->clear();
    ui->phoneEdit->clear();
    ui->emailEdit->clear();
    ui->seatNumberLabel->setText("None");

    // clear any highlights
    m_seatMapWidget->setOccupiedSeats({});
}

bool PassengerDialog::validateFields() const {
    return  !ui->firstNameEdit->text().trimmed().isEmpty()
         && !ui->lastNameEdit->text().trimmed().isEmpty()
         && !ui->phoneEdit->text().trimmed().isEmpty()
         && !ui->seatLabel->text().trimmed().isEmpty()
         && ui->phoneEdit->hasAcceptableInput();
}

void PassengerDialog::validate() {
    bool ok = validateFields();
    if (auto* btn = ui->buttonBox->button(QDialogButtonBox::Ok))
        btn->setEnabled(ok);
}

void PassengerDialog::accept() {
    if (!validateFields()) {
        QMessageBox::warning(this, tr("Invalid Input"),
                             tr("Please fill in all fields correctly."));
        return;
    }

    m_passenger = std::make_unique<Passenger>(
        ui->firstNameEdit->text().trimmed(),
        ui->lastNameEdit->text().trimmed(),
        ui->phoneEdit->text().trimmed(),
        ui->emailEdit->text().trimmed(),
        m_selectedSeat
    );
    QDialog::accept();
}

Passenger* PassengerDialog::getPassenger() const {
    return m_passenger ? new Passenger(*m_passenger)
                     : nullptr;
}

void PassengerDialog::setPassenger(const Passenger* p) {
    if (!p) return;
    ui->firstNameEdit->setText(p->getFirstName());
    ui->lastNameEdit->setText(p->getLastName());
    ui->phoneEdit->setText(p->getPhoneNumber());
    ui->seatLabel->setText(p->getSeatNumber());
    ui->seatNumberLabel->setText(p->getSeatNumber());
    // Optionally, update seat selection visuals here
}

void PassengerDialog::showEvent(QShowEvent* event) {
    QDialog::showEvent(event);
    QGraphicsOpacityEffect* effect = new QGraphicsOpacityEffect(this);
    setGraphicsEffect(effect);
    QPropertyAnimation* anim = new QPropertyAnimation(effect, "opacity", this);
    anim->setDuration(220);
    anim->setStartValue(0.0);
    anim->setEndValue(1.0);
    anim->setEasingCurve(QEasingCurve::InOutQuad);
    anim->start(QAbstractAnimation::DeleteWhenStopped);
}
