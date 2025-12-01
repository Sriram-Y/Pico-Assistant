#include "TerminalControl.hpp"

#include <unistd.h>
#include <cstring>
#include <sys/select.h>
#include <iostream>

TerminalControl::TerminalControl() : m_rawModeEnabled(false)
{
    // Save the initial state of the terminal immediately on creation
    tcgetattr(STDIN_FILENO, &m_origTermios);
}

TerminalControl::~TerminalControl()
{
    // Ensure terminal is reset when object is destroyed (RAII)
    resetMode();
}

void TerminalControl::setRawMode()
{
    if (m_rawModeEnabled)
        return;

    // Refresh original attributes before changing (in case they changed externally)
    tcgetattr(STDIN_FILENO, &m_origTermios);

    struct termios newTermios;
    std::memcpy(&newTermios, &m_origTermios, sizeof(newTermios));

    // cfmakeraw disables echo and sets non-canonical mode (input available immediately)
    cfmakeraw(&newTermios);
    tcsetattr(STDIN_FILENO, TCSANOW, &newTermios);
    m_rawModeEnabled = true;
}

void TerminalControl::resetMode()
{
    if (!m_rawModeEnabled)
        return;

    tcsetattr(STDIN_FILENO, TCSANOW, &m_origTermios);
    m_rawModeEnabled = false;
}

int TerminalControl::kbhit()
{
    struct timeval tv = {0L, 0L};
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(STDIN_FILENO, &fds);
    // select returns >0 if input is waiting in the buffer
    return select(1, &fds, NULL, NULL, &tv);
}

int TerminalControl::getch()
{
    int r;
    unsigned char c;
    if ((r = read(STDIN_FILENO, &c, sizeof(c))) < 0)
        return r;
    return c;
}