#pragma once
#include <nlohmann/json.hpp>
#include "NMEAParser.h"

// Shorten the namespace for convenience
using json = nlohmann::json;

// Define the mapping between C++ Struct and JSON Object
inline void to_json(json& j, const GPSData& data) {
    j = json{
        {"type", data.type},
        {"timestamp", data.timestamp}, // You might want to format this later
        {"isValid", data.isValid},
        {"lat", data.latitude},
        {"lon", data.longitude},
        {"speed", data.speed},   // Knots
        {"course", data.course}, // Degrees
        {"sats", data.satellites},
        {"alt", data.altitude}
    };
}

// Helper to get a string immediately
inline std::string GPSDataToJson(const GPSData& data) {
    json j = data;
    return j.dump(); // .dump() converts JSON object to string
}