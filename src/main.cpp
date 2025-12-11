#include <csignal>
#include <iostream>
#include <memory>
#include <thread>
#include <atomic>

// --- INCLUDE ORDER MATTERS FOR MACROS ---
#include "JSONUtils.h"
#include "WebServer.h"
#include "NMEAParser.h"
#include "NMEASource.h"
#include "SafeQueue.h"
#include "SQLiteLogger.h"
#include "GPSDashboard.h" // NCurses last to avoid "OK" conflict

struct RawPacket {
    std::string sourceID;
    std::string nmeaString;
};

// 1. Global handles for cleanup
std::atomic<bool> running(true);
SafeQueue<RawPacket> buffer;

// 2. Minimalist Signal Handler (no global source)
void signalHandler(int signum) {
    (void)signum; // Silence unused warning
    running = false;
}

void gpsReaderTask(std::string id,INMEASource* source, SafeQueue<RawPacket>& queue) {
    // Producer is silent (no cout) to protect TUI
    while (running) {
        std::string line = source->readLine();
        if (!line.empty()) {
            // Wrap the data with the ID
            queue.push({ id, line });
        } else {
            //Read Failure (e.g., source closed)
            break;
        }
    }
}

void dataProcessorTask(NMEAParser* parser, SafeQueue<RawPacket>& queue) {
    RawPacket packet;
    while (running) {
        if (!queue.waitAndPop(packet)) break; 

        GPSData data = parser->parse(packet.nmeaString);
        data.ID = packet.sourceID;

        // Trigger observers (DB, Web, TUI)
        parser->notifyListeners(data);
    }
}

int main() {
    // Register Signals
    std::signal(SIGINT, signalHandler); 
    std::signal(SIGTERM, signalHandler);

    NMEAParser parser;

    // -----------------------------------------------------
    // CONFIGURATION PHASE (Standard Terminal)
    // -----------------------------------------------------
    std::cout << "=== NMEA ENGINE SETUP ===" << std::endl;
    auto source1 = std::make_unique<UDPSource>(10110); // Source 2: A secondary UDP stream (A Drone Boat)
    auto source2 = std::make_unique<UDPSource>(10111); // Different port!

    if (!source1->open() || !source2->open()) {
        std::cerr << "Failed to open UDP ports!" << std::endl;
        return -1;
    }

    SQLiteLogger dbLogger("voyage_data.db");
    WebServer webServer;

    // Wire up Observers
    parser.onFix([&webServer](const GPSData& d) {
        if (d.isValid) webServer.broadcast(GPSDataToJson(d));
    });
    
    parser.onFix([&dbLogger](const GPSData& d) {
        if (d.isValid) dbLogger.log(d);
    });

    // -----------------------------------------------------
    // RUNTIME PHASE (NCurses Scope)
    // -----------------------------------------------------
    // We wrap this in a block {} so destructors run BEFORE main exits
    {
        GPSDashboard dashboard; // Clears screen, enters TUI mode
        
        parser.onFix([&dashboard](const GPSData& d) {
            dashboard.update(d);
        });

        // Launch Threads
        // std::thread producer(gpsReaderTask, "alpha", globalSource.get(), std::ref(buffer));
        // Launch TWO Producers
        std::thread t1(gpsReaderTask, "Alpha", source1.get(), std::ref(buffer));
        std::thread t2(gpsReaderTask, "Bravo", source2.get(), std::ref(buffer));
        std::thread consumer(dataProcessorTask, &parser, std::ref(buffer));
        std::thread webThread([&webServer](){ webServer.run(); });

    
        while(running) {
            // Check for user quit 'q' or 'Q'
            int ch = getch();
            if (ch == 'q' || ch == 'Q') {
                running = false;
            }
            
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }

        // --- CLEANUP SEQUENCE ---
        // We join threads HERE while TUI is still active (or just blank)
        // so we don't print "Joined" on top of the dashboard.
                // 1. Wake up Consumers
        buffer.shutdown(); 

        // 2. Wake up Producers (CRITICAL FIX)
        // Closing the socket forces recvfrom() to return error, 
        // allowing t1 and t2 to exit their while loops.
        source1->close();
        source2->close();
        // 3. Join Threads
        if (t1.joinable()) t1.join();
        if (t2.joinable()) t2.join();
        if (consumer.joinable()) consumer.join();

        // WebServer is tricky to stop cleanly without internal support, 
        // but detaching allows us to exit main.
        webThread.detach(); 

    } // <--- DESTRUCTOR FIRES HERE. endwin() called. Terminal restored.

    // -----------------------------------------------------
    // EXIT PHASE
    // -----------------------------------------------------
    std::cout << "[System] Resources released." << std::endl;
    std::cout << "[System] Database closed." << std::endl;
    std::cout << "[System] Goodbye." << std::endl;


    return 0;
}