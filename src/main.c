#include <stdio.h>
#include <unistd.h>
#include "keyboard.h"
#include "screen.h"
#include "game.h"

void draw_text(int x, int y, const char *text) {
    screen_move_cursor(x, y);
    printf("%s", text);
}

int main(void) {
    GameState state = START; // Simulating Initial Process State

    keyboard_init(); // Boot Keyboard driver
    reset_snake();

    // The main loop operates like a continuous CPU Scheduler for our application
    while (state != QUIT) {
        screen_clear();

        if (state == START) {
            draw_text((SCREEN_WIDTH / 2) - 5, (SCREEN_HEIGHT / 2) - 2, "Snake OS");
            draw_text((SCREEN_WIDTH / 2) - 10, (SCREEN_HEIGHT / 2), "Press SPACE to Start");
            draw_text((SCREEN_WIDTH / 2) - 5, (SCREEN_HEIGHT / 2) + 2, "Q to Quit");
            
            screen_flush();
            int key = read_key();
            if (key == ' ') state = RUNNING; // Process state change
            else if (key == 'q' || key == 'Q') state = QUIT;
            
            usleep(100000); 
        } 
        else if (state == RUNNING) {
            int key = read_key(); // Fetch interrupt input without blocking
            
            if (key == 'q' || key == 'Q') { state = QUIT; }
            else if (key != EOF && key != -1) {
                change_direction(key);
            }

            update_snake(); // Logic update
            
            // Check process exception condition (Collision)
            if (check_collision()) {
                state = GAME_OVER;
            }

            // Sync visual update
            draw_board();
            
            char score_str[32];
            sprintf(score_str, " Score: %d ", score);
            draw_text(2, 0, score_str);
            
            screen_flush();
            
            usleep(100000); // Thread pace (100ms)
        } 
        else if (state == GAME_OVER) {
            draw_text((SCREEN_WIDTH / 2) - 4, (SCREEN_HEIGHT / 2) - 2, "GAME OVER");
            
            char go_score[32];
            sprintf(go_score, "Final Score: %d", score);
            draw_text((SCREEN_WIDTH / 2) - 7, (SCREEN_HEIGHT / 2) + 0, go_score);
            
            draw_text((SCREEN_WIDTH / 2) - 9, (SCREEN_HEIGHT / 2) + 2, "Press R to Restart");
            draw_text((SCREEN_WIDTH / 2) - 5, (SCREEN_HEIGHT / 2) + 4, "Q to Quit");
            
            screen_flush();
            int key = read_key();
            if (key == 'r' || key == 'R') {
                reset_snake();
                state = RUNNING;      // Restart the logic thread equivalent
            } else if (key == 'q' || key == 'Q') {
                state = QUIT;
            }
            
            usleep(100000);
        }
    }

    // Teardown step, extremely important for Kernel simulations
    screen_clear();
    keyboard_restore();
    screen_move_cursor(0, 0);
    printf("Snake OS successfully terminated.\n");
    return 0;
}
