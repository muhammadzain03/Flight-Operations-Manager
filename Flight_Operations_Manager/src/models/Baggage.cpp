// This file contains the implementation for the `Baggage` class,
// which represents baggage details associated with flights or passengers
// in the Flight Operations Manager application. It provides methods for
// managing baggage attributes such as weight, dimensions, and type.

#include "models/Baggage.h"
#include <algorithm>

Baggage::Baggage()
    : totalWeight(0.0)
    , nextTagNumber(1)
{
}

QString Baggage::checkBag(double weight, const QString& description, bool isFragile) {
    QString tagNumber = generateTagNumber();
    
    BaggageTag bag{
        tagNumber,
        weight,
        description,
        isFragile,
        weight > 23.0,  // Standard oversize weight limit
        Status::Checked,
        "Check-in",
        QDateTime::currentDateTime()
    };
    
    bags.push_back(bag);
    updateTotalWeight();
    return tagNumber;
}

void Baggage::updateStatus(const QString& tagNumber, Status newStatus, const QString& location) {
    auto it = std::find_if(bags.begin(), bags.end(),
                          [&](const BaggageTag& bag) {
                              return bag.tagNumber == tagNumber;
                          });
    
    if (it != bags.end()) {
        it->status = newStatus;
        it->currentLocation = location;
        it->lastUpdated = QDateTime::currentDateTime();
    }
}

void Baggage::markAsLost(const QString& tagNumber) {
    updateStatus(tagNumber, Status::Lost, "Unknown");
}

void Baggage::markAsDamaged(const QString& tagNumber) {
    updateStatus(tagNumber, Status::Damaged, "Baggage Claim");
}

void Baggage::claim(const QString& tagNumber) {
    updateStatus(tagNumber, Status::Claimed, "Claimed by passenger");
}

const Baggage::BaggageTag* Baggage::getBaggage(const QString& tagNumber) const {
    auto it = std::find_if(bags.begin(), bags.end(),
                          [&](const BaggageTag& bag) {
                              return bag.tagNumber == tagNumber;
                          });
    
    return it != bags.end() ? &(*it) : nullptr;
}

std::vector<Baggage::BaggageTag> Baggage::getLostBaggage() const {
    std::vector<BaggageTag> lostBags;
    std::copy_if(bags.begin(), bags.end(), std::back_inserter(lostBags),
                 [](const BaggageTag& bag) {
                     return bag.status == Status::Lost;
                 });
    return lostBags;
}

std::vector<Baggage::BaggageTag> Baggage::getDamagedBaggage() const {
    std::vector<BaggageTag> damagedBags;
    std::copy_if(bags.begin(), bags.end(), std::back_inserter(damagedBags),
                 [](const BaggageTag& bag) {
                     return bag.status == Status::Damaged;
                 });
    return damagedBags;
}

bool Baggage::hasBaggage(const QString& tagNumber) const {
    return std::any_of(bags.begin(), bags.end(),
                      [&](const BaggageTag& bag) {
                          return bag.tagNumber == tagNumber;
                      });
}

QString Baggage::generateTagNumber() {
    return QString("BAG%1").arg(nextTagNumber++, 6, 10, QChar('0'));
}

void Baggage::updateTotalWeight() {
    totalWeight = std::accumulate(bags.begin(), bags.end(), 0.0,
                                [](double sum, const BaggageTag& bag) {
                                    return sum + bag.weight;
                                });
} 