# **NMEA-0183 Navigation Data Parser & Integration System**

## **Executive Summary**

This project is a modular, high-performance C++ library designed to ingest, validate, and parse **NMEA-0183** marine navigation data.  
It represents a complete Embedded Systems software stack, moving beyond simple string parsing to a robust **Event-Driven Architecture**. The system features a **Hardware Abstraction Layer (HAL)** for data ingestion, a **Factory Pattern** for dynamic sentence parsing, and an **Observer Pattern** for decoupling the core driver from consumer systems (such as displays, data loggers, or autopilots).

## **System Architecture**

The system is built on four core architectural pillars:

1. **The Abstraction Layer (INMEASource):** Uses Dependency Injection to treat Serial ports and UDP sockets identically.  
2. **The Gatekeeper (NMEAParser):** A centralized factory that validates checksums and dispatches logic.  
3. **The Strategies (INMEASentence):** Polymorphic classes that handle specific sentence logic (GPGGA vs GPRMC).  
4. **The Event Bus (Observer Pattern):** Allows multiple systems to subscribe to GPS updates via callbacks, enabling a "Push" data model.

```cpp
classDiagram  
    class INMEASource {  
        \<\<interface\>\>  
        \+readLine() string  
        \+open() bool  
    }  
    class SerialSource  
    class UDPSource  
    INMEASource \<|-- SerialSource  
    INMEASource \<|-- UDPSource  
      
    class NMEAParser {  
        \-listeners : vector\<function\>  
        \+onFix(callback)  
        \+parse(string)  
        \-notifyListeners()  
    }  
      
    class INMEASentence {  
        \<\<interface\>\>  
        \+parse(tokens, data)  
    }  
    class GPGGASentence  
    class GPRMCSentence  
```      
    NMEAParser ..\> INMEASentence : Instantiates (Factory)  
    NMEAParser \--\> "Many" ConsumerSystem : Notifies (Observer)  
    INMEASentence \<|-- GPGGASentence  
    INMEASentence \<|-- GPRMCSentence

## **Technical Capabilities**

### **1\. Hardware Integration (HAL)**

* **Serial/UART:** Implements low-level POSIX termios configuration to interface with physical GPS receivers at **4800 Baud (8N1)**. Compatible with Linux (/dev/ttyUSB0) and macOS (/dev/ttysXXX).  
* **UDP Networking:** Listens on Port **10110** (Standard Marine WiFi protocol) to accept NMEA data broadcast over local networks.

### **2\. Event-Driven Design**

* **Zero-Coupling:** The main driver loop does not know *what* happens to the data. It simply pumps the engine.  
* **Multi-Subscriber:** Supports multiple simultaneous outputs (e.g., a Console Display and a Database Logger) attached to the same parser instance via parser.onFix().

### **3\. Advanced Parsing Logic**

* **Polymorphism:** The system identifies the NMEA Header (e.g., GPRMC) and spawns the correct derived class to handle the parsing strategy.  
* **Coordinate Math:** Converts legacy NMEA DDMM.MMMM (Degrees-Minutes) into standard Decimal Degrees for modern GIS compatibility.  
* **Safety:** Implements **XOR Checksum Validation** to reject corrupted packets before they enter the processing pipeline.

## **Building and Running**

The project utilizes a Makefile for streamlined compilation and build management.

### **Prerequisites**

* A C++ compiler supporting C++17 (e.g., g++, clang).  
* make utility.

### **Build Instructions**

To compile the project and link all dependencies:  
```bash
make
```
### **Running the Application**

To run the executable immediately after building:  
```bash
make run
```
*Alternatively, you can run the binary directly:*  
```bash
./nmea\_engine
```
### **Cleaning Up**

To remove object files and the executable:  
make clean

## **Testing with Hardware Simulation**

Since physical GPS hardware is not always available, the system was verified using industry-standard simulation tools.

### **Scenario A: Simulating Network Data (UDP)**

To simulate a yacht broadcasting GPS data over WiFi:

1. Start the application and select **Mode 1 (UDP)**.  
2. Run netcat in a separate terminal to fire a packet: 
```bash 
   echo "\\$GPGGA,123519,4807.038,N,01131.000,E,1,08,0.9,545.4,M,46.9,M,,\*47" | nc \-u 127.0.0.1 10110
```
### **Scenario B: Simulating Serial Hardware (UART)**

To simulate a physical serial cable using socat:

1. **Create Virtual Ports:**  
```bash   
   socat \-d \-d pty,raw,echo=0 pty,raw,echo=0
```
   *Note output paths: e.g., /dev/ttys005 (Receiver) and /dev/ttys006 (Transmitter).*  
2. Start Application:  
   Select Mode 2 (Serial) and enter the Receiver path (e.g., /dev/ttys005).  
3. Inject Data:  
   Write directly to the Transmitter port:  
```bash   
   echo "\\$GPRMC,123519,A,4807.038,N,01131.000,E,022.4,084.4,230394,003.1,W\*6A" \> /dev/ttys006
```
## **Project Timeline (SDLC Simulation)**

This project was developed following a strict Agile workflow with atomic commits:

* **Phase 1:** Interface Design & Type Definitions (GPSData struct).  
* **Phase 2:** Tooling Implementation (Hex conversion, String tokenization).  
* **Phase 3:** Checksum Validation Logic (Security/Integrity).  
* **Phase 4:** Core Parsing Logic (GPGGA implementation).  
* **Phase 5:** Driver Implementation (Main Loop).  
* **Phase 6:** **Architectural Refactoring** \-\> Migrated to Factory Pattern & Polymorphism.  
* **Phase 7:** **System Integration** \-\> Implemented Serial and UDP data sources.  
* **Phase 8:** **Event-Driven Architecture** \-\> Implemented Observer Pattern for decoupled subscriptions.

Author: Umar Arshid