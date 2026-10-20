#ifndef METER_CPP
#define METER_CPP

#include "Meter.hpp"
#include <iostream>
#include <limits>

void Meter::enterReading() {
    std::cout << "Enter previous reading (kWh): ";
    std::cin >> previousReading;
    std::cout << "Enter current reading (kWh): ";
    std::cin >> currentReading;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    if (currentReading < previousReading) {
        std::cerr << "Error: current reading < previous reading!\n";
    }
}

float Meter::calculateUnits() const {
    if (currentReading < previousReading) return 0.0f;
    return currentReading - previousReading;
}

#endif // METER_CPP
