#pragma once
#include <string>
#include <vector>

// Abstract Base Class for any Data Source
class INMEASource {
public:
    virtual ~INMEASource() = default;
    
    // Blocking call that waits for the next line of data
    virtual std::string readLine() = 0;
    
    // Setup connection (open port, bind socket, etc.)
    virtual bool open() = 0;
    
    // Close connection
    virtual void close() = 0;
};

#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>
#include <iostream>

class UDPSource : public INMEASource {
    int sockfd;
    int port;
    char buffer[1024];

public:
    UDPSource(int port = 10110) : port(port), sockfd(-1) {}

    bool open() override {
        // 1. Create UDP Socket
        if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
            std::cerr << "UDP: Socket creation failed" << std::endl;
            return false;
        }

        // 2. Bind to Port (e.g., 10110)
        sockaddr_in servaddr{};
        servaddr.sin_family = AF_INET;
        servaddr.sin_addr.s_addr = INADDR_ANY; // Listen on all interfaces
        servaddr.sin_port = htons(port);

        if (bind(sockfd, (const sockaddr*)&servaddr, sizeof(servaddr)) < 0) {
            std::cerr << "UDP: Bind failed" << std::endl;
            return false;
        }
        std::cout << "UDP: Listening on port " << port << std::endl;
        return true;
    }

    std::string readLine() override {
        // Wait for packet (Blocking)
        socklen_t len;
        int n = recvfrom(sockfd, (char*)buffer, 1024, 
                        MSG_WAITALL, nullptr, &len);
        buffer[n] = '\0'; // Null terminate
        return std::string(buffer);
    }

    void close() override {
        ::close(sockfd);
    }
};