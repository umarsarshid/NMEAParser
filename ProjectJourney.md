# **The Voyage: Engineering the NMEA Navigation Engine**

**Dev Log:** A retrospective on scaling a C++ prototype into a distributed IoT system.

## **Milestone 1: The Core Engine (Phases 1-4)**

**The Goal:** Parse raw GPS data from a serial stream.  
I started this project treating it like a LeetCode problem: string manipulation. I just needed to verify the checksum and extract the latitude/longitude from a $GPGGA string.  
The Reality Check:  
Marine hardware is messy. The NMEA-0183 standard uses a weird DDMM.MMMM coordinate format that doesn't map directly to double.

* **Tech:** std::string\_view (for read-only parsing), XOR Checksumming, Hex-to-Int conversion.  
* **The Pivot:** I realized writing a monolithic parse() function was a trap. As soon as I wanted to support $GPRMC (Speed & Course), my if/else chains got ugly. I refactored early to the **Factory Pattern**, creating a polymorphic INMEASentence interface. Now, adding a new sentence type is just adding a file, not hacking the core.

## **Milestone 2: Connecting to the Metal (Phases 5-7)**

**The Goal:** Make it talk to real hardware, not just hardcoded strings.  
A parser is useless if it can't read from a port. I needed to support both physical GPS pucks (UART) and modern yacht simulators (UDP).

* **The Solution:** Dependency Injection. I built a **Hardware Abstraction Layer (HAL)** defined by an INMEASource interface.  
* **The Hardware:** I dug into low-level Linux syscalls (termios) to configure serial ports to the maritime standard: **4800 Baud, 8 Data bits, No Parity (8N1)**.  
* **The Network:** I implemented POSIX sockets to listen on UDP Port 10110\. Using socat to create virtual serial ports on my Mac was a huge "aha\!" moment for testing without buying hardware.

## **Milestone 3: The Architecture Shift (Phases 8-10)**

**The Goal:** Handle high-speed data bursts without choking.  
This was the most critical engineering phase. My simple loop Read \-\> Parse \-\> Print was blocking. If I tried to log data to a slow database, I missed incoming GPS packets.

* **The Fix:** I moved to an **Event-Driven Architecture**.  
  1. **Observer Pattern:** I turned the parser into an Event Emitter. The display and logger just subscribe via callbacks (parser.onFix(...)).  
  2. **Concurrency:** I implemented a **Producer-Consumer** model.  
     * **Thread A (Producer):** Polls hardware at 10Hz. Pushes raw bytes to a thread-safe queue.  
     * **Thread B (Consumer):** Wakes up via std::condition\_variable only when data exists.  
* **The Result:** I stress-tested this with a 500ms artificial delay in the consumer. The queue acted as a shock absorber, resulting in **Zero Packet Loss** at the network layer.

## **Milestone 4: Persistence & Usability (Phases 11-12)**

**The Goal:** Stop losing data when the app closes and make it readable.  
Printing scrolling text to stdout is terrible UX for a captain. I needed a dashboard and a "Black Box" recorder.

* **Persistence:** I integrated **SQLite** (the embedded standard). I wrote an RAII wrapper class to manage the C-style pointers and handles, ensuring the DB connection closes cleanly even if the app crashes. Using **Prepared Statements** was non-negotiable for performance and safety.  
* **TUI Dashboard:** I used **NCurses** to take over the terminal window. This required careful thread hygiene—I had to silence all my background debug logging to prevent the UI from glitching out.

## **Milestone 5: Professional Standards (Phases 13-14)**

**The Goal:** "It works on my machine" wasn't good enough.  
I wanted this code to be deployable.

* **Build System:** I ditched Makefiles for **CMake**. I used FetchContent to manage dependencies (GoogleTest, Crow, JSON) so users don't have to manually install libraries.  
* **DevOps:** I containerized the whole stack. I wrote a **Multi-Stage Dockerfile** using Alpine Linux.  
  * *Stage 1:* Heavy build image (GCC, CMake).  
  * *Stage 2:* Tiny runtime image (\<15MB) containing just the binary and libs.

## **Milestone 6: The IoT Pivot (Phases 15-20)**

**The Goal:** Bridge the gap between C++ and the Modern Web.  
The TUI is cool, but real IoT devices need web interfaces. I wanted a live map running in a browser, served directly from the C++ binary.

* **The Backend:** I integrated **Crow** (a C++ microframework). I set up a WebSocket server on a separate thread to broadcast JSON telemetry.  
* **The Logic:** I serialized my C++ structs into JSON using nlohmann/json.  
* **The Frontend:** I switched context to TypeScript/React. I built a **Vite** app using **Leaflet.js** for mapping.  
* **The Integration:** This was the "Full Stack" moment. I configured the C++ server to host the compiled React static assets (.html, .js, .css).

Final State:  
A single executable that:

1. Reads 4800 baud Serial data.  
2. Parses it on a background thread.  
3. Logs it to SQLite.  
4. Displays it on a Terminal UI.  
5. **AND** serves a React Dashboard over HTTP/WebSockets.