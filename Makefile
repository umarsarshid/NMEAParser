# Compiler Settings
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2

# Project Name
TARGET = nmea_engine

# Source Files
SRCS = main.cpp NMEAParser.cpp
OBJS = $(SRCS:.cpp=.o)

# Header Dependencies (for rebuilding if headers change)
HEADERS = NMEAParser.h NMEASentences.h NMEASource.h

# --- Targets ---

# Default target (what happens when you just type 'make')
all: $(TARGET)
	@echo "Build Complete. Run ./$(TARGET) to start."

# Linking the executable
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

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