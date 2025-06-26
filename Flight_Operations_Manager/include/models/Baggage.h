// This header defines the `Baggage` class, which represents baggage information
// associated with a passenger or flight in the Flight Operations Manager application.
// It includes details such as weight, dimensions, and special handling requirements.

#ifndef BAGGAGE_H
#define BAGGAGE_H

#include <QtCore/QString>
#include <QtCore/QDateTime>
#include <vector>

class Baggage {
public:
    enum class Status {
        Checked,
        InTransit,
        Arrived,
        Claimed,
        Lost,
        Damaged
    };

    struct BaggageTag {
        QString tagNumber;
        double weight;
        QString description;
        bool isFragile;
        bool isOversize;
        Status status;
        QString currentLocation;
        QDateTime lastUpdated;
    };

    Baggage();

    // Baggage management
    QString checkBag(double weight, const QString& description, bool isFragile = false);
    void updateStatus(const QString& tagNumber, Status newStatus, const QString& location);
    void markAsLost(const QString& tagNumber);
    void markAsDamaged(const QString& tagNumber);
    void claim(const QString& tagNumber);

    // Getters
    const std::vector<BaggageTag>& getAllBaggage() const { return bags; }
    const BaggageTag* getBaggage(const QString& tagNumber) const;
    double getTotalWeight() const { return totalWeight; }
    int getCount() const { return bags.size(); }

    // Queries
    std::vector<BaggageTag> getLostBaggage() const;
    std::vector<BaggageTag> getDamagedBaggage() const;
    bool hasBaggage(const QString& tagNumber) const;

private:
    std::vector<BaggageTag> bags;
    double totalWeight;
    int nextTagNumber;

    QString generateTagNumber();
    void updateTotalWeight();
};

#endif // BAGGAGE_H 