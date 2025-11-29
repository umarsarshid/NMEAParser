cmake_minimum_required(VERSION 3.14)
project(NMEA_Engine VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 1. Compiler Warnings (High Quality Code)
if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# 2. Dependencies
find_package(Threads REQUIRED)
find_package(Curses REQUIRED)
find_package(SQLite3 REQUIRED)

# 3. The Core Library (Logic Only)
# We exclude main.cpp so we can link this into Tests independently
add_library(nmea_core
    NMEAParser.cpp
    GPSDashboard.cpp
    SQLiteLogger.cpp
)

# Link external libs to the core
target_link_libraries(nmea_core PRIVATE 
    SQLite::SQLite3 
    ${CURSES_LIBRARIES} 
    Threads::Threads
)

# Allow other targets to see headers in root
target_include_directories(nmea_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# 4. The Application Executable
add_executable(nmea_app main.cpp)
target_link_libraries(nmea_app PRIVATE nmea_core)

# 5. Testing Suite (GoogleTest)
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
# Suppress GTest build noise
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

# Test Suite 1: Parsing Logic
add_executable(test_parsing tests/test_parser.cpp)
target_link_libraries(test_parsing PRIVATE nmea_core gtest_main)

# Test Suite 2: Concurrency Logic
add_executable(test_concurrency tests/test_queue.cpp)
target_link_libraries(test_concurrency PRIVATE nmea_core gtest_main)

# Integrate with CTest
include(GoogleTest)
gtest_discover_tests(test_parsing)
gtest_discover_tests(test_concurrency)