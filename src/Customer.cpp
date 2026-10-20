#include "Customer.hpp"

static const std::string CUSTOMER_DB = "data/customers.txt";

Customer::Customer(int _id, const std::string& _name,
                   const std::string& _addr,
                   const std::string& _meter,
                   const std::string& _type)
    : id(_id), name(_name), address(_addr),
      meterNumber(_meter), type(_type) {}

void Customer::addCustomer() const {
    std::ofstream out(CUSTOMER_DB, std::ios::app);
    if (!out) { std::cerr << "Cannot open DB.\n"; return; }
    out << id << '|' << name << '|' << address << '|' << meterNumber << '|' << type << '\n';
    std::cout << "Customer added successfully.\n";
}

void Customer::display() const {
    std::cout << "ID: " << id << "\n"
              << "Name: " << name << "\n"
              << "Address: " << address << "\n"
              << "Meter #: " << meterNumber << "\n"
              << "Type: " << type << "\n";
}

void Customer::update() {
    std::fstream file(CUSTOMER_DB, std::ios::in | std::ios::out);
    if (!file) { std::cerr << "DB error.\n"; return; }
    std::string line, newContent;
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string token; std::getline(ss, token, '|');
        int curId = std::stoi(token);
        if (curId == id) {
            std::cout << "Enter new name: "; std::getline(std::cin, name);
            std::cout << "Enter new address: "; std::getline(std::cin, address);
            std::cout << "Enter new meter #: "; std::getline(std::cin, meterNumber);
            std::cout << "Enter new type: "; std::getline(std::cin, type);
            newContent += std::to_string(id) + '|' + name + '|' + address + '|' + meterNumber + '|' + type + '\n';
        } else {
            newContent += line + '\n';
        }
    }
    file.close();
    std::ofstream out(CUSTOMER_DB, std::ios::trunc);
    out << newContent;
    std::cout << "Customer updated.\n";
}

Customer* Customer::search(int searchId) {
    std::ifstream in(CUSTOMER_DB);
    if (!in) return nullptr;
    std::string line;
    while (std::getline(in, line)) {
        std::stringstream ss(line);
        std::string token; std::getline(ss, token, '|');
        int curId = std::stoi(token);
        if (curId == searchId) {
            std::string name, addr, meter, type;
            std::getline(ss, name, '|');
            std::getline(ss, addr, '|');
            std::getline(ss, meter, '|');
            std::getline(ss, type, '|');
            return new Customer(curId, name, addr, meter, type);
        }
    }
    return nullptr;
}

void Customer::listAll() {
    std::ifstream in(CUSTOMER_DB);
    if (!in) { std::cerr << "DB missing.\n"; return; }
    std::string line;
    std::cout << "=== All Customers ===\n";
    while (std::getline(in, line)) {
        std::stringstream ss(line);
        std::string token;
        std::getline(ss, token, '|'); std::cout << "ID: " << token << " | ";
        std::getline(ss, token, '|'); std::cout << "Name: " << token << " | ";
        std::getline(ss, token, '|'); std::cout << "Meter: " << token << "\n";
    }
}
