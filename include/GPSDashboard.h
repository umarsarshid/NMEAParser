#pragma once
#include <ncurses.h>
#include <string>
#include "NMEAParser.h"

class GPSDashboard {
public:
    GPSDashboard() {
        // 1. Initialize NCurses
        initscr();            // Start curses mode
        cbreak();             // Line buffering disabled
        noecho();             // Don't echo keypresses
        curs_set(0);          // Hide the blinking cursor
        
        // 2. Draw static elements (Borders, Labels)
        drawStaticLayout();
    }

    ~GPSDashboard() {
        // Cleanup on exit
        endwin();
    }

    // Update the dynamic numbers
    void update(const GPSData& data);

private:
    void drawStaticLayout();
};