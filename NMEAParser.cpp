#include "NMEAParser.h"
#include <cmath> // Will be needed later for math
#include <sstream> // For stringstream in split 

// Main Parse Function
GPSData NMEAParser::parse(const std::string& nmeastring) {
    GPSData result;
    
    // TODO: Implement Logic
    // 1. Check Valid Checksum

    if (!validateChecksum(nmeastring)) {
        result.isValid = false;
        return result; // Early return on invalid data
    }
    result.isValid = true;  
    // 2. Tokenize String
    // 3. Parse Fields
    return result;
}

// Helper: Checksum Validation
bool NMEAParser::validateChecksum(const std::string& s) {
    // 1. Identify the Payload
    size_t dollar = s.find('$');
    size_t asterisk = s.find('*');

    // Edge Case: If chars are missing or out of order, it's garbage.
    if (dollar == std::string::npos || asterisk == std::string::npos || asterisk < dollar) {
        return false;
    }

    // 2. Implement the XOR Loop
    int calculatedChecksum = 0;
    // Loop only the payload (between delimiters)
    for (size_t i = dollar + 1; i < asterisk; ++i) {
        calculatedChecksum ^= s[i];
    }

    // 3. Validate
    // Ensure there are at least 2 characters after the asterisk for the hex code
    if (asterisk + 3 > s.length()) {
        return false;
    }

    // Extract the 2 hex characters after '*'
    std::string providedHex = s.substr(asterisk + 1, 2);
    int providedChecksum = hexToDecimal(providedHex);

    return calculatedChecksum == providedChecksum;
}


// Helper: String Splitter
std::vector<std::string> NMEAParser::split(const std::string& s, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(s);
    // getline reads from 'tokenStream' into 'token', stopping at 'delimiter'
    while (std::getline(tokenStream, token, delimiter)) {
        tokens.push_back(token);
    }
    // Edge case: If the string ends with a comma (e.g., "data,"), 
    // getline might ignore the trailing empty field. 
    // For NMEA, strictly speaking, we usually just need the data between commas.
    
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