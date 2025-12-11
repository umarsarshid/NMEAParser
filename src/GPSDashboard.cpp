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
    fleet[data.ID] = data;

    // 2. Redraw the table
    redrawTable();
}

void GPSDashboard::redrawTable() {
    int startRow = 6;
    int maxRows = 20; // Clear up to row 20 to prevent ghosting

    // 1. CLEAR PHASE: Scrub the table area
    for (int i = startRow; i < maxRows; i++) {
        move(i, 2);
        clrtoeol(); // Clear line from cursor to end
        
        // Re-draw right border if clrtoeol wiped it
        // (Simple hack: just print a pipe at the far right or redraw box later)
    }

    // 2. DRAW PHASE: Print active ships
    int row = startRow;
    for (const auto& [id, ship] : fleet) {
        mvprintw(row, 2, "%-10s", id.c_str());
        mvprintw(row, 15, "%9.5f %c", std::abs(ship.latitude), (ship.latitude >= 0 ? 'N' : 'S'));
        mvprintw(row, 30, "%9.5f %c", std::abs(ship.longitude), (ship.longitude >= 0 ? 'E' : 'W'));
        mvprintw(row, 45, "%5.1f kts", ship.speed);
        mvprintw(row, 56, "%2d", ship.satellites);
        row++;
    }
    
    // 3. FOOTER PHASE
    // Draw footer at a fixed location or strictly relative to the last row
    mvprintw(row + 1, 2, "-------------------------------------------------------------");
    mvprintw(row + 2, 2, "Active Sources: %lu   ", fleet.size()); // Extra spaces to wipe old numbers
    mvprintw(row + 3, 2, "Status: MONITORING active on Ports 10110, 10111");
    mvprintw(row + 4, 2, "Press 'q' or Ctrl+C to Shutdown");

    // Redraw the box border because clrtoeol likely ate the right side
    box(stdscr, 0, 0); 

    refresh();
}