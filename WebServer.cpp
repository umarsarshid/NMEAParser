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

    // 2. Assets Route: Serve JS/CSS files
    // Catch-all for anything inside /assets/
    CROW_ROUTE(app, "/assets/<string>")([](const crow::request&, crow::response& res, std::string filename){
        std::string path = "../frontend/dist/assets/" + filename;
        std::string content = readFile(path);
        
        if (content.empty()) {
            res.code = 404;
            res.write("Not Found");
        } else {
            // Simple MIME Type detection
            if (filename.find(".js") != std::string::npos) {
                res.set_header("Content-Type", "application/javascript");
            } else if (filename.find(".css") != std::string::npos) {
                res.set_header("Content-Type", "text/css");
            }
            res.write(content);
        }
        res.end();
    });

    // 3. WebSocket Route (Unchanged)
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