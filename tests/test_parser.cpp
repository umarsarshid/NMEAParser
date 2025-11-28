#include <gtest/gtest.h>
#include "../NMEAParser.h"

// Test 1: Checksum Validation
TEST(ParserTest, ValidatesCorrectChecksum) {
    // Checksum for this string is 47
    std::string valid = "$GPGGA,123519,4807.038,N,01131.000,E,1,08,0.9,545.4,M,46.9,M,,*47";
    EXPECT_TRUE(NMEAParser::validateChecksum(valid));
}

TEST(ParserTest, RejectsBadChecksum) {
    // Checksum 00 is wrong
    std::string invalid = "$GPGGA,123519,4807.038,N,01131.000,E,1,08,0.9,545.4,M,46.9,M,,*00";
    EXPECT_FALSE(NMEAParser::validateChecksum(invalid));
}

// Test 2: Coordinate Math
TEST(MathTest, ConvertsDMSToDecimal) {
    // 48 degrees, 07.038 minutes -> 48 + (7.038/60) = 48.1173
    double result = NMEAParser::convertToDecimalDegrees("4807.038", "N");
    EXPECT_NEAR(result, 48.1173, 0.0001);
}

TEST(MathTest, HandlesNegativeCoordinates) {
    // 11 degrees, 31.000 minutes WEST -> -(11 + 31/60) = -11.51666
    double result = NMEAParser::convertToDecimalDegrees("01131.000", "W");
    EXPECT_NEAR(result, -11.51666, 0.0001);
}

// Test 3: Full Parse Logic
TEST(ParserTest, ParsesGPGGA) {
    NMEAParser parser;
    std::string raw = "$GPGGA,123519,4807.038,N,01131.000,E,1,08,0.9,545.4,M,46.9,M,,*47";
    GPSData data = parser.parse(raw);
    
    EXPECT_TRUE(data.isValid);
    EXPECT_EQ(data.satellites, 8);
    EXPECT_NEAR(data.altitude, 545.4, 0.1);
}