#include <iostream>
#include <memory>
#include <thread>  
#include <atomic>  
#include "NMEAParser.h"
#include "NMEASource.h"
#include "SafeQueue.h" 
#include "SQLiteLogger.h"
#include "GPSDashboard.h"

// Global atomic flag to control thread shutdown
std::atomic<bool> running(true);

// ---------------------------------------------------------
// THREAD A: PRODUCER (Reads Hardware)
// ---------------------------------------------------------
void gpsReaderTask(INMEASource* source, SafeQueue<std::string>& queue) {
    // std::cout << "[THREAD-A] Reader started." << std::endl;
    while (running) {
        // This blocks until data comes from UDP/Serial
        std::string line = source->readLine();
        
        if (!line.empty()) {
            // std::cout << "[RX] " << line << std::endl;
            queue.push(line); // Hand off to queue
        }
    }
}

// ---------------------------------------------------------
// THREAD B: CONSUMER (Parses & Logs)
// ---------------------------------------------------------
void dataProcessorTask(NMEAParser* parser, SafeQueue<std::string>& queue) {
    std::cout << "[THREAD-B] Processor started." << std::endl;
    std::string rawData;
    
    while (running) {
        // This blocks (sleeps) until the queue has data
        queue.waitAndPop(rawData);

        // Simulate a slow database write or heavy calculation
        // 2. SIMULATE SLOW DATABASE
        // We force this thread to sleep for 500ms (0.5 seconds).
        // This is ETERNITY for a computer.
        // std::this_thread::sleep_for(std::chrono::milliseconds(500));

        parser->parse(rawData);
        // 4. VISUAL PROOF
        // We can't easily check .size() on std::queue without adding a method,
        // but we can print that we just finished one.
        // std::cout << "   [Slow-Consumer] Processed 1 msg. (Simulated Lag)" << std::endl;
    }
}

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
    SafeQueue<std::string> buffer;

    // 1. Setup Source (Hardcoded to UDP for brevity, or reuse your selection logic)
    std::cout << "Initializing System..." << std::endl;
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
    // 1. Setup DB
    std::cout << "Initializing Database..." << std::endl;
    SQLiteLogger dbLogger("voyage_data.db");

   // -----------------------------------------------------
    // 2. UI PHASE (NCurses Mode Starts Here)
    // -----------------------------------------------------
    // The moment this object is created, the screen clears!
    GPSDashboard dashboard;

    // Attach Dashboard to Parser
    parser.onFix([&dashboard](const GPSData& d) {
        dashboard.update(d);
    });

    // Attach Database Logger (Silent background task)
    parser.onFix([&dbLogger](const GPSData& d) {
        if (d.isValid) dbLogger.log(d);
    });

    // 3. START ENGINE
    std::thread producer(gpsReaderTask, source.get(), std::ref(buffer));
    std::thread consumer(dataProcessorTask, &parser, std::ref(buffer));

    producer.join();
    consumer.join();

    return 0;
}
