#pragma once
#include "NMEAParser.h" // Needs visibility of GPSData and Static Helpers

// Abstract Base Class
class INMEASentence {
public:
    virtual ~INMEASentence() = default; // Virtual destructor for proper cleanup
    virtual void parse(const std::vector<std::string>& tokens, GPSData& data) = 0; //each derived class implements this
};
// 1. GPGGA Strategy
class GPGGASentence : public INMEASentence {
public:
    void parse(const std::vector<std::string>& tokens, GPSData& data) override {
        data.type = "GPGGA";
        // Map tokens specific to GPGGA
        if (tokens.size() > 3) data.latitude = NMEAParser::convertToDecimalDegrees(tokens[2], tokens[3]);
        if (tokens.size() > 5) data.longitude = NMEAParser::convertToDecimalDegrees(tokens[4], tokens[5]);
        if (tokens.size() > 6 && !tokens[6].empty()) data.fixQuality = std::stoi(tokens[6]);
        if (tokens.size() > 7 && !tokens[7].empty()) data.satellites = std::stoi(tokens[7]);
        if (tokens.size() > 9) {
        try {
            data.altitude = std::stod(std::string(tokens[9]));
        } catch (...) { data.altitude = 0.0; }
    }
    }
};

// 2. GPRMC Strategy
class GPRMCSentence : public INMEASentence {
public:
    void parse(const std::vector<std::string>& tokens, GPSData& data) override {
        data.type = "GPRMC";
        // GPRMC puts status in token 2 ('A' = valid)
        if (tokens.size() > 2 && tokens[2] == "A") data.fixQuality = 1; 
        else data.fixQuality = 0;

        if (tokens.size() > 4) data.latitude = NMEAParser::convertToDecimalDegrees(tokens[3], tokens[4]);
        if (tokens.size() > 6) data.longitude = NMEAParser::convertToDecimalDegrees(tokens[5], tokens[6]);
        
        // Specific to RMC
        if (tokens.size() > 7 && !tokens[7].empty()) data.speed = std::stod(tokens[7]);
        if (tokens.size() > 8 && !tokens[8].empty()) data.course = std::stod(tokens[8]);
    }
};