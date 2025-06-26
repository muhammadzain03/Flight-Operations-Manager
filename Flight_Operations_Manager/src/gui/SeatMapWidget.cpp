// SeatMapWidget.cpp
// This file contains the implementation for the `SeatMapWidget` class.
// It provides the core logic for rendering and interacting with the graphical seat map,
// including loading the cabin image, defining clickable seat hotspots, highlighting
// occupied or selected seats, and handling user interactions.

#include "gui/SeatMapWidget.h"

#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QGraphicsRectItem>
#include <QGraphicsObject>
#include <QFile>
#include <QImageReader>
#include <QDebug>
#include <QBrush>
#include <QPen>
#include <QColor>
#include <QPainter>
#include <QEvent>
#include <QMouseEvent>
#include <QToolTip>
#include <QtMath>
#include <QTimer>
#include <QPainterPath>
#include <QLinearGradient>
#include <QScrollBar>
#include <QFontMetrics>
#include <QDateTime>
#include <QDebug>

SeatMapWidget::SeatMapWidget(QWidget* parent)
    : QGraphicsView(parent)
    , m_scene(new QGraphicsScene(this))
    , m_selectedSeatId()
    , m_hoveredSeatId()
{
    setScene(m_scene);
    setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform | QPainter::TextAntialiasing);
    setDragMode(QGraphicsView::NoDrag);
    setFrameStyle(QFrame::NoFrame);
    setAlignment(Qt::AlignTop | Qt::AlignHCenter);
    setBackgroundBrush(QColor("#2c3e50"));
    setMouseTracking(true);
    setViewportUpdateMode(FullViewportUpdate);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    
    setupClassColors();
    setSceneRect(QRectF(0, 0, 800, 2000));
    
    QFont font;
    font.setFamily("Segoe UI");
    font.setPointSize(9);
    setFont(font);
}

void SeatMapWidget::setupClassColors() {
    m_classColors.clear();
    m_classColors["First"] = QColor("#3498db");     // Royal Blue
    m_classColors["Business"] = QColor("#8e44ad");  // Wisteria Purple
    m_classColors["Premium"] = QColor("#16a085");   // Green Sea
    m_classColors["Economy"] = QColor("#f39c12");   // Orange
    m_classColors["Selected"] = QColor("#27ae60");  // Nephritis Green
    m_classColors["Occupied"] = QColor("#95a5a6");  // Concrete Gray
    m_classColors["Hover"] = QColor("#e74c3c");     // Pomegranate Red
    m_classColors["Text"] = QColor(255, 255, 255);
    m_classColors["TextDisabled"] = QColor("#7f8c8d");
}

void SeatMapWidget::setSeatLayout(const QMap<QString, SeatVisualInfo>& seatInfoMap) {
    m_seatInfoMap = seatInfoMap;

    if (!m_seatInfoMap.isEmpty()) {
        QRectF bounds;
        bool first = true;
        for (const auto& seat : m_seatInfoMap) {
            if (first) {
                bounds = seat.rect;
                first = false;
            } else {
                bounds = bounds.united(seat.rect);
            }
        }
        bounds.adjust(-48, -48, 48, 48);
        setSceneRect(bounds);
        centerOn(bounds.topLeft());
    }
    viewport()->update();
}

void SeatMapWidget::setOccupiedSeats(const QVector<QString>& occupied) {
    for (auto it = m_seatInfoMap.begin(); it != m_seatInfoMap.end(); ++it) {
        it.value().occupied = occupied.contains(it.key());
    }
    viewport()->update();
}

void SeatMapWidget::setSeatPrices(const QMap<QString, double>& prices) {
    // This function is now obsolete.
    (void)prices; // Mark as unused
}

void SeatMapWidget::setFlightInfo(const QString& flightNumber, const QDateTime& departureTime,
                               const QString& origin, const QString& destination) {
    // This function is now obsolete.
    (void)flightNumber;
    (void)departureTime;
    (void)origin;
    (void)destination;
}

void SeatMapWidget::mouseMoveEvent(QMouseEvent* event) {
    QPointF scenePos = mapToScene(event->pos());
    QString newHoveredSeatId = seatAt(scenePos);

    if (newHoveredSeatId != m_hoveredSeatId) {
        m_hoveredSeatId = newHoveredSeatId;
        viewport()->update();
    }

    if (!m_hoveredSeatId.isEmpty() && m_seatInfoMap.contains(m_hoveredSeatId)) {
        const auto& seat = m_seatInfoMap[m_hoveredSeatId];
        if (!seat.occupied) {
            setCursor(Qt::PointingHandCursor);
            QString tooltipText = QString("<b>Seat:</b> %1<br>"
                                          "<b>Class:</b> %2<br>"
                                          "<b>Price:</b> $%3")
                                          .arg(seat.seatId)
                                          .arg(seat.seatClass)
                                          .arg(QString::number(seat.price, 'f', 2));
            QToolTip::showText(event->globalPosition().toPoint(), tooltipText, this);
        } else {
            setCursor(Qt::ForbiddenCursor);
            QToolTip::hideText();
        }
    } else {
        unsetCursor();
        QToolTip::hideText();
    }

    QGraphicsView::mouseMoveEvent(event);
}

void SeatMapWidget::leaveEvent(QEvent* event) {
    if (!m_hoveredSeatId.isEmpty()) {
        m_hoveredSeatId.clear();
        unsetCursor();
        QToolTip::hideText();
        viewport()->update();
    }
    QGraphicsView::leaveEvent(event);
}

void SeatMapWidget::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        QPointF scenePos = mapToScene(event->pos());
        QString seatId = seatAt(scenePos);
        if (!seatId.isEmpty() && m_seatInfoMap.contains(seatId)) {
            const auto& seat = m_seatInfoMap[seatId];
            if (!seat.occupied) {
                m_selectedSeatId = seatId;
                emit seatSelected(seatId, seat.price, seat.seatClass);
                viewport()->update();
            }
        }
    }
    QGraphicsView::mousePressEvent(event);
}

void SeatMapWidget::drawForeground(QPainter* painter, const QRectF& rect) {
    // This is for drawing scene content that should scroll and zoom with the scene
    painter->setRenderHint(QPainter::Antialiasing);
    for (const auto& seatInfo : m_seatInfoMap) {
        if (seatInfo.rect.intersects(rect.toRect())) {
            drawSeat(painter, seatInfo, seatInfo.seatId == m_selectedSeatId, seatInfo.seatId == m_hoveredSeatId);
        }
    }
}

void SeatMapWidget::paintEvent(QPaintEvent* event) {
    // First, run the base class paint event to draw the scene items
    QGraphicsView::paintEvent(event);

    // Now, paint the legend on top of the viewport, so it doesn't scroll
    QPainter painter(this->viewport());
    drawLegend(&painter, 20, 15);
}

void SeatMapWidget::drawSeat(QPainter* painter, const SeatVisualInfo& seat, bool isSelected, bool isHovered) {
    if (seat.isAisle) return;

    painter->save();
    painter->setRenderHint(QPainter::Antialiasing, true);

    QColor baseColor = m_classColors.value(seat.seatClass, QColor(200, 200, 200));
    QColor seatColor = baseColor;
    QColor borderColor = baseColor.darker(120);
    QColor textColor = m_classColors["Text"];

    if (seat.occupied) {
        seatColor = m_classColors["Occupied"];
        borderColor = seatColor.darker(110);
        textColor = m_classColors["TextDisabled"];
    } else if (isSelected) {
        seatColor = m_classColors["Selected"];
        borderColor = seatColor.darker(120);
    }
    
    QPainterPath path;
    path.addRoundedRect(seat.rect, 8, 8);
    
    if (isHovered && !seat.occupied) {
        // A more elegant hover: a bright, accent-colored border
        painter->setPen(QPen(QColor("#f39c12"), 2.5)); // Orange accent border
        painter->drawPath(path);
    }
    
    QLinearGradient gradient(seat.rect.topLeft(), seat.rect.bottomLeft());
    gradient.setColorAt(0, seatColor.lighter(115));
    gradient.setColorAt(1, seatColor.darker(110));
    painter->fillPath(path, gradient);

    QPen pen(borderColor, isSelected ? 2 : 1.5);
    painter->setPen(pen);
    painter->drawPath(path);

    QFont seatFont("Segoe UI", 8, QFont::Bold);
    painter->setFont(seatFont);
    painter->setPen(textColor);
    painter->drawText(seat.rect, Qt::AlignCenter, seat.seatId);
    
    painter->restore();
}

QString SeatMapWidget::seatAt(const QPointF& scenePos) const {
    for (auto it = m_seatInfoMap.constBegin(); it != m_seatInfoMap.constEnd(); ++it) {
        if (it.value().rect.contains(scenePos.toPoint())) {
            return it.key();
        }
    }
    return QString();
}

void SeatMapWidget::selectSeat(const QString& seatId) {
    if (m_seatInfoMap.contains(seatId) || seatId.isEmpty()) {
        m_selectedSeatId = seatId;
        viewport()->update();
    }
}

void SeatMapWidget::clearSelection() {
    m_selectedSeatId.clear();
    viewport()->update();
}

QString SeatMapWidget::getSelectedSeat() const {
    return m_selectedSeatId;
}

void SeatMapWidget::wheelEvent(QWheelEvent *event)
{
    verticalScrollBar()->setValue(verticalScrollBar()->value() - event->angleDelta().y());
}

void SeatMapWidget::drawLegend(QPainter* painter, int x, int y) {
    painter->save();
    painter->setRenderHint(QPainter::Antialiasing);

    QVector<QPair<QString, QString>> legendItems;
    legendItems << QPair<QString, QString>("First", "First Class")
                << QPair<QString, QString>("Business", "Business")
                << QPair<QString, QString>("Premium", "Premium")
                << QPair<QString, QString>("Economy", "Economy")
                << QPair<QString, QString>("Selected", "Selected")
                << QPair<QString, QString>("Occupied", "Occupied");

    QFont legendFont("Segoe UI", 10, QFont::DemiBold);
    painter->setFont(legendFont);

    const int rectSize = 15;
    const int spacing = 12;
    const int textPadding = 6;
    const int itemVPadding = 6;
    const int itemHPadding = 12;
    
    int totalWidth = 0;
    for (const auto& item : legendItems) {
        totalWidth += rectSize + textPadding + QFontMetrics(legendFont).horizontalAdvance(item.second) + spacing;
    }
    totalWidth += itemHPadding * 2 - spacing;

    QRectF legendBgRect(x - itemHPadding, y - itemVPadding, totalWidth, rectSize + itemVPadding * 2);
    painter->setBrush(QColor(0, 0, 0, 180));
    painter->setPen(Qt::NoPen);
    painter->drawRoundedRect(legendBgRect, 8, 8);

    int currentX = x;
    painter->setPen(Qt::white);
    for (const auto& item : legendItems) {
        QColor color = m_classColors.value(item.first);

        painter->setBrush(color);
        painter->setPen(Qt::NoPen);
        painter->drawRoundedRect(currentX, y, rectSize, rectSize, 4, 4);

        painter->setPen(Qt::white);
        QString text = item.second;
        painter->drawText(currentX + rectSize + textPadding, y + rectSize - 2, text);
        
        currentX += rectSize + textPadding + QFontMetrics(legendFont).horizontalAdvance(text) + spacing;
    }

    painter->restore();
}
