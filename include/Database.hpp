#ifndef DATABASE_HPP
#define DATABASE_HPP

#include "sqlite3.h"
#include "Customer.hpp"
#include "ElectricityBill.hpp"
#include <vector>
#include <string>
#include <iostream>

// ============================================================
// DATABASE CLASS - SQLite3 Wrapper (Singleton)
// ============================================================

class Database {
private:
    sqlite3* db;
    static Database* instance;
    static const std::string DB_PATH;

    Database();                           // Private constructor (Singleton)
    void createTables();
    void seedSampleData();
    bool isEmpty();

    // Internal helpers
    static int customerRowCallback(void* vec, int argc, char** argv, char** colName);
    static int billRowCallback(void* vec, int argc, char** argv, char** colName);

public:
    static Database* getInstance();
    ~Database();

    // ── Customer CRUD ──────────────────────────────────────
    bool insertCustomer(int id,
                        const std::string& name,
                        const std::string& address,
                        const std::string& meter,
                        const std::string& type);

    bool updateCustomer(int id,
                        const std::string& name,
                        const std::string& address,
                        const std::string& meter,
                        const std::string& type);

    bool deleteCustomer(int id);
    bool customerExists(int id);
    std::vector<Customer*> getAllCustomers();
    Customer* findCustomerById(int id);

    // ── Bill CRUD ──────────────────────────────────────────
    bool insertBill(const std::string& meterNumber,
                    const std::string& customerName,
                    const std::string& customerType,
                    const std::string& monthYear,
                    float  units,
                    float  amount,
                    const std::string& status = "UNPAID");

    bool updateBillStatus(const std::string& meterNumber,
                          const std::string& monthYear,
                          const std::string& status);

    std::vector<ElectricityBill*> getAllBills();
    std::vector<ElectricityBill*> getBillsByMeter(const std::string& meterNumber);

    // ── Stats ──────────────────────────────────────────────
    int    getTotalCustomers();
    int    getTotalBills();
    double getTotalRevenue();
    int    getPaidBillCount();
    int    getUnpaidBillCount();
};

#endif // DATABASE_HPP
