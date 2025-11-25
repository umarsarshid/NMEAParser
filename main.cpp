#include <iostream>
#include <memory>
#include "NMEAParser.h"
#include "NMEASource.h"

int main(int argc, char* argv[]) {
    NMEAParser parser;
    std::unique_ptr<INMEASource> source;

    std::cout << "Select Source: [1] UDP Network  [2] Serial Port: ";
    int choice;
    std::cin >> choice;

    if (choice == 1) {
        source = std::make_unique<UDPSource>(10110);
    } else {
        std::string port;
        std::cout << "Enter Serial Device (e.g., /dev/ttyUSB0 or /dev/pts/X): ";
        std::cin >> port;
        source = std::make_unique<SerialSource>(port);
    }

    if (!source->open()) return -1;

    std::cout << "--- Waiting for Data (Ctrl+C to quit) ---" << std::endl;

    while (true) {
        // BLOCKING CALL: Will wait here until data arrives
        std::string raw = source->readLine(); 
        
        // Remove trailing \r if present
        if (!raw.empty() && raw.back() == '\r') raw.pop_back();

        std::cout << "[RX] " << raw << std::endl;

        GPSData data = parser.parse(raw);
        
        if (data.isValid) {
            std::cout << "   >>> " << data.toString() << std::endl;
        }
    }

    source->close();
    return 0;
}