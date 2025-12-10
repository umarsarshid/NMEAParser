#pragma once
#include <string>
#include <sqlite3.h> // The C Library header
#include <iostream>
#include "NMEAParser.h"

class SQLiteLogger {
private:
    sqlite3* db; // Raw pointer to the C struct

public:
    // Constructor: Opens DB and creates table if missing
    SQLiteLogger(const std::string& dbPath) {
        // 1. Open Database
        int rc = sqlite3_open(dbPath.c_str(), &db);
        if (rc) {
            std::cerr << "DB Error: " << sqlite3_errmsg(db) << std::endl;
        } else {
            std::cout << "DB: Opened " << dbPath << std::endl;
            initTable();
        }
    }

    // Destructor: Closes Database safely
    ~SQLiteLogger() {
        if (db) {
            sqlite3_close(db);
            std::cout << "DB: Connection Closed." << std::endl;
        }
    }

    // The Action Method
    void log(const GPSData& data);

private:
    void initTable();
};