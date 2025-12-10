#include <iostream>
#include <vector>
#include <chrono>
#include "NMEAParser.h"

int main() {
    NMEAParser parser;
    
    // 1. Generate Dummy Data (1 Million Lines)
    std::cout << "Generating 1,000,000 lines of data..." << std::endl;
    std::string sample = "$GPGGA,123519,4807.038,N,01131.000,E,1,08,0.9,545.4,M,46.9,M,,*47";
    std::vector<std::string> data(1000000, sample);

    // 2. Measure Parsing Time
    std::cout << "Starting Benchmark..." << std::endl;
    auto start = std::chrono::high_resolution_clock::now();

    volatile double prevent_opt = 0; // Prevent compiler from optimizing away the loop
    for (const auto& line : data) {
        GPSData result = parser.parse(line);
        if (result.isValid) prevent_opt += result.latitude;
    }

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> diff = end - start;

    std::cout << "Parsed 1,000,000 lines in: " << diff.count() << " seconds" << std::endl;
    std::cout << "Average throughput: " << (1000000 / diff.count()) << " msg/sec" << std::endl;

    return 0;
}
