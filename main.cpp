#include <iostream>
#include "NMEAParser.h"

int main() {
    // 1. Create the Parser
    NMEAParser parser;

    // 2. Define a dummy NMEA string (GPGGA)
    // This is a real string, but our parser logic isn't written yet.
    std::string rawData = "$GPGGA,123519,4807.038,N,01131.000,E,1,08,0.9,545.4,M,46.9,M,,*47";

    std::cout << "System Status: Initialized" << std::endl;
    std::cout << "Input Data: " << rawData << std::endl;

    // 3. Pass it to the parser
    GPSData location = parser.parse(rawData);

    // 4. Check output (It will be empty/invalid because we only wrote stubs)
    if (location.isValid) {
        std::cout << "Valid Fix Found!" << std::endl;
        std::cout << location.toString() << std::endl;
    } else {
        std::cout << "Parser Output: Invalid Data (Expected for Phase 1)" << std::endl;
    }

    return 0;
}