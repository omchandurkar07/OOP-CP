#include "Dashboard.hpp"
#include <iostream>
#include <iomanip>

void Dashboard::showSummary(const std::vector<Customer*>& customers,
                            const std::vector<ElectricityBill*>& bills) {
    size_t totalCustomers = customers.size();
    float totalUnits = 0.0f;
    float totalRevenue = 0.0f;
    size_t paidBills = 0; // For simplicity, treat all generated bills as paid
    for (const auto* bill : bills) {
        totalUnits += bill->getUnits();
        totalRevenue += bill->getAmount();
        ++paidBills;
    }
    size_t unpaidBills = bills.size() - paidBills;
    std::cout << "\n=== DASHBOARD SUMMARY ===\n"
              << "Customers       : " << totalCustomers << "\n"
              << "Total Units     : " << totalUnits << " kWh\n"
              << "Total Revenue   : ₹" << totalRevenue << "\n"
              << "Paid Bills      : " << paidBills << "\n"
              << "Unpaid Bills    : " << unpaidBills << "\n";
}
