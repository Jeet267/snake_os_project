#include "keyboard.h"
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>

static struct termios oldt, newt;
static int oldf;

void keyboard_init(void) {
    // Save current terminal state
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    // Disable canonical mode and echo (makes input raw and hides typed chars)
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    // Apply non-blocking constraint
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);
}

int read_key(void) {
    int ch = getchar();
    if (ch == 27) { // ESC sequence for arrow keys
        int next1 = getchar();
        if (next1 == '[') {
            int next2 = getchar();
            if (next2 == 'A') return KEY_UP;
            if (next2 == 'B') return KEY_DOWN;
            if (next2 == 'C') return KEY_RIGHT;
            if (next2 == 'D') return KEY_LEFT;
        }
    }
    return ch;
}

void keyboard_restore(void) {
    // Reset back to original state on teardown
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);
}
