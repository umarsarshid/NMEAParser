#include <gtest/gtest.h>
#include "../NMEAParser.h"

// Fixture for setting up complex tests
class ParserTest : public ::testing::Test {
protected:
    NMEAParser parser;
};

// 1. Math Helper Tests
TEST_F(ParserTest, ChecksumValidation) {
    // FIX: Use the full, original string where we KNOW *47 is the correct checksum
    std::string valid = "$GPGGA,123519,4807.038,N,01131.000,E,1,08,0.9,545.4,M,46.9,M,,*47";
    
    EXPECT_TRUE(NMEAParser::validateChecksum(valid));

    // Invalid Checksum (Same string, wrong hex at the end)
    std::string invalid = "$GPGGA,123519,4807.038,N,01131.000,E,1,08,0.9,545.4,M,46.9,M,,*00";
    EXPECT_FALSE(NMEAParser::validateChecksum(invalid));
}

TEST_F(ParserTest, CoordinateConversion) {
    // 48 deg 07.038 min North
    EXPECT_NEAR(NMEAParser::convertToDecimalDegrees("4807.038", "N"), 48.1173, 0.0001);
    // South should be negative
    EXPECT_NEAR(NMEAParser::convertToDecimalDegrees("4807.038", "S"), -48.1173, 0.0001);
}

// 2. Factory Pattern Tests (GPRMC vs GPGGA)
TEST_F(ParserTest, ParsesGPGGA_Sentences) {
    std::string raw = "$GPGGA,123519,4807.038,N,01131.000,E,1,08,0.9,545.4,M,46.9,M,,*47";
    GPSData data = parser.parse(raw);
    
    EXPECT_TRUE(data.isValid);
    EXPECT_EQ(data.type, "GPGGA");
    EXPECT_EQ(data.satellites, 8);
}

TEST_F(ParserTest, ParsesGPRMC_Sentences) {
    // RMC includes Speed and Course
    std::string raw = "$GPRMC,123519,A,4807.038,N,01131.000,E,022.4,084.4,230394,003.1,W*6A";
    GPSData data = parser.parse(raw);
    
    EXPECT_TRUE(data.isValid);
    EXPECT_EQ(data.type, "GPRMC");
    EXPECT_NEAR(data.speed, 22.4, 0.1);
    EXPECT_NEAR(data.course, 84.4, 0.1);
}

// 3. Observer Pattern Tests
TEST_F(ParserTest, ObserverCallbackFires) {
    bool callbackFired = false;
    
    // Register a lambda listener
    parser.onFix([&callbackFired](const GPSData& d) {
        callbackFired = true;
        EXPECT_EQ(d.type, "GPGGA");
    });

    std::string raw = "$GPGGA,123519,4807.038,N,01131.000,E,1,08,0.9,545.4,M,46.9,M,,*47";
    parser.parse(raw);

    EXPECT_TRUE(callbackFired) << "Callback failed to execute on valid fix";
}