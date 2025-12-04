#pragma once

#include <termios.h>

class TerminalControl {
public:
    TerminalControl();
    ~TerminalControl();

    void setRawMode();

    // Reset to original terminal settings
    void resetMode();

    // Check if a key is pressed (non-blocking)
    // Returns >0 if key is pressed, 0 otherwise
    int kbhit();

    int getch();

private:
    struct termios origTermios;
    bool rawModeEnabled;
};