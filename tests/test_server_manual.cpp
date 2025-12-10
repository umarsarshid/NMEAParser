#include "WebServer.h"
#include <thread>
#include <chrono>

int main() {
    WebServer server;

    // Launch server in a background thread
    std::thread t([&server](){
        server.run();
    });

    // Simulate the main app broadcasting data
    for (int i = 0; i < 5; i++) {
        std::this_thread::sleep_for(std::chrono::seconds(2));
        server.broadcast("{\"lat\": 48.1, \"status\": \"ping\"}");
        std::cout << "Broadcasted Ping " << i << std::endl;
    }

    // In a real app we would handle shutdown gracefully, 
    // but for this test we just kill it (Ctrl+C to exit)
    t.join();
    return 0;
}