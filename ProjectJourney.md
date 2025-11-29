# **Engineering Log: NMEA Navigation System**

**Summary:** Evolution of a C++17 embedded system from a string parser to a multi-threaded, hardware-integrated navigation engine.

## **Phase 1: Core Parsing Logic**

**Objective:** Parse legacy NMEA-0183 ($GPGGA) data streams.

* **Problem:** NMEA standards use non-standard coordinate formats (DDMM.MMMM) and require checksum validation to prevent processing corrupted serial data.  
* **Implementation:** \* Implemented custom math logic to convert Degrees-Minutes to Decimal Degrees.  
  * Wrote an XOR checksum validator to verify data integrity before parsing.  
* **Outcome:** Robust ingestion of raw ASCII streams.

// Coordinate Conversion Logic  
double convert(string val) {  
    // 4807.038 \-\> 48 \+ (07.038 / 60\) \-\> 48.1173  
}

## **Phase 2: Architectural Refactoring (Factory Pattern)**

**Objective:** Support multiple sentence types ($GPRMC, $GPZDA) without modifying core logic.

* **Problem:** The initial monolithic parse() function used a massive if/else chain, violating the Open/Closed Principle. Adding a new sentence type required modifying tested code.  
* **Implementation:** \* Defined an abstract base class INMEASentence.  
  * Implemented the **Factory Pattern** to dispatch logic based on the NMEA header string.  
* **Outcome:** New sentence types can be added as isolated classes without touching the main parser.

## **Phase 3: Hardware Abstraction Layer (HAL)**

**Objective:** Decouple parsing logic from the physical data source.

* **Problem:** The system needed to support both physical GPS hardware (Serial/UART) and simulation data (UDP Network), but hardcoding these sources created tight coupling.  
* **Implementation:** \* Created INMEASource interface using **Dependency Injection**.  
  * **Serial:** Implemented termios configuration for 4800 Baud 8N1 (standard marine hardware).  
  * **Network:** Implemented POSIX Sockets for UDP listening on port 10110\.  
* **Outcome:** Seamless switching between hardware and network sources via configuration.

## **Phase 4: Event-Driven Architecture (Observer Pattern)**

**Objective:** Distribute GPS data to multiple consumers (UI, Database) simultaneously.

* **Problem:** The main loop was tightly coupled to std::cout, making it impossible to add a database logger or GUI without cluttering the driver loop.  
* **Implementation:** \* Refactored the Parser into an **Event Emitter**.  
  * Utilized std::function callbacks to allow external systems to subscribe to onFix events.  
* **Outcome:** Zero coupling between the Driver and the Display/Logger systems.

## **Phase 5: Concurrency (Producer-Consumer)**

**Objective:** Prevent packet loss during high-frequency data bursts (10Hz).

* **Problem:** Slow downstream processing (e.g., database writes taking 500ms) blocked the input reader, causing the OS to drop incoming UDP packets.  
* **Implementation:** \* **Producer Thread:** Dedicated high-priority thread for non-blocking I/O.  
  * **Consumer Thread:** Dedicated thread for logic and persistence.  
  * **SafeQueue:** Implemented a thread-safe queue using std::mutex and std::condition\_variable to handle backpressure.  
* **Outcome:** Verified zero packet loss at 10Hz input rates, even with simulated 2Hz consumer latency.

## **Phase 6: Persistence & Visualization**

**Objective:** Persist track history and improve operator UX.

* **Persistence:** Integrated **SQLite** using RAII wrappers and Prepared Statements to securely log voyage data (voyage\_data.db).  
* **Visualization:** Replaced scrolling console logs with a static **NCurses TUI** dashboard, implementing strict thread isolation to prevent rendering artifacts.

## **Phase 7: Infrastructure & DevOps**

**Objective:** Ensure reproducibility and code quality.

* **Build System:** Migrated from Makefiles to **CMake** for cross-platform dependency management (FetchContent).  
* **Testing:** Replaced manual scripts with **GoogleTest**, covering Factory logic, Checksum validation, and Concurrency safety.  
* **Containerization:** Implemented a **Multi-Stage Dockerfile** (Alpine Linux), reducing the deployment artifact from \>500MB to \<15MB.

## **Technical Stack Summary**

* **Language:** C++17  
* **Patterns:** Factory, Observer, Producer-Consumer, RAII, Dependency Injection.  
* **Libs:** SQLite3, NCurses, GoogleTest, POSIX Threads.  
* **Tools:** CMake, Docker, Git.