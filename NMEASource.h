#pragma once
#include <string>
#include <vector>
#include <fcntl.h> 
#include <termios.h>

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

class SerialSource : public INMEASource {
    int serial_fd;
    std::string device;
    char buffer[1]; // Read 1 byte at a time

public:
    SerialSource(std::string dev) : device(dev), serial_fd(-1) {}

    bool open() override {
        // 1. Open the device
        serial_fd = ::open(device.c_str(), O_RDWR | O_NOCTTY | O_SYNC);
        if (serial_fd < 0) {
           // NEW LINE: Print the actual system error
            perror("Serial Open Error"); 
            return false;
        }

        // 2. Configure Termios (The Hard Part)
        struct termios tty;
        if (tcgetattr(serial_fd, &tty) != 0) return false;

        cfsetospeed(&tty, B4800); // NMEA Standard Speed
        cfsetispeed(&tty, B4800);

        tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8; // 8-bit chars
        tty.c_cflag |= (CLOCAL | CREAD); // ignore modem controls, enable reading
        tty.c_cflag &= ~(PARENB | PARODD); // shut off parity
        tty.c_cflag &= ~CSTOPB;
        tty.c_cflag &= ~CRTSCTS; // No hardware flow control

        // Canonical mode (Read line by line, not byte by byte)
        tty.c_lflag |= ICANON; 

        if (tcsetattr(serial_fd, TCSANOW, &tty) != 0) return false;

        std::cout << "Serial: Connected to " << device << " @ 4800 Baud" << std::endl;
        return true;
    }

    std::string readLine() override {
        std::string sentence;
        char c;
        // Simple blocking read loop
        while(true) {
            int n = ::read(serial_fd, &c, 1);
            if (n > 0) {
                if (c == '\n') break; // End of line
                sentence += c;
            } else {
                break; // Error or disconnect
            }
        }
        return sentence;
    }

    void close() override {
        ::close(serial_fd);
    }
};