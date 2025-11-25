# **NMEA-0183 Navigation Data Parser & Integration System**

## **Executive Summary**

This project is a modular, high-performance C++ library designed to ingest, validate, and parse **NMEA-0183** marine navigation data.  
Unlike simple string parsers, this system employs an **Object-Oriented Architecture** using the **Factory Pattern** to dynamically instantiate sentence parsers ($GPGGA, $GPRMC). It features a **Hardware Abstraction Layer (HAL)** that decouples the parsing logic from the data source, allowing seamless switching between File I/O, **UDP Network Streams** (Simulating WiFi Yacht Bridges), and **Serial/UART Ports** (Simulating physical GPS hardware).

## **System Architecture**

The system is built on three core architectural pillars:

1. **The Abstraction Layer (INMEASource):** Uses Dependency Injection to treat Serial ports and UDP sockets identically.  
2. **The Gatekeeper (NMEAParser):** A centralized factory that validates checksums and dispatches logic.  
3. **The Strategies (INMEASentence):** Polymorphic classes that handle specific sentence logic (GPGGA vs GPRMC).
```cpp
classDiagram  
    class INMEASource {  
        \<\<interface\>\>  
        \+readLine() string  
        \+open() bool  
    }  
    class SerialSource {  
        \+termios config  
        \+read()  
    }  
    class UDPSource {  
        \+socket bind  
        \+recvfrom()  
    }  
      
    INMEASource \<|-- SerialSource  
    INMEASource \<|-- UDPSource  
      
    class NMEAParser {  
        \+parse(string) GPSData  
        \-validateChecksum()  
    }  
      
    class INMEASentence {  
        \<\<interface\>\>  
        \+parse(tokens, data)  
    }  
    class GPGGASentence  
    class GPRMCSentence  
 ```     
    NMEAParser ..\> INMEASentence : Instantiates (Factory)  
    INMEASentence \<|-- GPGGASentence  
    INMEASentence \<|-- GPRMCSentence

## **Technical Capabilities**

### **1\. Hardware Integration (HAL)**

* **Serial/UART:** Implements low-level POSIX termios configuration to interface with physical GPS receivers at **4800 Baud (8N1)**. Compatible with Linux (/dev/ttyUSB0) and macOS (/dev/ttysXXX).  
* **UDP Networking:** Listens on Port **10110** (Standard Marine WiFi protocol) to accept NMEA data broadcast over local networks.

### **2\. Advanced Parsing Logic**

* **Polymorphism:** The system identifies the NMEA Header (e.g., GPRMC) and spawns the correct derived class to handle the parsing strategy.  
* **Coordinate Math:** Converts legacy NMEA DDMM.MMMM (Degrees-Minutes) into standard Decimal Degrees for modern GIS compatibility.  
* **Safety:** Implements **XOR Checksum Validation** to reject corrupted packets before they enter the processing pipeline.

## **Building and Running**

### **Compilation**

The project uses standard C++17.  
```bash
g++ main.cpp NMEAParser.cpp \-o nmea\_engine
```
### **Running the System**

The application runs in an infinite loop, acting as a real-time driver.  
```bash
./nmea\_engine
```
*Follow the on-screen prompts to select your data source.*

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
 ``` bash 
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

Author: \Umar Arshid\