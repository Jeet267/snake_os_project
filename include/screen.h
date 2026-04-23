/**
 * screen.h — Terminal Screen Driver Header
 * =========================================
 * Software framebuffer for flicker-free ANSI terminal rendering.
 */

#ifndef SCREEN_H
#define SCREEN_H

/* Framebuffer dimensions — game board is inset within these bounds */
#define SCREEN_FB_WIDTH   80
#define SCREEN_FB_HEIGHT  24

/* Color definitions */
typedef enum {
    SCR_COLOR_RESET = 0,
    SCR_COLOR_RED,
    SCR_COLOR_GREEN,
    SCR_COLOR_YELLOW,
    SCR_COLOR_BLUE,
    SCR_COLOR_CYAN,
    SCR_COLOR_WHITE
} ScreenColor;

void screen_hide_cursor(void);
void screen_show_cursor(void);
void screen_hard_clear(void);
void screen_clear(void);
void screen_move_cursor(int x, int y);
void screen_draw_char(int x, int y, char c, ScreenColor color);
void screen_draw_string(int x, int y, const char *s, ScreenColor color);
void screen_flush(void);

#endif /* SCREEN_H */
