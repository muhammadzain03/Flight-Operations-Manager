// SeatMapWidget.h
// This header defines the `SeatMapWidget` class, a custom QGraphicsView-based widget
// for displaying and interacting with a graphical seat map. It allows loading a cabin image,
// defining clickable seat hotspots, highlighting occupied seats, and selecting individual seats.

#ifndef SEATMAPWIDGET_H
#define SEATMAPWIDGET_H

#include <QGraphicsView>
#include <QMap>
#include <QString>
#include <QRect>
#include <QVector>
#include <QDateTime>
#include <QColor>
#include <QWheelEvent>
#include <QMouseEvent>

class QGraphicsScene;

struct SeatVisualInfo {
    QString seatId;
    QRect rect;
    QString seatClass; // "First", "Business", "Economy"
    double price;
    bool occupied = false;
    bool reserved = false;
    bool blocked = false;
    bool isAisle = false; // For aisle markers
    bool isExitRow = false;
    bool hasMeal = false;
    bool hasPower = false;
    bool isBassinet = false;
    bool isLavatory = false;
    
    bool isValid() const { return !seatId.isEmpty(); }
};

class SeatMapWidget : public QGraphicsView {
    Q_OBJECT

public:
    explicit SeatMapWidget(QWidget* parent = nullptr);
    ~SeatMapWidget() override = default;

    void setSeatLayout(const QMap<QString, SeatVisualInfo>& seatInfoMap);
    void setOccupiedSeats(const QVector<QString>& occupied);
    void setSeatPrices(const QMap<QString, double>& prices);
    void setFlightInfo(const QString& flightNumber, const QDateTime& departureTime, const QString& origin = "", const QString& destination = "");
    
    QString getSelectedSeat() const;
    void selectSeat(const QString& seatNumber);
    void clearSelection();

signals:
    void seatSelected(const QString& seatNumber, double price, const QString& seatClass);
    void seatHovered(const QString& seatNumber, double price, const QString& seatClass);

protected:
    void drawForeground(QPainter* painter, const QRectF& rect) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void leaveEvent(QEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;
    void paintEvent(QPaintEvent* event) override;

private:
    void drawSeat(QPainter* painter, const SeatVisualInfo& seatInfo, bool isSelected, bool isHovered);
    void setupClassColors();
    QString seatAt(const QPointF& scenePos) const;
    void drawLegend(QPainter* painter, int x, int y);

    QGraphicsScene* m_scene;
    QMap<QString, SeatVisualInfo> m_seatInfoMap;
    QMap<QString, QColor> m_classColors;
    
    // State
    QString m_selectedSeatId;
    QString m_hoveredSeatId;
};

#endif // SEATMAPWIDGET_H
