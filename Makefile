# Compiler Settings
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2

# Add Linker Flags (Linux needs -pthread and -lsqlite3)
LDFLAGS = -pthread -lsqlite3 -lncurses

# Project Name
TARGET = nmea_engine

# Source Files
SRCS = main.cpp NMEAParser.cpp SQLiteLogger.cpp GPSDashboard.cpp
OBJS = $(SRCS:.cpp=.o)

# Header Dependencies (for rebuilding if headers change)
HEADERS = NMEAParser.h NMEASentences.h NMEASource.h SQLiteLogger.h SafeQueue.h GPSDashboard.h

# --- Targets ---

# Default target (what happens when you just type 'make')
all: $(TARGET)
	@echo "Build Complete. Run ./$(TARGET) to start."

# Linking the executable
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS) $(LDFLAGS)

# Compiling source files into object files
%.o: %.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Run the application
run: $(TARGET)
	./$(TARGET)

# Clean up build files (use 'make clean')
clean:
	rm -f $(OBJS) $(TARGET)

# --- Phony Targets ---
.PHONY: all clean run