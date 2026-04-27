#include <stdio.h>
#include <unistd.h>
#include <time.h>

#include "game.h"
#include "screen.h"
#include "maths.h"
#include "memory.h"
#include "mystring.h"
#include "keyboard.h"

/* ── Kernel State ─────────────────────────────────────────────────────── */
static GameState current_state = START;

/* ── HUD and Stats ────────────────────────────────────────────────────── */
static void draw_hud(void) {
    char buf[24];
    screen_draw_string(2, 0, "[ SNAKE OS v2.1 ]", SCR_COLOR_CYAN);
    
    /* Memory usage at top */
    screen_draw_string(30, 0, "VRAM:", SCR_COLOR_WHITE);
    my_int_to_str(mem_bytes_used(), buf);
    screen_draw_string(36, 0, buf, SCR_COLOR_GREEN);
    screen_draw_string(42, 0, "/65536 bytes", SCR_COLOR_WHITE);
}

static void draw_side_panel(void) {
    char buf[24];
    int start_x = 64;

    screen_draw_string(start_x, 3, "SYSTEM STATS", SCR_COLOR_CYAN);
    screen_draw_string(start_x, 4, "------------", SCR_COLOR_CYAN);

    screen_draw_string(start_x, 6, "SCORE", SCR_COLOR_WHITE);
    my_int_to_str(score, buf);
    screen_draw_string(start_x, 7, buf, SCR_COLOR_GREEN);

    screen_draw_string(start_x, 9, "HIGH SCORE", SCR_COLOR_WHITE);
    my_int_to_str(high_score, buf);
    screen_draw_string(start_x, 10, buf, SCR_COLOR_YELLOW);

    screen_draw_string(start_x, 12, "LEVEL", SCR_COLOR_WHITE);
    my_int_to_str(level, buf);
    screen_draw_string(start_x, 13, buf, SCR_COLOR_CYAN);

    screen_draw_string(start_x, 15, "SNAKE LEN", SCR_COLOR_WHITE);
    my_int_to_str(snake_len, buf);
    screen_draw_string(start_x, 16, buf, SCR_COLOR_GREEN);
    
    screen_draw_string(start_x, 19, "CPU: ACTIVE", SCR_COLOR_GREEN);
    screen_draw_string(start_x, 20, "MODE: KERNEL", SCR_COLOR_BLUE);
}

/* ── Centered String Helper ───────────────────────────────────────────── */
static void draw_centered(int y, const char *s, ScreenColor color) {
    int len = my_strlen(s);
    int x   = my_div(SCREEN_FB_WIDTH - len, 2);
    if (x < 0) x = 0;
    screen_draw_string(x, y, s, color);
}

/* ── Start Screen ─────────────────────────────────────────────────────── */
static void draw_start_screen(void) {
    char buf[24];
    draw_centered(8,  "  *** SNAKE OS v2.1 ***  ", SCR_COLOR_GREEN);
    draw_centered(10, "A systems-programming capstone", SCR_COLOR_WHITE);
    draw_centered(12, "WASD / Arrow Keys to move", SCR_COLOR_YELLOW);
    draw_centered(13, "Q = Quit", SCR_COLOR_RED);
    draw_centered(15, "[ SPACE ] to Start", SCR_COLOR_CYAN);
    draw_centered(17, "Powered by: math.c | memory.c | string.c", SCR_COLOR_WHITE);
    draw_centered(18, "           screen.c | keyboard.c", SCR_COLOR_WHITE);

    /* show memory boot info */
    my_int_to_str(mem_bytes_total(), buf);
    screen_draw_string(2, 22, "Virtual RAM: ", SCR_COLOR_WHITE);
    screen_draw_string(15, 22, buf, SCR_COLOR_GREEN);
    screen_draw_string(21, 22, " bytes initialized", SCR_COLOR_WHITE);
}

/* ── Game Over Screen ─────────────────────────────────────────────────── */
static void draw_gameover_screen(void) {
    char buf[24];

    draw_centered(8,  "====== PROCESS EXCEPTION ======", SCR_COLOR_RED);
    draw_centered(9,  "Segmentation fault: Snake collided", SCR_COLOR_WHITE);
    draw_centered(11, "FINAL SCORE:", SCR_COLOR_YELLOW);
    my_int_to_str(score, buf);
    draw_centered(12, buf, SCR_COLOR_GREEN);

    draw_centered(14, "SNAKE LENGTH:", SCR_COLOR_WHITE);
    my_int_to_str(snake_len, buf);
    draw_centered(15, buf, SCR_COLOR_GREEN);

    draw_centered(17, "[ R ] Restart Process", SCR_COLOR_CYAN);
    draw_centered(18, "[ Q ] Kernel Shutdown", SCR_COLOR_RED);

    /* Show allocator diagnostics */
    screen_draw_string(2, 21, "Alloc count: ", SCR_COLOR_WHITE);
    my_int_to_str(mem_alloc_count(), buf);
    screen_draw_string(15, 21, buf, SCR_COLOR_GREEN);
    screen_draw_string(2, 22, "Total allocated: ", SCR_COLOR_WHITE);
    my_int_to_str(mem_total_allocated(), buf);
    screen_draw_string(19, 22, buf, SCR_COLOR_GREEN);
    screen_draw_string(25, 22, " bytes", SCR_COLOR_WHITE);
}

/* ── Compute Game Speed (us) from Level ───────────────────────────────── */
static int get_game_speed_us(void) {
    int base = 150000;
    int step = my_mul(level - 1, 10000);
    int final = base - step;
    if (final < 50000) final = 50000;
    return final;
}

/* ── Kernel Entry Point ───────────────────────────────────────────────── */
int main(void) {
    xor_rand_seed((unsigned int)time(0));
    keyboard_init();
    screen_hide_cursor();
    screen_hard_clear();

    while (current_state != QUIT) {
        screen_clear();

        if (current_state == START) {
            draw_start_screen();
            screen_flush();
            int key = read_key();
            if (key == ' ') {
                reset_snake();
                current_state = RUNNING;
            } else if (key == 'q' || key == 'Q') {
                current_state = QUIT;
            }
            usleep(50000);
        }
        else if (current_state == RUNNING) {
            int key = read_key();
            if (key == KEY_UP || key == 'w' || key == 'W') {
                if (snake_dy != 1) { snake_dx = 0; snake_dy = -1; }
            } else if (key == KEY_DOWN || key == 's' || key == 'S') {
                if (snake_dy != -1) { snake_dx = 0; snake_dy = 1; }
            } else if (key == KEY_LEFT || key == 'a' || key == 'A') {
                if (snake_dx != 1) { snake_dx = -1; snake_dy = 0; }
            } else if (key == KEY_RIGHT || key == 'd' || key == 'D') {
                if (snake_dx != -1) { snake_dx = 1; snake_dy = 0; }
            } else if (key == 'q' || key == 'Q') {
                current_state = QUIT;
            }

            update_snake();
            if (check_collision()) {
                current_state = GAME_OVER;
            }

            draw_hud();
            draw_side_panel();
            draw_board();
            screen_flush();

            usleep(get_game_speed_us());
        }
        else if (current_state == GAME_OVER) {
            draw_gameover_screen();
            screen_flush();
            int key = read_key();
            if (key == 'r' || key == 'R' || key == ' ') {
                reset_snake();
                current_state = RUNNING;
            } else if (key == 'q' || key == 'Q') {
                current_state = QUIT;
            }
            usleep(50000);
        }
    }

    keyboard_restore();
    screen_hard_clear();
    screen_show_cursor();
    
    printf("\n\033[36m[ SNAKE OS KERNEL SHUTDOWN ]\033[0m\n");
    printf("Final Score: \033[32m%d\033[0m\n", score);
    printf("Memory Freed: \033[32m100%%\033[0m\n\n");
    
    return 0;
}
