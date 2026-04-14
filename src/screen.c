#include "screen.h"
#include <stdio.h>

void screen_clear(void) {
    printf("\033[2J");
}

void screen_move_cursor(int x, int y) {
    // Terminal cursor is 1-indexed, so we add 1
    printf("\033[%d;%dH", y + 1, x + 1);
}

void screen_draw_char(int x, int y, char c) {
    screen_move_cursor(x, y);
    putchar(c);
}

void screen_flush(void) {
    fflush(stdout);
}
