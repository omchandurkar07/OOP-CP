#ifndef DASHBOARD_HPP
#define DASHBOARD_HPP

#include <vector>
#include "Customer.hpp"
#include "ElectricityBill.hpp"

class Dashboard {
public:
    static void showSummary(const std::vector<Customer*>& customers,
                            const std::vector<ElectricityBill*>& bills);
};

#endif // DASHBOARD_HPP
