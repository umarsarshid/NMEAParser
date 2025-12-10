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
std::unique_ptr<INMEASource> globalSource;
SafeQueue<RawPacket> buffer;

// 2. Minimalist Signal Handler
// REMOVED: std::cout calls (Unsafe in TUI mode)
void signalHandler(int signum) {
    (void)signum; // Silence unused warning
    running = false;
    
    // Wake up the Reader
    if (globalSource) {
        globalSource->close(); 
    }
    // Wake up the Consumer
    buffer.shutdown();
}

void gpsReaderTask(std::string id,INMEASource* source, SafeQueue<RawPacket>& queue) {
    // Producer is silent (no cout) to protect TUI
    while (running) {
        std::string line = source->readLine();
        if (!line.empty()) {
            // Wrap the data with the ID
            queue.push({ id, line });
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
    std::unique_ptr<INMEASource> source;

    // -----------------------------------------------------
    // CONFIGURATION PHASE (Standard Terminal)
    // -----------------------------------------------------
    std::cout << "=== NMEA ENGINE SETUP ===" << std::endl;
    std::cout << "Select Source: [1] UDP Network  [2] Serial Port: ";
    int choice;
    if (!(std::cin >> choice)) return 0; // Handle bad input

    if (choice == 1) {
        source = std::make_unique<UDPSource>(10110);
    } else {
        std::string port;
        std::cout << "Enter Serial Device (e.g., /dev/ttys005): ";
        std::cin >> port;
        source = std::make_unique<SerialSource>(port);
    }

    globalSource = std::move(source);
    if (!globalSource->open()) return -1;

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
        std::thread producer(gpsReaderTask, "alpha", globalSource.get(), std::ref(buffer));
        std::thread consumer(dataProcessorTask, &parser, std::ref(buffer));
        std::thread webThread([&webServer](){ webServer.run(); });

        // Enable non-blocking input for the main loop
        // This allows us to catch 'q' without blocking the UI
        nodelay(stdscr, TRUE);

        while(running) {
            // Check for user quit 'q' or 'Q'
            int ch = getch();
            if (ch == 'q' || ch == 'Q') {
                signalHandler(SIGINT); // Trigger shutdown manually
            }
            
            // Check if Ncurses resized (Optional robustness)
            if (ch == KEY_RESIZE) {
                // handle resize if needed
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }

        // --- CLEANUP SEQUENCE ---
        // We join threads HERE while TUI is still active (or just blank)
        // so we don't print "Joined" on top of the dashboard.
        
        if (producer.joinable()) producer.join();
        if (consumer.joinable()) consumer.join();
        webThread.detach(); 

    } // <--- DESTRUCTOR FIRES HERE. endwin() called. Terminal restored.

    // -----------------------------------------------------
    // EXIT PHASE (Back to Normal Terminal)
    // -----------------------------------------------------
    std::cout << "[System] Engine stopped safely." << std::endl;
    std::cout << "[System] Database closed." << std::endl;

    return 0;
}