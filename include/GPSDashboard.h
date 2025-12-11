#pragma once
#include <ncurses.h>
#include <string>
#include <map>
#include "NMEAParser.h"

class GPSDashboard {
private:
    // Store the latest state for every vessel ID
    std::map<std::string, GPSData> fleet;

public:
    GPSDashboard() {
        // 1. Initialize NCurses
        initscr();            // Start curses mode
        cbreak();             // Line buffering disabled
        noecho();             // Don't echo keypresses
        curs_set(0);          // Hide the blinking cursor
        
        // Enable non-blocking input for the dashboard itself if needed
        nodelay(stdscr, TRUE); 
        
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
    void redrawTable();
};