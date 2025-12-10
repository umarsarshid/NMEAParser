#include <gtest/gtest.h>
#include "JSONUtils.h"
#include <iostream>

TEST(JSONTest, SerializesCorrectly) {
    // 1. Setup a dummy GPS Data object
    GPSData data;
    data.type = "GPRMC";
    data.isValid = true;
    data.latitude = 48.117;
    data.longitude = 11.516;
    data.speed = 12.5;
    data.course = 220.0;
    data.satellites = 8;
    data.altitude = 500.0;

    // 2. Convert to JSON
    json j = data;

    // 3. Verify Fields
    EXPECT_EQ(j["type"], "GPRMC");
    EXPECT_TRUE(j["isValid"]);
    EXPECT_NEAR(j["lat"], 48.117, 0.0001);
    EXPECT_NEAR(j["speed"], 12.5, 0.0001);
}

TEST(JSONTest, StringDumpWorks) {
    GPSData data;
    data.latitude = 10.0;
    
    std::string output = GPSDataToJson(data);
    
    // Check if the string actually looks like JSON
    EXPECT_TRUE(output.find("\"lat\":10.0") != std::string::npos);
}