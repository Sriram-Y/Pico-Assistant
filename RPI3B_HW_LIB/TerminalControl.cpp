#include "TerminalControl.hpp"

#include <unistd.h>
#include <cstring>
#include <sys/select.h>
#include <iostream>

TerminalControl::TerminalControl() : rawModeEnabled(false)
{
    // Save the initial state of the terminal immediately on creation
    tcgetattr(STDIN_FILENO, &origTermios);
}

TerminalControl::~TerminalControl()
{
    resetMode();
}

void TerminalControl::setRawMode()
{
    if (rawModeEnabled)
    {
        return;
    }

    tcgetattr(STDIN_FILENO, &origTermios);

    struct termios newTermios;
    std::memcpy(&newTermios, &origTermios, sizeof(newTermios));

    cfmakeraw(&newTermios);
    tcsetattr(STDIN_FILENO, TCSANOW, &newTermios);
    rawModeEnabled = true;
}

void TerminalControl::resetMode()
{
    if (!rawModeEnabled)
        return;

    tcsetattr(STDIN_FILENO, TCSANOW, &origTermios);
    rawModeEnabled = false;
}

int TerminalControl::kbhit()
{
    struct timeval tv = {0L, 0L};
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(STDIN_FILENO, &fds);
    return select(1, &fds, NULL, NULL, &tv);
}

int TerminalControl::getch()
{
    int r;
    unsigned char c;
    if ((r = read(STDIN_FILENO, &c, sizeof(c))) < 0)
    {
        return r;
    }
    
    return c;
}