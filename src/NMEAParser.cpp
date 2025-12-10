#include "NMEAParser.h"
#include "NMEASentences.h"
#include <cmath> // Will be needed later for math
#include <sstream> // For stringstream in split 
#include <string>

/* Logic:
1. Validate Checksum
2. Tokenize String
3. Factory Dispatcher (Which sentence type?)
4. Execution
5. Return GPSData Object
*/
  // NEW: Subscription Method
void NMEAParser::onFix(GPSCallback cb) {
    listeners.push_back(cb);
}   

// NEW: Notify Listeners
void NMEAParser::notifyListeners(const GPSData& data) {
    for (const auto& listener : listeners) {
        listener(data); // Call each subscribed function
    }
}

// Internal helper: safely convert string to double
double NMEAParser::safeStod(std::string_view str) {
    if (str.empty()) return 0.0;
    try {
        // std::stod requires std::string, so we cast
        return std::stod(std::string(str));
    } catch (...) {
        return 0.0; // Return 0.0 on error instead of crashing
    }
}

// Internal helper: safely convert string to int
int NMEAParser::safeStoi(std::string_view str) {
    if (str.empty()) return 0;
    try {
        return std::stoi(std::string(str));
    } catch (...) {
        return 0; // Return 0 on error
    }
}


// Main Parse Function
GPSData NMEAParser::parse(const std::string& nmeastring) {
    GPSData result;
    
    // 1. Check Valid Checksum
    if (!validateChecksum(nmeastring)) {
        result.isValid = false;
        return result; // Early return on invalid data
    }

      
    // 2. Tokenize String
     // We assume the payload starts at '$' and we split by comma
    std::vector<std::string> tokens = split(nmeastring, ',');
    if (tokens.empty()) return result; // Early return on empty data

    // 3. The Factory Dispatcher
    // We use a pointer to the Interface (Polymorphism)
    INMEASentence* parser = nullptr;

    if (tokens[0] == "$GPGGA") {
        parser = new GPGGASentence();
    } else if (tokens[0] == "$GPRMC") {
        parser = new GPRMCSentence();
    }

    // 4. Execution
    if (parser != nullptr) {
        result.isValid = true;
        parser->parse(tokens, result);
        delete parser;
        
        // NEW: If valid, notify everyone!
        if (result.isValid) {
            notifyListeners(result);
        }
    } else {
        result.isValid = false;
    }

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
double NMEAParser::convertToDecimalDegrees(std::string_view nmeaPos, std::string_view direction) {
    if (nmeaPos.empty()) return 0.0;

    size_t decimalPos = nmeaPos.find('.');
    if (decimalPos == std::string_view::npos || decimalPos < 2) return 0.0;

    // Separate Degrees and Minutes
    size_t minutesStart = decimalPos - 2;
    std::string_view degStr = nmeaPos.substr(0, minutesStart);
    std::string_view minStr = nmeaPos.substr(minutesStart);

    // --- FIX: USE SAFE CONVERSION ---
    double degrees = safeStod(degStr);
    double minutes = safeStod(minStr);
    // --------------------------------

    double result = degrees + (minutes / 60.0);

    if (direction == "S" || direction == "W") {
        result *= -1.0;
    }

    return result;
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