# **NMEA-0183 Navigation Data Parser**

## **Project Overview**

This project is a high-performance C++ library designed to ingest, validate, and parse **NMEA-0183** standard navigation data (specifically $GPGGA sentences) from simulated GPS receivers.  
The goal was to build a memory-safe, robust parsing engine that converts raw ASCII serial streams into structured C++ objects (GPSData), demonstrating core competencies in embedded systems logic and string manipulation.

### **Key Features**

* **Protocol Compliance:** Full implementation of NMEA-0183 checksum validation (XOR verification).  
* **Coordinate Conversion:** Custom algorithm to translate NMEA DDMM.MMMM format into standard Decimal Degrees.  
* **Fault Tolerance:** Handles corrupted strings, empty fields (signal loss), and checksum mismatches without crashing.  
* **Modern C++:** Utilizes C++17 standards, including std::string\_view for efficient memory usage and struct based data encapsulation.

## **System Design & Architecture**

### **1\. The Data Flow**

The system follows a linear pipeline architecture to ensure invalid data is rejected as early as possible (Fail-Fast methodology).  
graph LR  
    A\[Raw String\] \--\> B{Checksum Validator}  
    B \-- Invalid \--\> C\[Reject Packet\]  
    B \-- Valid \--\> D\[Tokenizer\]  
    D \--\> E\[Header Check\]  
    E \--\> F\[Coordinate Math\]  
    F \--\> G\[GPSData Object\]

### **Design Decisions**

* **Checksum "Gatekeeper":** Before any expensive parsing (tokenization/conversion) occurs, the validateChecksum function performs a bitwise XOR operation on the payload. This prevents CPU cycles from being wasted on corrupted packets.  
* **Data Encapsulation:** All parsed data is stored in a POD (Plain Old Data) GPSData struct with a specific isValid flag. This allows the consumer application to check data integrity in O(1) time.  
* **Safety First:** The parser checks for empty strings before attempting std::stoi or std::stod conversions, addressing the common edge case where GPS receivers transmit empty fields (e.g., ,,) when satellite lock is lost.

## **Technical Implementation Details**

### **Domain Logic: Coordinate Conversion**

NMEA devices transmit coordinates in a legacy format: 4807.038,N (48 degrees, 07.038 minutes). To make this compatible with modern mapping APIs (Google Maps/Mapbox), the parser implements the following conversion logic:  
Decimal \= Degrees \+ (Minutes / 60.0);  
if (Direction \== 'S' || Direction \== 'W') Decimal \*= \-1;

### **Checksum Algorithm**

The parser verifies data integrity by calculating the XOR sum of all bytes between $ and \*:  
int calculatedChecksum \= 0;  
for (char c : payload) {  
    calculatedChecksum ^= c; // Bitwise XOR  
}

## **🚀 Building and Running**

This project is designed to be compiler-agnostic and relies only on the C++ Standard Library (STL).

### **Prerequisites**

* A C++ compiler (GCC, Clang, or MSVC) supporting C++17.

### **Compilation**

g++ main.cpp NMEAParser.cpp \-o gps\_parser

### **Execution**

./gps\_parser

### **Sample Output**

\--- GPS NMEA Parser System v1.0 \---

\[RX\] Data Received: $GPGGA,123519,4807.038,N,01131.000,E,1,08,0.9,545.4,M,46.9,M,,\*47  
  \-\> STATUS: \[VALID\]  
     Lat: 48.117300 | Lon: 11.516667 | Alt: 545.400000m  
\----------------------------------------------------  
\[RX\] Data Received: $GPGGA,123520,4807.040,N,01131.010,E,1,08,0.9,545.4,M,46.9,M,,\*00  
  \-\> STATUS: \[INVALID / CHECKSUM ERROR\]

## **Development Process (SDLC)**

This project was executed following a simulated 1-week Agile Sprint:

* **Phase 1: Requirements & Types:** Defined the GPSData contract and interface.  
* **Phase 2: Tooling:** Implemented helper utilities (split, hexToDecimal) using TDD (Test Driven Development).  
* **Phase 3: The Gatekeeper:** Implemented and unit-tested the XOR checksum logic.  
* **Phase 4: Core Logic:** Implemented the GPGGA parsing and coordinate math.  
* **Phase 5: Integration:** Built the driver loop to simulate real-time data streams.

## **Future Improvements**

* **Integration:** Add support for termios to read from a physical Linux Serial Port (/dev/ttyUSB0).  
* **Expansion:** Add support for $GPRMC (Recommended Minimum Navigation Information) sentences.  
* **Optimization:** Refactor tokenization to use std::string\_view completely to eliminate temporary string allocations.

Author: Umar Arshid