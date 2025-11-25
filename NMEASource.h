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