#include "SQLiteLogger.h"

void SQLiteLogger::initTable() {
    // Basic Schema: ID, Timestamp, Lat, Lon, Speed
    const char* sql = "CREATE TABLE IF NOT EXISTS tracklog (" \
                      "id INTEGER PRIMARY KEY AUTOINCREMENT," \
                      "timestamp TEXT," \
                      "lat REAL," \
                      "lon REAL," \
                      "speed REAL);";

    char* errMsg = 0;
    // sqlite3_exec is fine for simple statements with no variables
    int rc = sqlite3_exec(db, sql, 0, 0, &errMsg);
    
    if (rc != SQLITE_OK) {
        std::cerr << "SQL Error: " << errMsg << std::endl;
        sqlite3_free(errMsg);
    }
}

void SQLiteLogger::log(const GPSData& data) {
    // The Query using '?' placeholders
    const char* sql = "INSERT INTO tracklog (timestamp, lat, lon, speed) VALUES (?, ?, ?, ?);";
    
    sqlite3_stmt* stmt;

    // 1. Prepare (Compile) the SQL
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) != SQLITE_OK) {
        std::cerr << "DB Prepare Error: " << sqlite3_errmsg(db) << std::endl;
        return;
    }

    // 2. Bind Values to the '?' placeholders
    // (Note: We just store timestamp as string for now to keep it simple)
    // Index starts at 1, not 0 in SQLite!
    sqlite3_bind_text(stmt, 1, "123519", -1, SQLITE_STATIC); // TODO: Pass real time string
    sqlite3_bind_double(stmt, 2, data.latitude);
    sqlite3_bind_double(stmt, 3, data.longitude);
    sqlite3_bind_double(stmt, 4, data.speed); // Only valid if GPRMC, else 0

    // 3. Execute
    if (sqlite3_step(stmt) != SQLITE_DONE) {
        std::cerr << "DB Step Error: " << sqlite3_errmsg(db) << std::endl;
    }

    // 4. Cleanup (Critical!)
    sqlite3_finalize(stmt);
}