#ifndef GAME_H
#define GAME_H

/**
 * Game state representations simulating Process scheduling states.
 */

typedef enum {
    START,
    RUNNING,
    GAME_OVER,
    QUIT
} GameState;

#define SCREEN_WIDTH 40
#define SCREEN_HEIGHT 20

// Snake properties
extern int snake_x;
extern int snake_y;
extern int snake_dx;
extern int snake_dy;

// Scoring & Food Phase 1
extern int score;
extern int food_x;
extern int food_y;

void spawn_food(void);
void reset_snake(void);
void change_direction(int key);
void update_snake(void);
int check_collision(void);
void draw_board(void);

#endif // GAME_H
