#include "NMEAParser.h"
#include <cmath> // Will be needed later for math

// Main Parse Function
GPSData NMEAParser::parse(const std::string& nmeastring) {
    GPSData result;
    
    // TODO: Implement Logic
    // 1. Check Valid Checksum
    // 2. Tokenize String
    // 3. Parse Fields
    
    // For Phase 1 testing, we just return an empty object
    return result;
}

// Helper: Checksum Validation
bool NMEAParser::validateChecksum(const std::string& s) {
    // Stub: Always say yes for now
    return true;
}

// Helper: String Splitter
std::vector<std::string> NMEAParser::split(const std::string& s, char delimiter) {
    std::vector<std::string> tokens;
    // Stub
    return tokens;
}

// Helper: Coordinate Converter
double NMEAParser::convertToDecimalDegrees(const std::string& nmeaPos, const std::string& direction) {
    // Stub
    return 0.0;
}

// Helper: Hex Converter
int NMEAParser::hexToDecimal(const std::string& hex) {
    if (hex.empty()) return 0;
       // std::stoi converts string to integer. Base 16 handles "A" -> 10, "F" -> 15.
    try{
        return std::stoi(hex, nullptr, 16);
    } catch(const std::exception& e){
        std::cerr << e.what() << '\n';
        return 0;// Return 0 on error
    }
}