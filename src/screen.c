/**
 * screen.c — Terminal Screen Driver (Framebuffer + ANSI)
 * =======================================================
 * Maintains a software framebuffer (char grid) to produce
 * flicker-free rendering via double-buffer diffing.
 *
 * Only <stdio.h> is used (permitted by the Hardware Abstraction rule).
 */

#include "screen.h"
#include <stdio.h>

/* ── Framebuffer ──────────────────────────────────────────────────────── */
typedef struct {
    char c;
    ScreenColor color;
} Pixel;

static Pixel fb[SCREEN_FB_HEIGHT][SCREEN_FB_WIDTH];   
static Pixel prev_fb[SCREEN_FB_HEIGHT][SCREEN_FB_WIDTH];
static int   fb_dirty = 1;

static const char* color_codes[] = {
    "\033[0m",  /* Reset */
    "\033[31m", /* Red */
    "\033[32m", /* Green */
    "\033[33m", /* Yellow */
    "\033[34m", /* Blue */
    "\033[36m", /* Cyan */
    "\033[37m"  /* White */
};

/* ── Cursor Visibility ────────────────────────────────────────────────── */
void screen_hide_cursor(void) { printf("\033[?25l"); }
void screen_show_cursor(void) { printf("\033[?25h"); }

/* ── Clear Virtual Framebuffer ────────────────────────────────────────── */
/**
 * screen_clear — fill the back buffer with spaces.
 * Does NOT immediately print; the diff is resolved on screen_flush().
 */
void screen_clear(void) {
    int x, y;
    for (y = 0; y < SCREEN_FB_HEIGHT; y++) {
        for (x = 0; x < SCREEN_FB_WIDTH; x++) {
            fb[y][x].c = ' ';
            fb[y][x].color = SCR_COLOR_RESET;
        }
    }
}

/* ── Move Terminal Cursor ─────────────────────────────────────────────── */
void screen_move_cursor(int x, int y) {
    printf("\033[%d;%dH", y + 1, x + 1);
}

/* ── Draw Character to Framebuffer ───────────────────────────────────── */
void screen_draw_char(int x, int y, char c, ScreenColor color) {
    if (x < 0 || x >= SCREEN_FB_WIDTH)  return;
    if (y < 0 || y >= SCREEN_FB_HEIGHT) return;
    fb[y][x].c = c;
    fb[y][x].color = color;
}

void screen_draw_string(int x, int y, const char *s, ScreenColor color) {
    while (*s && x < SCREEN_FB_WIDTH) {
        screen_draw_char(x++, y, *s++, color);
    }
}

/* ── Flush Framebuffer (Diff-Only Updates) ────────────────────────────── */
/**
 * screen_flush — compare back buffer vs front buffer.
 * Only emit ANSI escape + char for cells that changed.
 * This eliminates full-screen flicker.
 */
void screen_flush(void) {
    int x, y;
    ScreenColor current_color = SCR_COLOR_RESET;
    printf("%s", color_codes[SCR_COLOR_RESET]);

    for (y = 0; y < SCREEN_FB_HEIGHT; y++) {
        for (x = 0; x < SCREEN_FB_WIDTH; x++) {
            if (fb_dirty || fb[y][x].c != prev_fb[y][x].c || fb[y][x].color != prev_fb[y][x].color) {
                screen_move_cursor(x, y);
                
                if (fb[y][x].color != current_color) {
                    printf("%s", color_codes[fb[y][x].color]);
                    current_color = fb[y][x].color;
                }
                
                putchar(fb[y][x].c);
                prev_fb[y][x] = fb[y][x];
            }
        }
    }
    printf("%s", color_codes[SCR_COLOR_RESET]);
    fflush(stdout);
    fb_dirty = 0;
}

/* ── Hard Terminal Clear (initial startup) ────────────────────────────── */
void screen_hard_clear(void) {
    printf("\033[2J\033[H");
    fflush(stdout);
    fb_dirty = 1;
    screen_clear();
}
