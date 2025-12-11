#include "GPSDashboard.h"
#include <cmath>

void GPSDashboard::drawStaticLayout() {
    clear(); // Clear screen for fresh layout
    box(stdscr, 0, 0);

    // Title
    attron(A_BOLD);
    mvprintw(1, 2, " FLEET COMMAND CENTER ");
    attroff(A_BOLD);
    mvprintw(2, 2, "======================");

    // Table Header
    mvprintw(4, 2, "%-10s | %-12s | %-12s | %-8s | %-5s", 
             "VESSEL ID", "LATITUDE", "LONGITUDE", "SPEED", "SATS");
    mvprintw(5, 2, "-------------------------------------------------------------");

    refresh();
}

void GPSDashboard::update(const GPSData& data) {
    // 1. Update the state
    fleetState[data.ID] = data;

    // 2. Redraw the table
    redrawFleet();
}

void GPSDashboard::redrawFleet() {
    int row = 6; // Start below header

    for (const auto& [id, data] : fleetState) {
        // Clear the row before printing
        move(row, 2);
        clrtoeol(); 
        
        // Print columns
        // ID
        mvprintw(row, 2, "%-10s", id.c_str());
        
        // Lat
        mvprintw(row, 15, "%9.5f %c", std::abs(data.latitude), (data.latitude >= 0 ? 'N' : 'S'));
        
        // Lon
        mvprintw(row, 30, "%9.5f %c", std::abs(data.longitude), (data.longitude >= 0 ? 'E' : 'W'));
        
        // Speed
        mvprintw(row, 45, "%5.1f kts", data.speed);

        // Sats
        mvprintw(row, 56, "%2d", data.satellites);

        row++;
    }
    
    // Draw Footer
    box(stdscr, 0, 0); // Re-draw border in case we wiped it
    mvprintw(row + 2, 2, "Status: MONITORING active on Ports 10110, 10111");
    mvprintw(row + 3, 2, "Press 'q' or Ctrl+C to Shutdown");

    refresh();
}