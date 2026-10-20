#ifndef ELECTRICITY_BILL_CPP
#define ELECTRICITY_BILL_CPP

#include "ElectricityBill.hpp"
#include <iostream>

void ElectricityBill::calculateBill() {
    // Simple slab rates:
    // 0-100 : 5 per unit
    // 101-300 : 7 per unit
    // >300 : 10 per unit
    if (units <= 100) amount = units * 5;
    else if (units <= 300) amount = 100 * 5 + (units - 100) * 7;
    else amount = 100 * 5 + 200 * 7 + (units - 300) * 10;

    // Add 5% tax/charge
    amount *= 1.05f;
}

void ElectricityBill::display() const {
    std::cout << "=== Bill for " << monthYear << " ===\n"
              << "Units Consumed : " << units << " kWh\n"
              << "Amount Payable : ₹" << amount << "\n";
}

#endif // ELECTRICITY_BILL_CPP
