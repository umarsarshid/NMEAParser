#include <iostream>
#include "NMEAParser.h"
#include <vector> // Required for vector

void runTest(NMEAParser& parser, std::string name, std::string nmea) {
    std::cout << "Test: " << name << " | Input: " << nmea << std::endl;
    GPSData data = parser.parse(nmea);
    if (data.isValid) {
        std::cout << " -> Result: PASSED (Checksum Valid)" << std::endl;
    } else {
        std::cout << " -> Result: BLOCKED (Checksum Invalid)" << std::endl;
    }
    std::cout << "------------------------------------------------" << std::endl;
    
}

int main() {
    NMEAParser parser;
    
    // 1. Valid String (Checksum 47 is correct for this data)
    runTest(parser, "Good Data", "$GPGGA,123519,4807.038,N,01131.000,E,1,08,0.9,545.4,M,46.9,M,,*47");

    // 2. Corrupted String (Modified 'N' to 'S' without changing checksum)
    // The checksum *should* be different, so the parser must reject this.
    runTest(parser, "Bad Data ", "$GPGGA,123519,4807.038,S,01131.000,E,1,08,0.9,545.4,M,46.9,M,,*47");

    // 3. Garbage String
    runTest(parser, "Garbage  ", "Not NMEA Data");

    return 0;
}