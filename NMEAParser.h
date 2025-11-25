#pragma once

#include <string>
#include <vector>
#include <iostream>

// 1. Define the Data Object
// This struct holds the final, clean data extracted from the messy string.
struct GPSData {
    double timestamp = 0.0;     // UTC Time
    double latitude = 0.0;      // Decimal Degrees (converted from NMEA format)
    double longitude = 0.0;     // Decimal Degrees (converted from NMEA format)
    double altitude = 0.0;      // Meters above sea level
    
    int fixQuality = 0;         // 0 = Invalid, 1 = GPS Fix, 2 = DGPS Fix
    int satellites = 0;         // Number of satellites being tracked
    
    bool isValid = false;       // The critical flag: True only if checksum passes
    
    // New fields for GPRMC
    double speed = 0.0;     // Speed over ground (knots)
    double course = 0.0;    // Track angle in degrees True
    std::string date = "";  // Date string (DDMMYY)
    std::string type = "";  // "GPGGA" or "GPRMC"

    std::string toString() const {
        return type + " | Lat: " + std::to_string(latitude) + 
               " | Lon: " + std::to_string(longitude); 
    }
};

// 2. Define the Class Interface
class NMEAParser {
public:
    // Constructor
    NMEAParser() = default;

    // The Main Public Interface
    // Takes a raw NMEA string, returns a clean GPSData object
    GPSData parse(const std::string& nmeastring);

private:
    // HELPER FUNCTIONS (Internal logic only)
    
    // Verifies if the string is not corrupted
    bool validateChecksum(const std::string& s);
    
    // Splits the string by commas (like Python's split)
    std::vector<std::string> split(const std::string& s, char delimiter);
    
    // Converts NMEA weird coordinates (DDMM.MMMM) to standard Decimal Degrees
    double convertToDecimalDegrees(const std::string& nmeaPos, const std::string& direction);
    
    // Hex to Int converter for checksum validation
    int hexToDecimal(const std::string& hex);
};