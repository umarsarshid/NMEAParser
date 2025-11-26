#include <iostream>
#include <memory>
#include "NMEAParser.h"
#include "NMEASource.h"

// System A: The "GUI" (Prints formatted data)
void displaySystem(const GPSData& data) {
    std::cout << "\n[DISPLAY] Fix Acquired!" << std::endl;
    std::cout << "   Lat: " << data.latitude << " | Lon: " << data.longitude << std::endl;
}

// System B: The "Data Logger" (Simulates writing to a database)
void loggingSystem(const GPSData& data) {
    // In a real app, this might write to a CSV file or SQL DB
    std::cout << "[LOG] Database updated: " << data.toString() << std::endl;
}

int main() {
    NMEAParser parser;
    std::unique_ptr<INMEASource> source;

    // 1. Setup Source (Hardcoded to UDP for brevity, or reuse your selection logic)
    std::cout << "Initializing System..." << std::endl;
    // For testing, let's use Serial or UDP. 
    // If you don't have socat running, stick to UDP/netcat for easier testing.
    source = std::make_unique<UDPSource>(10110); 
    
    if (!source->open()) return -1;

    // 2. Subscribe Systems (The Wiring)
    // We attach two independent systems to the same parser
    parser.onFix(displaySystem);
    parser.onFix(loggingSystem);

    std::cout << "--- System Online: Waiting for Events ---" << std::endl;

    // 3. The Pump (The "Event Loop")
    while (true) {
        std::string raw = source->readLine();
        
        // Remove trailing \r
        if (!raw.empty() && raw.back() == '\r') raw.pop_back();

        // Note: We ignore the return value now! 
        // The parser handles the dispatching internally.
        parser.parse(raw);
    }

    source->close();
    return 0;
}
