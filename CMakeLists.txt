# Update the minimum version to a range to support policies up to 3.30
cmake_minimum_required(VERSION 3.14...3.30)

# FIX: Force dependencies to use at least CMake 3.5 policies
# This silences the "Compatibility with CMake < 3.5" error
set(CMAKE_POLICY_VERSION_MINIMUM 3.5)

project(NMEA_Engine VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Optimization: Turn off the JSON library's own tests (saves compile time)
set(JSON_BuildTests OFF CACHE INTERNAL "")
set(CROW_BUILD_EXAMPLES OFF CACHE INTERNAL "")
set(CROW_BUILD_TESTS OFF CACHE INTERNAL "")

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
# Update the linking for the Core Library
target_link_libraries(nmea_core PRIVATE 
    SQLite::SQLite3 
    ${CURSES_LIBRARIES} 
    Threads::Threads
    nlohmann_json::nlohmann_json  # <--- Added
    Crow::Crow                    # <--- Added
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

# ----------------------------------------------------
# 6. JSON Library (nlohmann/json)
# Industry standard for C++ JSON serialization
# ----------------------------------------------------
FetchContent_Declare(
    json
    URL https://github.com/nlohmann/json/releases/download/v3.11.2/json.tar.xz
)
FetchContent_MakeAvailable(json)
# ----------------------------------------------------
# 6.5. Standalone Asio
# ----------------------------------------------------
FetchContent_Declare(
    asio
    URL https://github.com/chriskohlhoff/asio/archive/refs/tags/asio-1-28-0.zip
)
FetchContent_MakeAvailable(asio)

# FIX: Get the path where CMake downloaded Asio
FetchContent_GetProperties(asio)
if(NOT asio_POPULATED)
    FetchContent_Populate(asio)
endif()

# FIX: Manually set the variable that Crow is looking for.
# The headers are located inside the 'asio/include' subfolder of the download.
set(ASIO_INCLUDE_DIR "${asio_SOURCE_DIR}/asio/include" CACHE PATH "" FORCE)

# ----------------------------------------------------
# 7. Crow Web Framework
# ----------------------------------------------------
set(CROW_USE_BOOST OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
    Crow
    URL https://github.com/CrowCpp/Crow/archive/refs/tags/v1.2.0.zip
)
FetchContent_MakeAvailable(Crow)

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
# Test Suite 3: JSON Logic
add_executable(test_json tests/test_json.cpp)
# We need to link nlohmann_json here!
target_link_libraries(test_json PRIVATE nmea_core gtest_main nlohmann_json::nlohmann_json)

# Integrate with CTest
include(GoogleTest)
gtest_discover_tests(test_parsing)
gtest_discover_tests(test_concurrency)
gtest_discover_tests(test_json)
