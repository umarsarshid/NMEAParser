#include "GPSDashboard.h"

void GPSDashboard::drawStaticLayout() {
    // Draw a box around the screen
    box(stdscr, 0, 0);

    // Title
    mvprintw(1, 2, " NMEA NAVIGATION ENGINE v1.0 ");
    mvprintw(2, 2, "=============================");

    // Labels
    mvprintw(4, 2, "LATITUDE  :");
    mvprintw(5, 2, "LONGITUDE :");
    mvprintw(6, 2, "ALTITUDE  :");
    
    mvprintw(8, 2, "SPEED     :");
    mvprintw(9, 2, "COURSE    :");
    
    mvprintw(11, 2, "SATELLITES:");
    mvprintw(12, 2, "FIX STATUS:");

    mvprintw(14, 2, "LAST UPD  :");

    refresh();
}

void GPSDashboard::update(const GPSData& data) {
    // Update Values (Columns offset by 15 spaces)
    
    // Latitude
    mvprintw(4, 15, "%10.6f %c", std::abs(data.latitude), (data.latitude >= 0 ? 'N' : 'S'));
    
    // Longitude
    mvprintw(5, 15, "%10.6f %c", std::abs(data.longitude), (data.longitude >= 0 ? 'E' : 'W'));
    
    // Altitude
    mvprintw(6, 15, "%6.1f m", data.altitude);

    // Speed (Only if GPRMC)
    mvprintw(8, 15, "%5.1f kts", data.speed);
    
    // Course
    mvprintw(9, 15, "%5.1f deg", data.course);

    // Sats
    mvprintw(11, 15, "%2d", data.satellites);
    
    // Fix Status
    if (data.isValid) {
        attron(A_BOLD); // Turn on Bold
        mvprintw(12, 15, "  ACTIVE  ");
        attroff(A_BOLD);
    } else {
        mvprintw(12, 15, " SEARCHING");
    }

    // Timestamp
    // (Assuming data.timestamp is raw NMEA time for now)
    mvprintw(14, 15, "%f", data.timestamp);

    // Push changes to screen
    refresh();
}