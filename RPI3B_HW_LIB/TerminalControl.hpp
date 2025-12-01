#pragma once

#include <termios.h>

class TerminalControl {
public:
    TerminalControl();
    ~TerminalControl();

    // Enable raw mode (non-blocking, no echo)
    void setRawMode();

    // Reset to original terminal settings
    void resetMode();

    // Check if a key is pressed (non-blocking)
    // Returns >0 if key is pressed, 0 otherwise
    int kbhit();

    // Get a character (non-blocking if in raw mode)
    int getch();

private:
    struct termios m_origTermios;
    bool m_rawModeEnabled;
};
