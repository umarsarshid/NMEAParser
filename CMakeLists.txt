cmake_minimum_required(VERSION 3.14)
project(NMEA_Engine)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 1. Dependencies
find_package(Curses REQUIRED)
find_package(SQLite3 REQUIRED)
find_package(Threads REQUIRED)

# 2. Define the Library (Core Logic)
# We separate logic from main.cpp so we can test the logic independently
add_library(nmea_core
    NMEAParser.cpp
    GPSDashboard.cpp
    SQLiteLogger.cpp
)

target_link_libraries(nmea_core 
    PRIVATE 
    SQLite::SQLite3 
    ${CURSES_LIBRARIES} 
    Threads::Threads
)

# 3. Define the Executable (The App)
add_executable(nmea_app main.cpp)
target_link_libraries(nmea_app PRIVATE nmea_core)

# 4. GoogleTest Setup (Automatic Download)
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
FetchContent_MakeAvailable(googletest)

# 5. Define Tests
enable_testing()

add_executable(unit_tests tests/test_parser.cpp)
target_link_libraries(unit_tests PRIVATE nmea_core gtest_main)

include(GoogleTest)
gtest_discover_tests(unit_tests)