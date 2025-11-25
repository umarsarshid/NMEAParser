#include <iostream>
#include <vector>
#include <iomanip> // For std::setprecision
#include "NMEAParser.h"

int main() {
    // 1. Create an instance of NMEAParser
    NMEAParser parser;

    std::cout << "--- GPS NMEA Parser System v1.0 ---\n" << std::endl;

    // 2. Create raw string variables (Simulating an incoming data stream)
    // We include a valid fix, an invalid checksum, and a 'lost fix' scenario.
    std::vector<std::string> gpsStream = {
        "$GPGGA,123519,4807.038,N,01131.000,E,1,08,0.9,545.4,M,46.9,M,,*47", // Valid Fix
        "$GPGGA,123520,4807.040,N,01131.010,E,1,08,0.9,545.4,M,46.9,M,,*00", // Checksum Error
        "$GPGGA,,,,,,0,,,,,,"                                              // No Satellite Lock
    };

    // 3. The Main Loop
    for (const auto& rawSentence : gpsStream) {
        std::cout << "[RX] Data Received: " << rawSentence << std::endl;

        // Pass it to parser.parse()
        GPSData data = parser.parse(rawSentence);

        // 13. Output Display
        if (data.isValid) {
            // If yes, print the Latitude and Longitude
            std::cout << "  -> STATUS: [VALID]" << std::endl;
            std::cout << std::fixed << std::setprecision(6); // Format to 6 decimal places
            std::cout << "     Lat: " << data.latitude 
                      << " | Lon: " << data.longitude 
                      << " | Alt: " << data.altitude << "m" << std::endl;
        } else {
            // If no, print Error
            std::cout << "  -> STATUS: [INVALID / CHECKSUM ERROR]" << std::endl;
        }
        std::cout << "----------------------------------------------------" << std::endl;
    }

    return 0;
}
