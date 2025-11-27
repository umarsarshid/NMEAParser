# **🗺️ The Voyage: Building the NMEA Parser**

*A development log chronicling the evolution from a simple string parser to a hardware-integrated navigation system.*

## **Chapter 1: Mini Project**

**The Goal:** Parse a single $GPGGA string from a GPS receiver.  
When I started, I treated this purely as a **String Manipulation** problem. The focus was simple: "Get data in, get struct out."

### **The Challenge: "NMEA is Weird"**

I quickly learned that Marine Standards (NMEA-0183) are ancient.

1. **Coordinates aren't Decimal:** They use DDMM.MMMM (Degrees-Minutes). I had to write a custom math converter because std::stod("4807.038") gives you 4807 degrees, which is impossible (Earth only goes to 90/180).  
2. **Checksums are Critical:** You can't trust the data. I implemented an XOR loop validator.

The "Junior" Solution:  
My initial parse() function was a giant monolith. It worked, but it was fragile.  
// The Old Way (Monolithic)  
GPSData parse(string s) {  
    if (\!checksum(s)) return error;  
    if (s.starts\_with("$GPGGA")) {  
       // ... 50 lines of parsing logic ...  
    }  
}

## **🔄 Chapter 2: The Architectural Pivot**

**The Trigger:** I needed to add $GPRMC (Speed & Course) support.  
I realized that adding more if/else statements inside the main parser violated the **Open/Closed Principle**. Every time I added a sentence type, I risked breaking the GPGGA logic.

### **The Solution: The Factory Pattern**

I refactored the code to use **Polymorphism**.

1. **Abstract Base:** Created INMEASentence.  
2. **Concrete Classes:** Created GPGGASentence and GPRMCSentence.  
3. **The Factory:** The main parser became a dispatcher. It looks at the header (e.g., "GPRMC") and spawns the correct class.

The Result:  
Now, adding support for $GPZDA (Time) or $GPGSV (Satellites) requires zero changes to the existing parser logic. I just create a new file.

## **Chapter 3: Connecting to the Real World**

**The Trigger:** A parser is useless if it can't read from hardware.  
Testing with hardcoded strings was fine for logic, but real boats use **Serial Cables (RS-422)** or **UDP WiFi Broadcasts**.

### **The Challenge: Hardware Abstraction**

I didn't want my parser to care where the data came from. I implemented a **Hardware Abstraction Layer (HAL)** using Dependency Injection.

1. **The Interface:** INMEASource (Must implement readLine()).  
2. **Serial Implementation:** I used low-level Linux termios to configure a UART port to **4800 Baud, 8N1** (The NMEA hardware standard).  
3. **Network Implementation:** I used POSIX Sockets to listen on UDP Port 10110\.

The "Senior" Moment:  
I used socat to create virtual serial ports on my Mac (/dev/ttys005 \<-\> /dev/ttys006), allowing me to simulate a physical hardware connection without needing a real GPS puck.

## **Chapter 4: Decoupling the Systems**

**The Trigger:** My main.cpp was getting messy. It was printing to the console, but I realized a real Chart Plotter needs to update a GUI, log to a database, and steer the autopilot simultaneously.  
My system was "Pulling" data (asking for updates). Real-time systems need to "Push" data (notify when updates happen).

### **The Solution: The Observer Pattern**

I transformed the Parser into an **Event Emitter**.

1. **Callbacks:** I utilized std::function to define a GPSCallback type.  
2. **Subscription:** I added an onFix() method, allowing any external system to subscribe to updates.  
3. **The Event Loop:** The main loop became "dumb." It simply pumps data from the hardware into the parser. The parser then "rings the bell," and the separate Logging and Display systems wake up automatically.

##  **Chapter 5: Packet Loss**

**The Trigger:** I simulated a slow database write (500ms latency) while receiving high-speed GPS data (10Hz / 100ms). The result was catastrophic: **Packet Loss**. The single-threaded application was busy writing to the database while new data arrived at the network card, causing the OS to drop UDP packets.

### **The Solution: Producer-Consumer Concurrency**

I implemented a multi-threaded architecture to decouple **Ingestion** from **Processing**.

1. **Thread Safety:** I built a custom SafeQueue\<T\> class using std::mutex and std::condition\_variable to prevent race conditions.  
2. **The Producer (Thread A):** A dedicated thread that does nothing but read from hardware and push to the queue. It never blocks.  
3. **The Consumer (Thread B):** A dedicated thread that wakes up only when data exists, parses it, and handles the slow database writes.

The Result:  
I verified via stress testing that the system could handle a 10Hz input stream even if the consumer was artificially slowed down to 2Hz. The queue acted as a "Shock Absorber," ensuring Zero Packet Loss.

## **Chapter 6: Recording the Data(Day 11\)**

**The Trigger:** The system was performing beautifully in memory, but if I crashed the app or lost power, the voyage data was gone forever. Real navigation systems need a persistent "Track History."

### **The Solution: SQLite Integration**

I needed a database, but running a full MySQL server on an embedded device is overkill. I chose **SQLite** (a C-library embedded DB).

1. **C++ Wrapper (RAII):** SQLite is a raw C library (pointers everywhere). I wrote a SQLiteLogger class that automatically opens the DB in the constructor and closes it in the destructor, preventing memory leaks.  
2. **Prepared Statements:** Instead of concatenating strings (which is slow and dangerous), I used sqlite3\_prepare\_v2 to pre-compile the SQL. This optimized the logging speed significantly.  
3. **Integration:** I simply attached the Logger as another "Observer" to the event bus. The parser doesn't even know the database exists.

The Result:  
The system now creates a permanent voyage\_data.db file on disk, logging every movement of the vessel.

## **Chapter 7: TUI Dashboard (Day 12\)**

**The Trigger:** Scrolling text logs (std::cout) are useless for a helmsman who needs to see their current heading at a glance. I needed a professional dashboard.

### **The Solution: NCurses TUI**

I integrated **ncurses** to take control of the terminal window.

1. **RAII Wrapper:** I created a GPSDashboard class. Its constructor calls initscr() (enter visual mode) and its destructor calls endwin() (exit visual mode), ensuring the terminal isn't broken if the app crashes.  
2. **Thread Hygiene:** This was the hardest part. If the background "Reader Thread" tried to print debug logs while the "UI Thread" was drawing the dashboard, the screen would shatter into garbled text. I had to strictly silence all background threads before launching the UI.

The Result:  
A static, flicker-free dashboard that updates Latitude, Longitude, and Speed in real-time, resembling a physical marine instrument.

## **Conclusion**

What started as a regex exercise evolved into a robust **Systems Engineering** project.

* **Architecture:** Factory Pattern, Observer Pattern, Producer-Consumer Model.  
* **Key Tech:** C++17, Multi-threading, POSIX Sockets, Termios, SQLite3, NCurses.

This project demonstrates not just C++ syntax, but the ability to design scalable software that survives the messy reality of hardware integration, real-time requirements, and resource contention.