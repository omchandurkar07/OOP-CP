#include <iostream>
#include <vector>
#include <memory>
#include <string>
#include <limits>
#include <iomanip>
#include <algorithm>
#include <cmath>
#include <fstream>
#include <sstream>
#include <cctype>
#include <stdexcept>
#include <cstdlib>

using namespace std;

// ============================================================
// INPUT VALIDATOR UTILITY CLASS
// Ensures all inputs are robust, validated, and crash-proof.
// ============================================================

class InputValidator {
public:
    // Trims leading and trailing whitespace
    static string trim(const string& str) {
        size_t first = str.find_first_not_of(" \t\r\n");
        if (first == string::npos) return "";
        size_t last = str.find_last_not_of(" \t\r\n");
        return str.substr(first, (last - first + 1));
    }

    // Converts a string to lowercase for case-insensitive comparisons
    static string toLower(string s) {
        transform(s.begin(), s.end(), s.begin(), [](unsigned char c) {
            return static_cast<char>(tolower(c));
        });
        return s;
    }

    // Safely reads and validates an integer within [minVal, maxVal]
    static int getValidInt(const string& prompt, int minVal = 1, int maxVal = numeric_limits<int>::max()) {
        int value;
        while (true) {
            cout << prompt;
            string line;
            if (!getline(cin, line)) {
                cin.clear();
                continue;
            }
            line = trim(line);
            if (line.empty()) {
                cout << "  [Error] Input cannot be empty. Please enter a valid number.\n";
                continue;
            }

            stringstream ss(line);
            if (ss >> value) {
                string extra;
                if (!(ss >> extra)) {
                    if (value >= minVal && value <= maxVal) {
                        return value;
                    }
                    if (maxVal >= 1000000000) {
                        cout << "  [Error] Value must be at least " << minVal << ". Try again.\n";
                    } else {
                        cout << "  [Error] Value must be between " << minVal << " and " << maxVal << ". Try again.\n";
                    }
                } else {
                    cout << "  [Error] Invalid characters detected. Please enter whole numbers only.\n";
                }
            } else {
                cout << "  [Error] Invalid input. Please enter a valid integer.\n";
            }
        }
    }

    // Safely reads and validates a floating-point double within [minVal, maxVal]
    static double getValidDouble(const string& prompt, double minVal = 0.0, double maxVal = numeric_limits<double>::max()) {
        double value;
        while (true) {
            cout << prompt;
            string line;
            if (!getline(cin, line)) {
                cin.clear();
                continue;
            }
            line = trim(line);
            if (line.empty()) {
                cout << "  [Error] Input cannot be empty. Please enter a valid decimal number.\n";
                continue;
            }

            stringstream ss(line);
            if (ss >> value) {
                string extra;
                if (!(ss >> extra)) {
                    if (value >= minVal && value <= maxVal) {
                        return value;
                    }
                    cout << fixed << setprecision(2);
                    if (maxVal >= 1e9) {
                        cout << "  [Error] Value must be at least " << minVal << ". Try again.\n";
                    } else {
                        cout << "  [Error] Value must be between " << minVal << " and " << maxVal << ". Try again.\n";
                    }
                } else {
                    cout << "  [Error] Invalid numeric format. Please enter numbers only.\n";
                }
            } else {
                cout << "  [Error] Invalid input. Please enter a valid decimal number.\n";
            }
        }
    }

    // Safely reads a non-empty string without '|' (to avoid corrupting file persistence)
    static string getValidString(const string& prompt, const string& fieldName = "Field", size_t minLen = 1) {
        while (true) {
            cout << prompt;
            string line;
            if (!getline(cin, line)) {
                cin.clear();
                continue;
            }
            line = trim(line);

            if (line.empty()) {
                cout << "  [Error] " << fieldName << " cannot be empty. Please try again.\n";
                continue;
            }
            if (line.length() < minLen) {
                cout << "  [Error] " << fieldName << " must be at least " << minLen << " characters long.\n";
                continue;
            }
            if (line.find('|') != string::npos) {
                cout << "  [Error] " << fieldName << " cannot contain the '|' delimiter character.\n";
                continue;
            }
            return line;
        }
    }

    // Validates person name (must have alphabetic characters)
    static string getValidName(const string& prompt) {
        while (true) {
            string name = getValidString(prompt, "Name", 2);
            bool hasAlpha = false;
            for (char c : name) {
                if (isalpha(static_cast<unsigned char>(c))) {
                    hasAlpha = true;
                    break;
                }
            }
            if (!hasAlpha) {
                cout << "  [Error] Name must contain at least one alphabetical letter.\n";
                continue;
            }
            return name;
        }
    }

    // Validates address (minimum length 3, non-empty, no '|')
    static string getValidAddress(const string& prompt) {
        return getValidString(prompt, "Address", 3);
    }

    // Validates meter number (no spaces, non-empty, alphanumeric, no '|')
    static string getValidMeterNumber(const string& prompt) {
        while (true) {
            string meter = getValidString(prompt, "Meter Number", 2);
            if (meter.find(' ') != string::npos) {
                cout << "  [Error] Meter Number cannot contain spaces. Use format like 'MTR-101' or 'M101'.\n";
                continue;
            }
            return meter;
        }
    }

    // Validates Customer Type: strictly Residential or Commercial
    static string getValidCustomerType(const string& prompt) {
        while (true) {
            cout << prompt;
            string line;
            if (!getline(cin, line)) {
                cin.clear();
                continue;
            }
            line = trim(line);
            string lower = toLower(line);

            if (lower == "residential" || lower == "res" || lower == "r" || lower == "1") {
                return "Residential";
            }
            if (lower == "commercial" || lower == "com" || lower == "c" || lower == "2") {
                return "Commercial";
            }

            cout << "  [Error] Invalid Customer Type! Must be 'Residential' or 'Commercial'.\n";
        }
    }

    // Validates Month and Year (e.g. 'July 2026')
    static string getValidMonthYear(const string& prompt) {
        return getValidString(prompt, "Month (e.g. July 2026)", 3);
    }

    // Validates Yes/No confirmation
    static bool getConfirm(const string& prompt) {
        while (true) {
            cout << prompt;
            string line;
            if (!getline(cin, line)) {
                cin.clear();
                continue;
            }
            line = trim(line);
            if (!line.empty()) {
                char c = static_cast<char>(tolower(line[0]));
                if (c == 'y') return true;
                if (c == 'n') return false;
            }
            cout << "  [Error] Please answer 'Y' for Yes or 'N' for No.\n";
        }
    }
};

// ============================================================
// USER BASE CLASS - Demonstrates Inheritance & Abstraction
// ============================================================

class User {
protected:
    string name;

public:
    User(string n = "") : name(n) {}

    virtual void displayRole() = 0; // Pure virtual function for Abstraction

    string getName() const {
        return name;
    }

    virtual ~User() = default;
};

// ============================================================
// ADMIN CLASS
// ============================================================

class Admin : public User {
private:
    string username;
    string password;

public:
    Admin()
        : User("Administrator"),
          username("admin"),
          password("1234") {}

    bool login() {
        int attempts = 3;

        while (attempts > 0) {
            cout << "\n=====================================\n";
            cout << "       SMART ELECTRICITY SYSTEM\n";
            cout << "              ADMIN LOGIN\n";
            cout << "=====================================\n";

            cout << "Username: ";
            string user;
            if (!getline(cin, user)) { cin.clear(); continue; }
            user = InputValidator::trim(user);

            cout << "Password: ";
            string pass;
            if (!getline(cin, pass)) { cin.clear(); continue; }
            pass = InputValidator::trim(pass);

            if (user == username && pass == password) {
                cout << "\n[OK] Login successful! Welcome, Administrator.\n";
                return true;
            }

            attempts--;
            if (attempts > 0) {
                cout << "\n[ALERT] Invalid credentials! " << attempts << " attempt(s) remaining.\n";
            }
        }

        cout << "\n[ALERT] Maximum login attempts exceeded. Access denied.\n";
        return false;
    }

    void displayRole() override {
        cout << "Role: Administrator\n";
    }
};

// ============================================================
// CUSTOMER CLASS
// ============================================================

class Customer : public User {
private:
    int id;
    string address;
    string meterNumber;
    string type;

public:
    Customer(
        int i,
        string n,
        string addr,
        string meter,
        string customerType
    )
        : User(n),
          id(i),
          address(addr),
          meterNumber(meter),
          type(customerType) {}

    int getId() const {
        return id;
    }

    string getMeterNumber() const {
        return meterNumber;
    }

    string getType() const {
        return type;
    }

    string getAddress() const {
        return address;
    }

    void setName(const string& n) { name = n; }
    void setAddress(const string& addr) { address = addr; }
    void setMeterNumber(const string& meter) { meterNumber = meter; }
    void setType(const string& customerType) { type = customerType; }

    void displayRole() override {
        cout << "Role: Customer\n";
    }

    void display() const {
        cout << "\n-------------------------------------\n";
        cout << "Customer ID   : " << id << endl;
        cout << "Name          : " << name << endl;
        cout << "Address       : " << address << endl;
        cout << "Meter Number  : " << meterNumber << endl;
        cout << "Customer Type : " << type << endl;
        cout << "-------------------------------------\n";
    }

    // Robust update with duplicate checks against existing customers
    void update(const vector<unique_ptr<Customer>>& allCustomers) {
        cout << "\n========== UPDATE CUSTOMER (ID: " << id << ") ==========\n";
        cout << "Current Name        : " << name << endl;
        cout << "Current Address     : " << address << endl;
        cout << "Current Meter Number: " << meterNumber << endl;
        cout << "Current Type        : " << type << endl;
        cout << "---------------------------------------------------------\n";
        cout << "(Press ENTER on any field to keep current value)\n\n";

        // Update Name
        cout << "Enter new Name [" << name << "]: ";
        string newName;
        getline(cin, newName);
        newName = InputValidator::trim(newName);
        if (!newName.empty()) {
            if (newName.find('|') != string::npos) {
                cout << "  [Notice] Name cannot contain '|'. Retaining current name.\n";
            } else {
                name = newName;
                cout << "  [Updated] Name set to: " << name << endl;
            }
        }

        // Update Address
        cout << "Enter new Address [" << address << "]: ";
        string newAddr;
        getline(cin, newAddr);
        newAddr = InputValidator::trim(newAddr);
        if (!newAddr.empty()) {
            if (newAddr.find('|') != string::npos) {
                cout << "  [Notice] Address cannot contain '|'. Retaining current address.\n";
            } else {
                address = newAddr;
                cout << "  [Updated] Address set to: " << address << endl;
            }
        }

        // Update Meter Number with uniqueness check
        cout << "Enter new Meter Number [" << meterNumber << "]: ";
        string newMeter;
        getline(cin, newMeter);
        newMeter = InputValidator::trim(newMeter);
        if (!newMeter.empty() && InputValidator::toLower(newMeter) != InputValidator::toLower(meterNumber)) {
            if (newMeter.find(' ') != string::npos || newMeter.find('|') != string::npos) {
                cout << "  [Error] Meter number cannot contain spaces or '|'. Retaining current meter number.\n";
            } else {
                bool exists = false;
                for (const auto& c : allCustomers) {
                    if (c->getId() != id &&
                        InputValidator::toLower(c->getMeterNumber()) == InputValidator::toLower(newMeter)) {
                        cout << "  [Error] Meter Number '" << newMeter << "' is already in use by Customer ID "
                             << c->getId() << " (" << c->getName() << ")! Retaining current meter.\n";
                        exists = true;
                        break;
                    }
                }
                if (!exists) {
                    meterNumber = newMeter;
                    cout << "  [Updated] Meter Number set to: " << meterNumber << endl;
                }
            }
        }

        // Update Customer Type
        cout << "Enter new Type (Residential/Commercial) [" << type << "]: ";
        string newType;
        getline(cin, newType);
        newType = InputValidator::trim(newType);
        if (!newType.empty()) {
            string lower = InputValidator::toLower(newType);
            if (lower == "residential" || lower == "res" || lower == "r" || lower == "1") {
                type = "Residential";
                cout << "  [Updated] Customer Type set to: Residential\n";
            } else if (lower == "commercial" || lower == "com" || lower == "c" || lower == "2") {
                type = "Commercial";
                cout << "  [Updated] Customer Type set to: Commercial\n";
            } else {
                cout << "  [Notice] Invalid type entered. Retaining '" << type << "'.\n";
            }
        }

        cout << "\n[OK] Customer details successfully saved!\n";
    }
};

// ============================================================
// METER CLASS - Validated Readings
// ============================================================

class Meter {
private:
    double previousReading;
    double currentReading;

public:
    Meter() : previousReading(0.0), currentReading(0.0) {}
    Meter(double previous, double current)
        : previousReading(previous), currentReading(current) {}

    void enterReading() {
        cout << "\n========== METER READING ==========\n";

        previousReading = InputValidator::getValidDouble("Enter previous reading (>= 0): ", 0.0);

        while (true) {
            currentReading = InputValidator::getValidDouble("Enter current reading: ", 0.0);

            if (currentReading >= previousReading) {
                break;
            }

            cout << "\n  [Error] Current reading (" << fixed << setprecision(2) << currentReading
                 << ") cannot be less than previous reading (" << previousReading << ")!\n";
            cout << "  Please re-enter the current reading.\n\n";
        }
    }

    double calculateUnits() const {
        return currentReading - previousReading;
    }

    double getPreviousReading() const {
        return previousReading;
    }

    double getCurrentReading() const {
        return currentReading;
    }
};

// ============================================================
// ABSTRACT BILL CLASS - Abstraction
// ============================================================

class Bill {
public:
    virtual double calculateBill() = 0;
    virtual void display() const = 0;
    virtual ~Bill() = default;
};

// ============================================================
// ELECTRICITY BILL CLASS - Demonstrates Polymorphism
// ============================================================

class ElectricityBill : public Bill {
private:
    int customerId;
    string customerName;
    string meterNumber;
    string customerType;

    string monthYear;

    double previousReading;
    double currentReading;
    double units;
    double billAmount;

    string paymentStatus;

public:
    ElectricityBill(
        int id,
        string name,
        string meter,
        string type,
        string month,
        double previous,
        double current
    )
        : customerId(id),
          customerName(name),
          meterNumber(meter),
          customerType(type),
          monthYear(month),
          previousReading(previous),
          currentReading(current),
          paymentStatus("UNPAID")
    {
        units = max(0.0, currentReading - previousReading);
        billAmount = calculateBill();
    }

    // Polymorphic billing function — flat rate: ₹10 per unit
    double calculateBill() override {
        return units * 10.0;
    }

    void display() const override {
        cout << fixed << setprecision(2);

        cout << "\n=====================================\n";
        cout << "          ELECTRICITY BILL\n";
        cout << "=====================================\n";

        cout << "Customer ID      : " << customerId << endl;
        cout << "Customer Name    : " << customerName << endl;
        cout << "Meter Number     : " << meterNumber << endl;
        cout << "Customer Type    : " << customerType << endl;
        cout << "Month            : " << monthYear << endl;
        cout << "Previous Reading : " << previousReading << " kWh" << endl;
        cout << "Current Reading  : " << currentReading  << " kWh" << endl;
        cout << "Units Consumed   : " << units           << " kWh" << endl;
        cout << "Bill Amount      : ₹" << billAmount << endl;
        cout << "Payment Status   : " << paymentStatus << endl;

        cout << "=====================================\n";
    }

    int getCustomerId() const {
        return customerId;
    }

    string getMeterNumber() const {
        return meterNumber;
    }

    string getMonthYear() const {
        return monthYear;
    }

    double getUnits() const {
        return units;
    }

    double getBillAmount() const {
        return billAmount;
    }

    string getPaymentStatus() const {
        return paymentStatus;
    }

    void makePayment() {
        paymentStatus = "PAID";
    }

    string getCustomerName()    const { return customerName; }
    string getCustomerType()    const { return customerType; }
    double getPreviousReading() const { return previousReading; }
    double getCurrentReading()  const { return currentReading; }
};

// ============================================================
// THEFT DETECTION CLASS
// ============================================================

class TheftDetection {
private:
    double expectedUnits;
    double actualUnits;

public:
    TheftDetection(double expected, double actual)
        : expectedUnits(expected),
          actualUnits(actual) {}

    bool isAnomaly() const {
        if (expectedUnits <= 0)
            return false;

        double difference = std::abs(expectedUnits - actualUnits);
        double percentage = (difference / expectedUnits) * 100.0;

        // More than 50% deviation indicates suspicious usage/theft
        return percentage >= 50.0;
    }

    void generateAlert() const {
        cout << fixed << setprecision(2);
        cout << "\n";
        cout << "****************************************\n";
        cout << "       ELECTRICITY ANOMALY ALERT!\n";
        cout << "****************************************\n";

        cout << "Expected Usage : " << expectedUnits << " units\n";
        cout << "Actual Usage   : " << actualUnits   << " units\n";

        double difference = abs(expectedUnits - actualUnits);
        double pct = (expectedUnits > 0) ? (difference / expectedUnits) * 100.0 : 0.0;

        cout << "Difference     : " << difference << " units (" << pct << "% deviation)\n";
        cout << "Status         : SUSPICIOUS USAGE DETECTED\n";
        cout << "Action         : On-site meter inspection & audit required.\n";

        cout << "****************************************\n";
    }
};

// ============================================================
// DASHBOARD CLASS
// ============================================================

class Dashboard {
public:
    static void showSummary(
        const vector<unique_ptr<Customer>>& customers,
        const vector<unique_ptr<ElectricityBill>>& bills
    ) {
        double totalUnits = 0;
        double totalRevenue = 0;
        int paidBills = 0;
        int unpaidBills = 0;

        for (const auto& bill : bills) {
            totalUnits += bill->getUnits();
            totalRevenue += bill->getBillAmount();

            if (bill->getPaymentStatus() == "PAID")
                paidBills++;
            else
                unpaidBills++;
        }

        cout << "\n";
        cout << "============================================\n";
        cout << "          SMART ELECTRICITY DASHBOARD\n";
        cout << "============================================\n";

        cout << "Total Customers : " << customers.size() << endl;
        cout << "Total Bills     : " << bills.size() << endl;
        cout << fixed << setprecision(2);
        cout << "Total Energy    : " << totalUnits << " kWh" << endl;
        cout << "Total Revenue   : ₹" << totalRevenue << endl;
        cout << "Paid Bills      : " << paidBills << endl;
        cout << "Unpaid Bills    : " << unpaidBills << endl;

        cout << "============================================\n";
    }
};

// ============================================================
// SEARCH CUSTOMER HELPER
// ============================================================

Customer* searchCustomer(
    vector<unique_ptr<Customer>>& customers,
    int id
) {
    for (auto& customer : customers) {
        if (customer->getId() == id)
            return customer.get();
    }
    return nullptr;
}

const Customer* searchCustomerConst(
    const vector<unique_ptr<Customer>>& customers,
    int id
) {
    for (const auto& customer : customers) {
        if (customer->getId() == id)
            return customer.get();
    }
    return nullptr;
}

// ============================================================
// LIST CUSTOMERS
// ============================================================

void listCustomers(
    const vector<unique_ptr<Customer>>& customers
) {
    if (customers.empty()) {
        cout << "\n[Info] No customers currently registered.\n";
        return;
    }

    cout << "\n========== ALL REGISTERED CUSTOMERS (" << customers.size() << ") ==========\n";
    for (const auto& customer : customers) {
        customer->display();
    }
}

// ============================================================
// SEARCH CUSTOMER MENU
// ============================================================

void searchCustomerMenu(
    vector<unique_ptr<Customer>>& customers
) {
    if (customers.empty()) {
        cout << "\n[Info] No customers currently in database.\n";
        return;
    }

    int id = InputValidator::getValidInt("\nEnter Customer ID to search: ", 1);

    Customer* customer = searchCustomer(customers, id);
    if (customer) {
        customer->display();
    } else {
        cout << "\n[ALERT] Customer with ID " << id << " not found.\n";
    }
}

// ============================================================
// SEARCH MONTHLY BILL
// ============================================================

void searchBill(
    const vector<unique_ptr<ElectricityBill>>& bills
) {
    if (bills.empty()) {
        cout << "\n[Info] No bills generated yet.\n";
        return;
    }

    cout << "\n========== SEARCH MONTHLY BILL ==========\n";
    string meter = InputValidator::getValidMeterNumber("Enter Meter Number: ");
    string month = InputValidator::getValidMonthYear("Enter Month (e.g. July 2026): ");

    bool found = false;
    for (const auto& bill : bills) {
        if (InputValidator::toLower(bill->getMeterNumber()) == InputValidator::toLower(meter) &&
            InputValidator::toLower(bill->getMonthYear()) == InputValidator::toLower(month)) {
            bill->display();
            found = true;
            break;
        }
    }

    if (!found) {
        cout << "\n[ALERT] No bill found for:\n";
        cout << "  Meter : " << meter << endl;
        cout << "  Month : " << month << endl;
    }
}

// ============================================================
// BILL HISTORY
// ============================================================

void showBillHistory(
    const vector<unique_ptr<ElectricityBill>>& bills
) {
    if (bills.empty()) {
        cout << "\n[Info] No bills generated yet.\n";
        return;
    }

    string meter = InputValidator::getValidMeterNumber("\nEnter Meter Number for History: ");

    bool found = false;
    cout << "\n========== BILL HISTORY FOR METER '" << meter << "' ==========\n";

    for (const auto& bill : bills) {
        if (InputValidator::toLower(bill->getMeterNumber()) == InputValidator::toLower(meter)) {
            bill->display();
            found = true;
        }
    }

    if (!found) {
        cout << "No billing records found for Meter Number: " << meter << endl;
    }
}

// ============================================================
// PAYMENT
// ============================================================

bool saveBills(const vector<unique_ptr<ElectricityBill>>& bills);
void syncFrontendDatabase(
    const vector<unique_ptr<Customer>>& customers,
    const vector<unique_ptr<ElectricityBill>>& bills
);

void makeBillPayment(
    vector<unique_ptr<ElectricityBill>>& bills,
    const vector<unique_ptr<Customer>>& customers
) {
    if (bills.empty()) {
        cout << "\n[Info] No bills available for payment.\n";
        return;
    }

    cout << "\n========== BILL PAYMENT ==========\n";
    string meter = InputValidator::getValidMeterNumber("Enter Meter Number: ");
    string month = InputValidator::getValidMonthYear("Enter Month (e.g. July 2026): ");

    for (auto& bill : bills) {
        if (InputValidator::toLower(bill->getMeterNumber()) == InputValidator::toLower(meter) &&
            InputValidator::toLower(bill->getMonthYear()) == InputValidator::toLower(month)) {

            if (bill->getPaymentStatus() == "PAID") {
                cout << "\n[Info] This bill is ALREADY PAID in full.\n";
                bill->display();
                return;
            }

            bill->display();

            if (InputValidator::getConfirm("\nConfirm payment of ₹" + to_string(static_cast<int>(bill->getBillAmount())) + "? (Y/N): ")) {
                bill->makePayment();
                saveBills(bills);
                syncFrontendDatabase(customers, bills);
                cout << "\n[OK] Payment successful! Bill status changed to PAID.\n";
                cout << "  -> Synchronized with Frontend (database.js updated)\n";
            } else {
                cout << "\n[Info] Payment cancelled.\n";
            }
            return;
        }
    }

    cout << "\n[ALERT] No bill found matching Meter '" << meter << "' and Month '" << month << "'.\n";
}

// ============================================================
// DELETE CUSTOMER
// ============================================================

void deleteCustomer(
    vector<unique_ptr<Customer>>& customers
) {
    if (customers.empty()) {
        cout << "\n[Info] No customers available to delete.\n";
        return;
    }

    int id = InputValidator::getValidInt("\nEnter Customer ID to delete: ", 1);

    auto it = find_if(
        customers.begin(),
        customers.end(),
        [id](const unique_ptr<Customer>& customer) {
            return customer->getId() == id;
        }
    );

    if (it != customers.end()) {
        cout << "\nCustomer found:\n";
        (*it)->display();

        if (InputValidator::getConfirm("\nAre you SURE you want to permanently delete this customer? (Y/N): ")) {
            customers.erase(it);
            cout << "\n[OK] Customer deleted successfully.\n";
        } else {
            cout << "\n[Info] Deletion cancelled.\n";
        }
    } else {
        cout << "\n[ALERT] Customer ID " << id << " not found.\n";
    }
}

// ============================================================
// FILE PERSISTENCE - Safe Exception-Handled Parsing
// ============================================================

const string CUSTOMERS_FILE = "customers.dat";
const string BILLS_FILE     = "bills.dat";
const string DATABASE_JS    = "database.js";

// Helper to escape strings safely for JSON / JS
string escapeJSON(const string& s) {
    ostringstream o;
    for (char c : s) {
        if (c == '"') o << "\\\"";
        else if (c == '\\') o << "\\\\";
        else if (c == '\b') o << "\\b";
        else if (c == '\f') o << "\\f";
        else if (c == '\n') o << "\\n";
        else if (c == '\r') o << "\\r";
        else if (c == '\t') o << "\\t";
        else if (static_cast<unsigned char>(c) < 0x20) {
            const char* hex = "0123456789abcdef";
            unsigned char value = static_cast<unsigned char>(c);
            o << "\\u00" << hex[value >> 4] << hex[value & 0x0f];
        }
        else o << c;
    }
    return o.str();
}

// Synchronizes all C++ terminal data into database.js for the index.html frontend
void syncFrontendDatabase(
    const vector<unique_ptr<Customer>>& customers,
    const vector<unique_ptr<ElectricityBill>>& bills
) {
    ofstream file(DATABASE_JS);
    if (!file.is_open()) return;

    file << "// Auto-generated by Smart Electricity System (C++ Backend)\n";
    file << "// Live sync bridge with index.html\n";
    file << "window.SEMS_BACKEND_DATA = {\n";
    file << "  syncedAt: \"" << __DATE__ << " " << __TIME__ << "\",\n";
    file << "  customers: [\n";
    for (size_t i = 0; i < customers.size(); ++i) {
        const auto& c = customers[i];
        file << "    {\n";
        file << "      \"id\": " << c->getId() << ",\n";
        file << "      \"name\": \"" << escapeJSON(c->getName()) << "\",\n";
        file << "      \"address\": \"" << escapeJSON(c->getAddress()) << "\",\n";
        file << "      \"meterNumber\": \"" << escapeJSON(c->getMeterNumber()) << "\",\n";
        file << "      \"type\": \"" << escapeJSON(c->getType()) << "\"\n";
        file << "    }" << (i + 1 < customers.size() ? "," : "") << "\n";
    }
    file << "  ],\n";
    file << "  bills: [\n";
    for (size_t i = 0; i < bills.size(); ++i) {
        const auto& b = bills[i];
        file << "    {\n";
        file << "      \"customerId\": " << b->getCustomerId() << ",\n";
        file << "      \"customerName\": \"" << escapeJSON(b->getCustomerName()) << "\",\n";
        file << "      \"meterNumber\": \"" << escapeJSON(b->getMeterNumber()) << "\",\n";
        file << "      \"customerType\": \"" << escapeJSON(b->getCustomerType()) << "\",\n";
        file << "      \"monthYear\": \"" << escapeJSON(b->getMonthYear()) << "\",\n";
        file << fixed << setprecision(2);
        file << "      \"previousReading\": " << b->getPreviousReading() << ",\n";
        file << "      \"currentReading\": " << b->getCurrentReading() << ",\n";
        file << "      \"units\": " << b->getUnits() << ",\n";
        file << "      \"billAmount\": " << b->getBillAmount() << ",\n";
        file << "      \"paymentStatus\": \"" << escapeJSON(b->getPaymentStatus()) << "\"\n";
        file << "    }" << (i + 1 < bills.size() ? "," : "") << "\n";
    }
    file << "  ]\n";
    file << "};\n";
}

// ---------- Save / Load Customers ----------

bool saveCustomers(
    const vector<unique_ptr<Customer>>& customers
) {
    ofstream file(CUSTOMERS_FILE);
    if (!file.is_open()) return false;
    for (const auto& c : customers) {
        file << c->getId()          << "|"
             << c->getName()        << "|"
             << c->getAddress()     << "|"
             << c->getMeterNumber() << "|"
             << c->getType()        << "\n";
    }
    file.close();
    return file.good();
}

void loadCustomers(
    vector<unique_ptr<Customer>>& customers
) {
    ifstream file(CUSTOMERS_FILE);
    if (!file.is_open()) return;

    string line;
    while (getline(file, line)) {
        line = InputValidator::trim(line);
        if (line.empty()) continue;

        stringstream ss(line);
        string token;
        vector<string> fields;
        while (getline(ss, token, '|'))
            fields.push_back(token);

        if (fields.size() == 5) {
            try {
                int id = stoi(fields[0]);
                if (id > 0) {
                    customers.push_back(
                        make_unique<Customer>(
                            id,
                            fields[1],
                            fields[2],
                            fields[3],
                            fields[4]
                        )
                    );
                }
            } catch (const exception&) {
                // Safely skip malformed row
            }
        }
    }

    if (!customers.empty())
        cerr << "[Loaded " << customers.size() << " customer(s) from " << CUSTOMERS_FILE << "]\n";
}

// ---------- Save / Load Bills ----------

bool saveBills(
    const vector<unique_ptr<ElectricityBill>>& bills
) {
    ofstream file(BILLS_FILE);
    if (!file.is_open()) return false;
    for (const auto& b : bills) {
        file << fixed << setprecision(6)
             << b->getCustomerId()      << "|"
             << b->getCustomerName()    << "|"
             << b->getMeterNumber()     << "|"
             << b->getCustomerType()    << "|"
             << b->getMonthYear()       << "|"
             << b->getPreviousReading() << "|"
             << b->getCurrentReading()  << "|"
             << b->getPaymentStatus()   << "\n";
    }
    file.close();
    return file.good();
}

void loadBills(
    vector<unique_ptr<ElectricityBill>>& bills
) {
    ifstream file(BILLS_FILE);
    if (!file.is_open()) return;

    string line;
    while (getline(file, line)) {
        line = InputValidator::trim(line);
        if (line.empty()) continue;

        stringstream ss(line);
        string token;
        vector<string> fields;
        while (getline(ss, token, '|'))
            fields.push_back(token);

        if (fields.size() == 8) {
            try {
                int cid = stoi(fields[0]);
                double prev = stod(fields[5]);
                double curr = stod(fields[6]);

                auto bill = make_unique<ElectricityBill>(
                    cid,
                    fields[1],
                    fields[2],
                    fields[3],
                    fields[4],
                    prev,
                    curr
                );

                if (fields[7] == "PAID")
                    bill->makePayment();

                bills.push_back(move(bill));
            } catch (const exception&) {
                // Safely skip malformed row
            }
        }
    }

    if (!bills.empty())
        cerr << "[Loaded " << bills.size() << " bill(s) from " << BILLS_FILE << "]\n";
}

class ApiError : public runtime_error {
private:
    int statusCode;

public:
    ApiError(int status, const string& message)
        : runtime_error(message), statusCode(status) {}

    int getStatusCode() const { return statusCode; }
};

class ElectricityApplication {
private:
    static bool hasUnsafeField(const string& value) {
        return value.find('|') != string::npos ||
               value.find('\n') != string::npos ||
               value.find('\r') != string::npos;
    }

    static int parseId(const string& value) {
        size_t consumed = 0;
        int id;
        try {
            id = stoi(value, &consumed);
        } catch (const exception&) {
            throw ApiError(400, "Customer ID must be a positive integer.");
        }
        if (consumed != value.size() || id <= 0)
            throw ApiError(400, "Customer ID must be a positive integer.");
        return id;
    }

    static double parseReading(const string& value) {
        size_t consumed = 0;
        double reading;
        try {
            reading = stod(value, &consumed);
        } catch (const exception&) {
            throw ApiError(400, "Meter readings must be valid non-negative numbers.");
        }
        if (consumed != value.size() || !isfinite(reading) || reading < 0)
            throw ApiError(400, "Meter readings must be valid non-negative numbers.");
        return reading;
    }

    static string customerJSON(const Customer& customer) {
        ostringstream json;
        json << "{\"id\":" << customer.getId()
             << ",\"name\":\"" << escapeJSON(customer.getName())
             << "\",\"address\":\"" << escapeJSON(customer.getAddress())
             << "\",\"meterNumber\":\"" << escapeJSON(customer.getMeterNumber())
             << "\",\"type\":\"" << escapeJSON(customer.getType()) << "\"}";
        return json.str();
    }

    static string billJSON(const ElectricityBill& bill) {
        ostringstream json;
        json << fixed << setprecision(2)
             << "{\"customerId\":" << bill.getCustomerId()
             << ",\"customerName\":\"" << escapeJSON(bill.getCustomerName())
             << "\",\"meterNumber\":\"" << escapeJSON(bill.getMeterNumber())
             << "\",\"customerType\":\"" << escapeJSON(bill.getCustomerType())
             << "\",\"monthYear\":\"" << escapeJSON(bill.getMonthYear())
             << "\",\"previousReading\":" << bill.getPreviousReading()
             << ",\"currentReading\":" << bill.getCurrentReading()
             << ",\"units\":" << bill.getUnits()
             << ",\"billAmount\":" << bill.getBillAmount()
             << ",\"paymentStatus\":\"" << escapeJSON(bill.getPaymentStatus()) << "\"}";
        return json.str();
    }

public:
    string validateCustomer(const vector<string>& args) const {
        int id = parseId(args[0]);
        string name = InputValidator::trim(args[1]);
        string address = InputValidator::trim(args[2]);
        string meter = InputValidator::trim(args[3]);
        string type = InputValidator::trim(args[4]);
        if (name.size() < 2 || address.size() < 3 || meter.size() < 2 ||
            hasUnsafeField(name) || hasUnsafeField(address) || hasUnsafeField(meter) ||
            meter.find(' ') != string::npos || hasUnsafeField(type))
            throw ApiError(400, "Customer fields are invalid.");
        if (type != "Residential" && type != "Commercial")
            throw ApiError(400, "Customer type must be Residential or Commercial.");

        Customer customer(id, name, address, meter, type);
        return customerJSON(customer);
    }

    string generateBill(const vector<string>& args) {
        int id = parseId(args[0]);
        string name = InputValidator::trim(args[1]);
        string meterNumber = InputValidator::trim(args[2]);
        string type = InputValidator::trim(args[3]);
        string month = InputValidator::trim(args[4]);
        double previous = parseReading(args[5]);
        double current = parseReading(args[6]);
        if (name.size() < 2 || meterNumber.size() < 2 ||
            hasUnsafeField(name) || hasUnsafeField(meterNumber) ||
            meterNumber.find(' ') != string::npos)
            throw ApiError(400, "Customer details for this bill are invalid.");
        if (type != "Residential" && type != "Commercial")
            throw ApiError(400, "Customer type must be Residential or Commercial.");
        if (current < previous)
            throw ApiError(400, "Current reading cannot be less than previous reading.");
        if (month.size() < 2 || hasUnsafeField(month))
            throw ApiError(400, "Enter a valid billing month.");

        Meter reading(previous, current);
        if (reading.calculateUnits() < 0)
            throw ApiError(400, "Current reading cannot be less than previous reading.");
        ElectricityBill bill(id, name, meterNumber, type, month, previous, current);
        return billJSON(bill);
    }

    string markBillPaid(const vector<string>& args) const {
        int id = parseId(args[0]);
        string name = InputValidator::trim(args[1]);
        string meterNumber = InputValidator::trim(args[2]);
        string type = InputValidator::trim(args[3]);
        string month = InputValidator::trim(args[4]);
        double previous = parseReading(args[5]);
        double current = parseReading(args[6]);
        if (current < previous)
            throw ApiError(400, "Current reading cannot be less than previous reading.");
        ElectricityBill bill(id, name, meterNumber, type, month, previous, current);
        bill.makePayment();
        return billJSON(bill);
    }
};

int runApiCommand(int argc, char* argv[]) {
    if (argc < 3 || string(argv[1]) != "--api") {
        cerr << "Usage: sems_cpp.exe --api <validate-customer|generate-bill|pay-bill> [arguments]\n";
        return 2;
    }

    try {
        ElectricityApplication application;
        vector<string> args;
        for (int i = 3; i < argc; ++i) args.push_back(argv[i]);
        string result;
        string command = argv[2];

        if (command == "validate-customer" && args.size() == 5) {
            result = application.validateCustomer(args);
        } else if (command == "generate-bill" && args.size() == 7) {
            result = application.generateBill(args);
        } else if (command == "pay-bill" && args.size() == 7) {
            result = application.markBillPaid(args);
        } else {
            throw ApiError(400, "Invalid C++ API command or arguments.");
        }

        cout << result;
        return 0;
    } catch (const ApiError& error) {
        cerr << error.what();
        if (error.getStatusCode() == 404) return 3;
        if (error.getStatusCode() == 409) return 4;
        if (error.getStatusCode() >= 500) return 5;
        return 2;
    } catch (const exception& error) {
        cerr << error.what();
        return 5;
    }
}

// ============================================================
// MAIN APPLICATION
// ============================================================

int main(int argc, char* argv[]) {
    if (argc > 1 && string(argv[1]) == "--api")
        return runApiCommand(argc, argv);

    Admin admin;

    if (!admin.login()) {
        cout << "\nTerminating application. Goodbye!\n";
        return 0;
    }

    vector<unique_ptr<Customer>> customers;
    vector<unique_ptr<ElectricityBill>> bills;

    // Load persisted data
    loadCustomers(customers);
    loadBills(bills);
    syncFrontendDatabase(customers, bills);

    int choice = 0;

    while (choice != 11) {
        cout << "\n\n";
        cout << "=================================================\n";
        cout << "       SMART ELECTRICITY MANAGEMENT SYSTEM\n";
        cout << "=================================================\n";
        cout << " 1.  Add Customer\n";
        cout << " 2.  Meter Reading & Generate Bill\n";
        cout << " 3.  Search Monthly Bill\n";
        cout << " 4.  Bill History\n";
        cout << " 5.  Electricity Theft Detection\n";
        cout << " 6.  Dashboard Summary\n";
        cout << " 7.  List All Customers\n";
        cout << " 8.  Search Customer\n";
        cout << " 9.  Update Customer\n";
        cout << "10. Delete Customer\n";
        cout << "11. Exit\n";
        cout << "=================================================\n";

        choice = InputValidator::getValidInt("Select option (1-11): ", 1, 11);

        switch (choice) {

        // ====================================================
        // ADD CUSTOMER - Fully Validated
        // ====================================================
        case 1: {
            cout << "\n========== ADD CUSTOMER ==========\n";

            int id = InputValidator::getValidInt("Enter Customer ID (> 0): ", 1);

            // Check duplicate Customer ID
            if (searchCustomer(customers, id)) {
                cout << "\n!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n";
                cout << "  [ALERT] Customer ID " << id << " already exists!\n";
                cout << "  Cannot add duplicate customer ID.\n";
                cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n";
                break;
            }

            string name = InputValidator::getValidName("Enter Name: ");
            string address = InputValidator::getValidAddress("Enter Address: ");
            string meter = InputValidator::getValidMeterNumber("Enter Meter Number: ");

            // Check duplicate Meter Number (case-insensitive)
            bool duplicateMeter = false;
            for (const auto& c : customers) {
                if (InputValidator::toLower(c->getMeterNumber()) == InputValidator::toLower(meter)) {
                    cout << "\n!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n";
                    cout << "  [ALERT] Meter Number '" << meter << "' is already\n";
                    cout << "  registered to Customer ID: " << c->getId()
                         << " (" << c->getName() << ")!\n";
                    cout << "  No customer was added.\n";
                    cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n";
                    duplicateMeter = true;
                    break;
                }
            }
            if (duplicateMeter) break;

            string type = InputValidator::getValidCustomerType("Enter Type (Residential/Commercial): ");

            auto customer = make_unique<Customer>(
                id,
                name,
                address,
                meter,
                type
            );

            customers.push_back(move(customer));
            saveCustomers(customers);
            syncFrontendDatabase(customers, bills);

            cout << "\n[OK] Customer added successfully!\n";
            cout << "  -> Automatically synchronized with Frontend (database.js updated)\n";
            break;
        }

        // ====================================================
        // METER READING & GENERATE BILL - Fully Validated
        // ====================================================
        case 2: {
            if (customers.empty()) {
                cout << "\n[Info] No customers registered yet. Please add a customer first.\n";
                break;
            }

            int id = InputValidator::getValidInt("\nEnter Customer ID for Billing: ", 1);

            Customer* customer = searchCustomer(customers, id);
            if (!customer) {
                cout << "\n[ALERT] Customer with ID " << id << " not found.\n";
                break;
            }

            cout << "\nGenerating bill for:\n";
            cout << "  Customer : " << customer->getName() << " (ID: " << customer->getId() << ")\n";
            cout << "  Meter No : " << customer->getMeterNumber() << "\n";
            cout << "  Type     : " << customer->getType() << "\n";

            string monthYear = InputValidator::getValidMonthYear("Enter Month (e.g. July 2026): ");

            // Check if a bill already exists for this meter and month
            bool billExists = false;
            for (const auto& b : bills) {
                if (InputValidator::toLower(b->getMeterNumber()) == InputValidator::toLower(customer->getMeterNumber()) &&
                    InputValidator::toLower(b->getMonthYear()) == InputValidator::toLower(monthYear)) {
                    billExists = true;
                    break;
                }
            }

            if (billExists) {
                cout << "\n!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n";
                cout << "  [ALERT] A bill for Meter '" << customer->getMeterNumber()
                     << "' and Month '" << monthYear << "' already exists!\n";
                cout << "  Duplicate monthly billing is not allowed.\n";
                cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n";
                break;
            }

            Meter meter;
            meter.enterReading();

            double units = meter.calculateUnits();
            cout << "\nUnits Consumed: " << fixed << setprecision(2) << units << " kWh\n";

            auto bill = make_unique<ElectricityBill>(
                customer->getId(),
                customer->getName(),
                customer->getMeterNumber(),
                customer->getType(),
                monthYear,
                meter.getPreviousReading(),
                meter.getCurrentReading()
            );

            bill->display();

            bills.push_back(move(bill));
            saveBills(bills);
            syncFrontendDatabase(customers, bills);

            cout << "\n[OK] Bill generated and saved successfully!\n";
            cout << "  -> Automatically synchronized with Frontend (database.js updated)\n";
            break;
        }

        // ====================================================
        // SEARCH BILL
        // ====================================================
        case 3:
            searchBill(bills);
            break;

        // ====================================================
        // BILL HISTORY
        // ====================================================
        case 4:
            showBillHistory(bills);
            break;

        // ====================================================
        // THEFT DETECTION - Fully Validated
        // ====================================================
        case 5: {
            if (customers.empty()) {
                cout << "\n[Info] No customers registered yet.\n";
                break;
            }

            int id = InputValidator::getValidInt("\nEnter Customer ID for Theft Detection: ", 1);

            Customer* customer = searchCustomer(customers, id);
            if (!customer) {
                cout << "\n[ALERT] Customer with ID " << id << " not found.\n";
                break;
            }

            cout << "\nCustomer: " << customer->getName() << " | Meter: " << customer->getMeterNumber() << "\n";

            double expected = InputValidator::getValidDouble("Enter expected/average units (> 0): ", 0.001);
            double actual = InputValidator::getValidDouble("Enter actual units consumed (>= 0): ", 0.0);

            TheftDetection detection(expected, actual);

            if (detection.isAnomaly()) {
                detection.generateAlert();
            } else {
                cout << "\n[OK] Usage is within the normal expected range.\n";
                cout << "No suspicious activity or theft detected.\n";
            }
            break;
        }

        // ====================================================
        // DASHBOARD
        // ====================================================
        case 6:
            Dashboard::showSummary(customers, bills);
            break;

        // ====================================================
        // LIST CUSTOMERS
        // ====================================================
        case 7:
            listCustomers(customers);
            break;

        // ====================================================
        // SEARCH CUSTOMER
        // ====================================================
        case 8:
            searchCustomerMenu(customers);
            break;

        // ====================================================
        // UPDATE CUSTOMER - Fully Validated
        // ====================================================
        case 9: {
            if (customers.empty()) {
                cout << "\n[Info] No customers registered yet to update.\n";
                break;
            }

            int id = InputValidator::getValidInt("\nEnter Customer ID to update: ", 1);

            Customer* customer = searchCustomer(customers, id);
            if (customer) {
                customer->update(customers);
                saveCustomers(customers);
                syncFrontendDatabase(customers, bills);
                cout << "  -> Automatically synchronized with Frontend (database.js updated)\n";
            } else {
                cout << "\n[ALERT] Customer ID " << id << " not found.\n";
            }
            break;
        }

        // ====================================================
        // DELETE CUSTOMER - Confirmed & Validated
        // ====================================================
        case 10:
            deleteCustomer(customers);
            saveCustomers(customers);
            syncFrontendDatabase(customers, bills);
            cout << "  -> Automatically synchronized with Frontend (database.js updated)\n";
            break;

        // ====================================================
        // EXIT
        // ====================================================
        case 11:
            cout << "\nThank you for using Smart Electricity Management System!\n";
            break;

        default:
            cout << "\n[Error] Invalid choice. Please select an option between 1 and 11.\n";
        }
    }

    return 0;
}