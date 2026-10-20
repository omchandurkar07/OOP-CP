#ifndef ELECTRICITY_BILL_HPP
#define ELECTRICITY_BILL_HPP

#include <string>

class ElectricityBill {
private:
    float units = 0.0f;
    float amount = 0.0f;
    std::string monthYear; // e.g. "July 2026"

public:
    ElectricityBill() = default;
    ElectricityBill(float u, const std::string& my) : units(u), monthYear(my) {
        calculateBill();
    }

    void calculateBill();
    void display() const;
    float getAmount() const { return amount; }
    const std::string& getMonthYear() const { return monthYear; }
    float getUnits() const { return units; }
};

#endif // ELECTRICITY_BILL_HPP
