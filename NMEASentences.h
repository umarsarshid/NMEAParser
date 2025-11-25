#pragma once
#include "NMEAParser.h" // Needs visibility of GPSData and Static Helpers

// Abstract Base Class
class INMEASentence {
public:
    virtual ~INMEASentence() = default;
    virtual void parse(const std::vector<std::string>& tokens, GPSData& data) = 0;
};