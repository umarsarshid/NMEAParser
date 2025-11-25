#include <iostream>
#include "NMEAParser.h"
#include <vector> // Required for vector

int main() {
    // 1. Create the Parser
    NMEAParser parser;

    std::cout << "=== PHASE 2 TEST BENCH ===" << std::endl;

    // TEST 1: Hex Conversion
    std::string hexVal = "5A";
    int decimal = parser.hexToDecimal(hexVal);
    std::cout << "Hex '5A' -> Decimal: " << decimal << " (Expected: 90)" << std::endl;

    // TEST 2: Splitter
    std::string data = "Time,Lat,,Lon";
    std::vector<std::string> tokens = parser.split(data, ',');
    
    std::cout << "Split 'Time,Lat,,Lon':" << std::endl;
    for(size_t i = 0; i < tokens.size(); i++) {
        std::cout << " [" << i << "]: '" << tokens[i] << "'" << std::endl;
    }
    // Expected: [0]: 'Time', [1]: 'Lat', [2]: '', [3]: 'Lon'

    return 0;
}