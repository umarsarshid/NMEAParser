#include <iostream>
#include <cmath> // For std::abs
#include "NMEAParser.h"

int main() {
    NMEAParser parser;
    
    std::cout << "Testing Coordinate Conversion..." << std::endl;

    // Test 1: North (Positive)
    // 48 deg, 07.038 min -> 48 + (7.038/60) = 48.1173
    double lat = parser.convertToDecimalDegrees("4807.038", "N");
    std::cout << "4807.038 N -> " << lat << " (Expect ~48.1173)" << std::endl;

    // Test 2: West (Negative)
    // 11 deg, 31.000 min -> -(11 + 31/60) = -11.5166...
    double lon = parser.convertToDecimalDegrees("01131.000", "W");
    std::cout << "01131.000 W -> " << lon << " (Expect ~ -11.5166)" << std::endl;

    return 0;
}