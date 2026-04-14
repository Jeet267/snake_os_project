#include "game.h"
#include "screen.h"
#include "maths.h"
#include "keyboard.h"
#include <stdlib.h>

int snake_x = 0;
int snake_y = 0;
int snake_dx = 1;
int snake_dy = 0;

int score = 0;
int food_x = 0;
int food_y = 0;

void spawn_food(void) {
    food_x = 1 + rand() % (SCREEN_WIDTH - 2);
    food_y = 1 + rand() % (SCREEN_HEIGHT - 2);
}

void reset_snake(void) {
    snake_x = SCREEN_WIDTH / 2;
    snake_y = SCREEN_HEIGHT / 2;
    snake_dx = 1;
    snake_dy = 0;
    score = 0;
    spawn_food();
}

void change_direction(int key) {
    if ((key == 'w' || key == 'W' || key == KEY_UP) && snake_dy != 1) { snake_dx = 0; snake_dy = -1; }
    else if ((key == 's' || key == 'S' || key == KEY_DOWN) && snake_dy != -1) { snake_dx = 0; snake_dy = 1; }
    else if ((key == 'a' || key == 'A' || key == KEY_LEFT) && snake_dx != 1) { snake_dx = -1; snake_dy = 0; }
    else if ((key == 'd' || key == 'D' || key == KEY_RIGHT) && snake_dx != -1) { snake_dx = 1; snake_dy = 0; }
}

void update_snake(void) {
    snake_x += snake_dx;
    snake_y += snake_dy;
    
    if (snake_x == food_x && snake_y == food_y) {
        score += 10;
        spawn_food();
    }
}

int check_collision(void) {
    // Collision checking acting as boundary bounds for OS memory access limits
    if (snake_x <= 0 || snake_x >= SCREEN_WIDTH - 1 || 
        snake_y <= 0 || snake_y >= SCREEN_HEIGHT - 1) {
        return 1; // Collision detected
    }
    return 0; // Safe
}

void draw_board(void) {
    for (int y = 0; y < SCREEN_HEIGHT; y++) {
        for (int x = 0; x < SCREEN_WIDTH; x++) {
            if (y == 0 || y == SCREEN_HEIGHT - 1 || x == 0 || x == SCREEN_WIDTH - 1) {
                screen_draw_char(x, y, '#');
            }
        }
    }
    
    // Draw simple food for Phase 1
    screen_draw_char(food_x, food_y, '*');
    
    // Phase 1 features only the Snake Head
    screen_draw_char(snake_x, snake_y, '@');
}
