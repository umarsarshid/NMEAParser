#include "WebServer.h"
#include <algorithm> // Required for std::remove

WebServer::WebServer() {
    // 1. Static Asset Route (The Frontend)
    // Eventually, this will serve the React App
    CROW_ROUTE(app, "/")([](){
        return "<h1>NMEA Engine Online</h1><p>WebSocket endpoint at /ws</p>";
    });

    // 2. WebSocket Route (The Real-Time Stream)
    CROW_WEBSOCKET_ROUTE(app, "/ws")
        .onopen([this](crow::websocket::connection& conn) {
            std::lock_guard<std::mutex> lock(mtx);
            connections.push_back(&conn);
            std::cout << "[Web] Client Connected! Total: " << connections.size() << std::endl;
        })
        .onclose([this](crow::websocket::connection& conn, std::string reason) {
            std::lock_guard<std::mutex> lock(mtx);
            // Fancy STL way to remove an item from a vector by value
            connections.erase(
                std::remove(connections.begin(), connections.end(), &conn), 
                connections.end()
            );
            std::cout << "[Web] Client Disconnected. Total: " << connections.size() << std::endl;
        })
        .onmessage([](crow::websocket::connection&, std::string data, bool is_binary) {
            // We generally ignore incoming data for a dashboard, 
            // but you could add control logic here (e.g., "Reset Trip")
            (void)data;      // Silence unused variable warning
            (void)is_binary; // Silence unused variable warning
        });
}

void WebServer::run() {
    std::cout << "[Web] Starting Server on Port 8080..." << std::endl;
    // Set log level to Warning to stop Crow from spamming the console
    app.loglevel(crow::LogLevel::Warning);
    app.port(8080).multithreaded().run();
}

void WebServer::broadcast(const std::string& message) {
    std::lock_guard<std::mutex> lock(mtx);
    
    // Loop through all active connections and send data
    for (auto* conn : connections) {
        conn->send_text(message);
    }
}