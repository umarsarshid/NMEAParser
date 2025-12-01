#pragma once
#include "crow.h"
#include <vector>
#include <mutex>
#include <algorithm>
#include <string>
#include <iostream>

class WebServer {
private:
    crow::SimpleApp app; // The Crow Application
    
    // List of active WebSocket connections
    // We store pointers because Crow manages the actual connection objects
    std::vector<crow::websocket::connection*> connections;
    
    // Mutex to protect the connection list from race conditions
    std::mutex mtx;

public:
    WebServer();

    // Blocking call that starts the server loop
    void run();

    // Sends a JSON string to all connected clients
    void broadcast(const std::string& message);
};