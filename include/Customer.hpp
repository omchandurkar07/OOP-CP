#ifndef CUSTOMER_HPP
#define CUSTOMER_HPP

#include <string>
#include <fstream>
#include <iostream>

class Customer {
protected:
    int id;
    std::string name;
    std::string address;
    std::string meterNumber;
    std::string type; // "Residential" or "Commercial"

public:
    Customer() = default;
    Customer(int _id, const std::string& _name,
             const std::string& _addr,
             const std::string& _meter,
             const std::string& _type);

    // CRUD operations
    void addCustomer() const;
    void display() const;
    void update();
    static Customer* search(int searchId);
    static void listAll();

    // Getters
    int getId() const { return id; }
    const std::string& getMeter() const { return meterNumber; }
};

#endif // CUSTOMER_HPP
