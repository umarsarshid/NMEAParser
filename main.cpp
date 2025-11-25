#include <iostream>
#include <iomanip> // For std::setprecision
#include "NMEAParser.h"

int main() {
    NMEAParser parser;
    
    // Real Data: 48 deg 07.038 min North, 11 deg 31.000 min East
    std::string rawData = "$GPGGA,123519,4807.038,N,01131.000,E,1,08,0.9,545.4,M,46.9,M,,*47";

    std::cout << "Parsing: " << rawData << std::endl;
    
    GPSData location = parser.parse(rawData);

    if (location.isValid) {
        std::cout << "---------------------------------" << std::endl;
        std::cout << "STATUS:    Valid Fix" << std::endl;
        std::cout << "SATELLITES:" << location.satellites << std::endl;
        
        // Print coordinates with high precision
        std::cout << std::fixed << std::setprecision(6);
        std::cout << "LATITUDE:  " << location.latitude << " (Expected ~48.1173)" << std::endl;
        std::cout << "LONGITUDE: " << location.longitude << " (Expected ~11.5167)" << std::endl;
        std::cout << "ALTITUDE:  " << location.altitude << " m" << std::endl;
        std::cout << "---------------------------------" << std::endl;
    } else {
        std::cout << "STATUS: Invalid Data" << std::endl;
    }

    return 0;
}
