# **NMEA-0183 Navigation Data Parser & Integration System**

## **Executive Summary**

This project is a modular, fault-tolerant C++ library designed to ingest, validate, parse, persist, and visualize **NMEA-0183** marine navigation data.  
It represents a complete Embedded Systems software stack. The system has evolved from a simple string parser into a robust **Real-Time Engine** capable of handling high-frequency data bursts (10Hz+) without packet loss. It features a **Hardware Abstraction Layer (HAL)**, a **Producer-Consumer Architecture** for concurrency, an embedded **SQLite Database** for tracking, and a professional **Text User Interface (TUI)** for real-time monitoring.

## **System Architecture**

The system is built on seven core architectural pillars:

1. **The Abstraction Layer (INMEASource):** Uses Dependency Injection to treat Serial ports and UDP sockets identically.  
2. **The Gatekeeper (NMEAParser):** A centralized factory that validates checksums and dispatches logic.  
3. **The Strategies (INMEASentence):** Polymorphic classes that handle specific sentence logic (GPGGA vs GPRMC).  
4. **The Event Bus (Observer Pattern):** Allows multiple systems to subscribe to GPS updates via callbacks.  
5. **The Shock Absorber (Concurrency):** A Thread-Safe Queue separating Ingestion from Processing.  
6. **The Black Box (Persistence):** An RAII-compliant SQLite wrapper that securely logs voyage data to disk.  
7. **The Face (User Interface):** An NCurses-based dashboard that renders live data without console scrolling.

graph LR  
    subgraph "Thread A: Hardware (Producer)"  
    A\[UDP / Serial Port\] \--\>|Raw Bytes| B(INMEASource)  
    B \--\>|Push| C{SafeQueue\<T\>}  
    end  
      
    subgraph "Thread B: Logic (Consumer)"  
    C \--\>|Pop| D\[NMEAParser\]  
    D \--\>|Factory| E\[GPGGA / GPRMC Logic\]  
    E \--\>|Notify| F\[Observer Callbacks\]  
    end  
      
    F \--\> G\[SQLite Database\]  
    F \--\> H\[TUI Dashboard\]

## **Technical Capabilities**

### **1\. High-Performance Concurrency**

* **Producer-Consumer Model:** Implements a multi-threaded architecture where **Thread A** handles high-speed IO (Ingestion) and **Thread B** handles CPU-intensive Logic/IO (Processing).  
* **Backpressure Management:** Verified via stress testing to handle **10Hz** input streams even when downstream consumers lag (simulated 2Hz throughput), ensuring **Zero Packet Loss**.

### **2\. User Interface (TUI)**

* **NCurses Integration:** Replaces scrolling console logs with a static, professional Terminal User Interface.  
* **Real-Time Rendering:** Updates GPS coordinates, Speed, and Heading in-place.  
* **Thread Isolation:** Manages console access carefully to prevent "garbled text" race conditions between logging threads and drawing threads.

### **3\. Data Persistence (SQLite)**

* **Embedded Database:** Integrates libsqlite3 to persist GPS fixes to a local .db file using **Prepared Statements** for security and speed.

### **4\. Hardware Integration (HAL)**

* **Serial/UART:** Implements low-level POSIX termios configuration (4800 Baud, 8N1).  
* **UDP Networking:** Listens on Port **10110** (Marine WiFi protocol).

## **Building and Running**

### **Prerequisites**

* C++17 Compiler (GCC/Clang)  
* **SQLite3** (libsqlite3-dev / brew install sqlite3)  
* **NCurses** (libncurses-dev / brew install ncurses)

### **Compilation**

The project uses a Makefile for streamlined compilation with \-pthread, \-lsqlite3, and \-lncurses support.  
make

### **Running the System**

The application runs in an infinite loop.  
```bash
./nmea\_engine
```
*Note: The application will ask for configuration (UDP/Serial) in standard text mode before launching the TUI dashboard.*

## **Testing with Hardware Simulation**

Since physical GPS hardware is not always available, the system was verified using industry-standard simulation tools.

### **Scenario A: Simulating Network Data (UDP)**

To simulate a yacht broadcasting GPS data over WiFi:

1. Start the application and select **Mode 1 (UDP)**.  
2. Run netcat in a separate terminal to fire a packet:  
```bash
   echo "\\$GPGGA,123519,4807.038,N,01131.000,E,1,08,0.9,545.4,M,46.9,M,,\*47" | nc \-u 127.0.0.1 10110
```
   *Result: The TUI Dashboard updates instantly.*

### **Scenario B: Verifying Persistence**

1. Run the application and inject data.  
2. Inspect the generated voyage\_data.db file:  
```bash
   sqlite3 voyage\_data.db "SELECT \* FROM tracklog;"
```
## **Project Timeline (SDLC Simulation)**

This project was developed following a strict Agile workflow with atomic commits:

* **Phase 1-4:** Core Logic (Types, Tooling, Checksum, Parsing).  
* **Phase 5:** Driver Implementation.  
* **Phase 6:** **Architectural Refactoring** \-\> Migrated to Factory Pattern.  
* **Phase 7:** **System Integration** \-\> Implemented Serial and UDP HAL.  
* **Phase 8:** **Event-Driven Architecture** \-\> Implemented Observer Pattern.  
* **Phase 10:** **Concurrency** \-\> Implemented Thread-Safe Producer-Consumer Queue.  
* **Phase 11:** **Persistence** \-\> Implemented SQLite Logger.  
* **Phase 12:** **User Interface** \-\> Implemented NCurses TUI Dashboard.

Author: Umar Arshid