#ifndef SCREEN_H
#define SCREEN_H

/**
 * Screen driver utilizing ANSI escape sequences
 * acting as an output video buffer manager.
 */

void screen_clear(void);
void screen_move_cursor(int x, int y);
void screen_draw_char(int x, int y, char c);
void screen_flush(void);

#endif // SCREEN_H
