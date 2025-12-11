#include "WebServer.h"
#include <algorithm>
#include <fstream>  // <--- NEW
#include <sstream>  // <--- NEW

// Helper to read file content from disk
std::string readFile(const std::string& path) {
    std::ifstream in(path, std::ios::in | std::ios::binary);
    if (in) {
        std::ostringstream contents;
        contents << in.rdbuf();
        in.close();
        return contents.str();
    }
    return "";
}

WebServer::WebServer() {
    // 1. Root Route: Serve the React "index.html"
    // Note: We assume the "dist" folder is next to the executable
    CROW_ROUTE(app, "/")([](const crow::request&, crow::response& res){
        std::string content = readFile("../frontend/dist/index.html");
        if (content.empty()) {
            res.code = 404;
            res.write("Error: frontend/dist/index.html not found. Did you run 'npm run build'?");
        } else {
            res.set_header("Content-Type", "text/html");
            res.write(content);
        }
        res.end();
    });

    // 2. Assets Route: Serve JS/CSS/IMG files
    // Catch-all for anything inside /assets/
    CROW_ROUTE(app, "/assets/<string>")([](const crow::request&, crow::response& res, std::string filename){
        std::string path = "../frontend/dist/assets/" + filename;
        std::string content = readFile(path);
        
        if (content.empty()) {
            res.code = 404;
            res.write("Not Found");
        } else {
            // MIME Type detection
            if (filename.find(".js") != std::string::npos) {
                res.set_header("Content-Type", "application/javascript");
            } else if (filename.find(".css") != std::string::npos) {
                res.set_header("Content-Type", "text/css");
            } else if (filename.find(".png") != std::string::npos) {
                res.set_header("Content-Type", "image/png");
            } else if (filename.find(".jpg") != std::string::npos || filename.find(".jpeg") != std::string::npos) {
                res.set_header("Content-Type", "image/jpeg");
            } else if (filename.find(".svg") != std::string::npos) {
                res.set_header("Content-Type", "image/svg+xml");
            }
            res.write(content);
        }
        res.end();
    });

    // 3. WebSocket Route
    CROW_WEBSOCKET_ROUTE(app, "/ws")
        .onopen([this](crow::websocket::connection& conn) {
            std::lock_guard<std::mutex> lock(mtx);
            connections.push_back(&conn);
        })
        .onclose([this](crow::websocket::connection& conn, std::string reason) {
            std::lock_guard<std::mutex> lock(mtx);
            connections.erase(
                std::remove(connections.begin(), connections.end(), &conn), 
                connections.end()
            );
        })
        .onmessage([](crow::websocket::connection&, std::string data, bool is_binary) {
            // Ignore input
            (void)data; (void)is_binary;
        });
}
void WebServer::run() {
    // std::cout << "[Web] Starting Server on Port 8080..." << std::endl;
    app.loglevel(crow::LogLevel::Warning);
    
    try {
        // Try to start the server
        app.port(8080).multithreaded().run();
    } catch (const std::exception& e) {
        // If port 8080 is busy, print error instead of crashing
        std::cerr << "\n[Web Error] Failed to start server: " << e.what() << std::endl;
        std::cerr << "Is port 8080 already in use?\n" << std::endl;
    }
}

void WebServer::broadcast(const std::string& message) {
    std::lock_guard<std::mutex> lock(mtx);
    
    // Loop through all active connections and send data
    for (auto* conn : connections) {
        conn->send_text(message);
    }
}