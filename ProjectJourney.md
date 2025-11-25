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
    // What if I want to add $GPRMC? I have to modify this huge function again.  
}

## **Chapter 2: The Architectural Pivot**

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

## **🏁 Conclusion**

What started as a regex exercise evolved into a robust **Systems Engineering** project.

* **Lines of Code:** \~400  
* **Architecture:** Factory Pattern & Strategy Pattern  
* **Key Tech:** C++17, Polymorphism, POSIX Sockets, Termios

This project demonstrates not just C++ syntax, but the ability to design scalable software that survives the messy reality of hardware integration.