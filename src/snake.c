/**
 * snake.c — Snake Game Logic (Phase 2 — Full System Integration)
 * ==============================================================
 * 
 * Library Pipeline:
 *   keyboard.c  → captures input
 *   maths.c     → boundary math, XOR-shift PRNG for food placement
 *   memory.c    → my_alloc() for snake body segments
 *   mystring.c  → my_int_to_str() for score/HUD text
 *   screen.c    → renders all output via framebuffer
 *
 * Rules compliance:
 *   - No <string.h>, <math.h>, or malloc/free
 *   - rand() replaced with xor_rand() (bitwise XOR-shift PRNG)
 *   - sprintf replaced with my_int_to_str()
 *   - Body segments allocated with my_alloc() / my_dealloc()
 */

#include "game.h"
#include "screen.h"
#include "maths.h"
#include "memory.h"
#include "mystring.h"

/* ── Game State ───────────────────────────────────────────────────────── */
SnakeSegment *snake_body = 0;
int           snake_len  = 0;
int           snake_dx   = 1;
int           snake_dy   = 0;

int    score       = 0;
int    high_score  = 0;
Entity foods[FOOD_COUNT];
Entity obstacles[OBSTACLE_COUNT];
int    level       = 1;

static int foods_eaten = 0;   /* track foods to compute level */

/* ── Spawn Food ───────────────────────────────────────────────────────── */
/**
 * spawn_food — place food at a random empty cell.
 * Uses xor_rand() (bitwise XOR-shift) instead of rand().
 * my_mod() (bitwise division-derived) constrains to board.
 */
void spawn_food_at(int index) {
    int i, occupied;
    int attempts = 0;
    int rx, ry;

    do {
        /* Use masking to ensure positive values for my_mod */
        rx = (int)(xor_rand() & 0x7FFFFFFF);
        ry = (int)(xor_rand() & 0x7FFFFFFF);
        
        foods[index].x = BOARD_X + 1 + my_mod(rx, BOARD_W - 2);
        foods[index].y = BOARD_Y + 1 + my_mod(ry, BOARD_H - 2);
        
        /* Final safety clamp to ensure within internal board */
        foods[index].x = my_clamp(foods[index].x, BOARD_X + 1, BOARD_X + BOARD_W - 2);
        foods[index].y = my_clamp(foods[index].y, BOARD_Y + 1, BOARD_Y + BOARD_H - 2);

        occupied = 0;
        /* Check against snake body */
        for (i = 0; i < snake_len; i++) {
            if (snake_body[i].x == foods[index].x && snake_body[i].y == foods[index].y) {
                occupied = 1; break;
            }
        }
        /* Check against other foods */
        for (i = 0; i < FOOD_COUNT; i++) {
            if (i == index) continue;
            if (foods[i].x == foods[index].x && foods[i].y == foods[index].y) {
                occupied = 1; break;
            }
        }
        /* Check against obstacles */
        for (i = 0; i < OBSTACLE_COUNT; i++) {
            if (obstacles[i].x == foods[index].x && obstacles[i].y == foods[index].y) {
                occupied = 1; break;
            }
        }
        attempts++;
    } while (occupied && attempts < 100);
}

void spawn_obstacles(void) {
    int i, j, occupied;
    int rx, ry;

    for (i = 0; i < OBSTACLE_COUNT; i++) {
        do {
            rx = (int)(xor_rand() & 0x7FFFFFFF);
            ry = (int)(xor_rand() & 0x7FFFFFFF);

            obstacles[i].x = BOARD_X + 1 + my_mod(rx, BOARD_W - 2);
            obstacles[i].y = BOARD_Y + 1 + my_mod(ry, BOARD_H - 2);
            
            obstacles[i].x = my_clamp(obstacles[i].x, BOARD_X + 1, BOARD_X + BOARD_W - 2);
            obstacles[i].y = my_clamp(obstacles[i].y, BOARD_Y + 1, BOARD_Y + BOARD_H - 2);

            occupied = 0;
            /* Don't spawn on starting area */
            if (my_abs(obstacles[i].x - (BOARD_X + my_div(BOARD_W, 2))) < 5 &&
                my_abs(obstacles[i].y - (BOARD_Y + my_div(BOARD_H, 2))) < 5) {
                occupied = 1;
            }
            /* Check against other obstacles */
            for (j = 0; j < i; j++) {
                if (obstacles[i].x == obstacles[j].x && obstacles[i].y == obstacles[j].y) {
                    occupied = 1; break;
                }
            }
        } while (occupied);
    }
}

/* ── Reset Snake (Game Restart) ───────────────────────────────────────── */
/**
 * reset_snake — free old body, allocate fresh body, reset all state.
 * Uses my_alloc() for the body array — this is the "boot allocation".
 */
void reset_snake(void) {
    int i;
    if (snake_body) { my_dealloc(snake_body); snake_body = 0; }
    snake_body = (SnakeSegment *)my_alloc(MAX_SNAKE_LEN * (int)sizeof(SnakeSegment));

    snake_len     = 3;
    snake_dx      = 1;
    snake_dy      = 0;
    score         = 0;
    level         = 1;
    foods_eaten   = 0;

    snake_body[0].x = BOARD_X + my_div(BOARD_W, 2);
    snake_body[0].y = BOARD_Y + my_div(BOARD_H, 2);
    snake_body[1].x = snake_body[0].x - 1;
    snake_body[1].y = snake_body[0].y;
    snake_body[2].x = snake_body[0].x - 2;
    snake_body[2].y = snake_body[0].y;

    spawn_obstacles();
    for (i = 0; i < FOOD_COUNT; i++) {
        foods[i].x = 0; foods[i].y = 0; // Initialize
    }
    for (i = 0; i < FOOD_COUNT; i++) spawn_food_at(i);
}

/* ── Change Direction ─────────────────────────────────────────────────── */
/**
 * change_direction — update velocity from key press.
 * Prevents 180° reversal (snake can't instantly reverse).
 */
void change_direction(int key) {
    if ((key == 'w' || key == 'W' || key == KEY_UP)    && snake_dy != 1)  { snake_dx = 0;  snake_dy = -1; }
    else if ((key == 's' || key == 'S' || key == KEY_DOWN)  && snake_dy != -1) { snake_dx = 0;  snake_dy =  1; }
    else if ((key == 'a' || key == 'A' || key == KEY_LEFT)  && snake_dx != 1)  { snake_dx = -1; snake_dy =  0; }
    else if ((key == 'd' || key == 'D' || key == KEY_RIGHT) && snake_dx != -1) { snake_dx = 1;  snake_dy =  0; }
}

/* ── Update Snake Position & State ───────────────────────────────────── */
/**
 * update_snake — advance the snake one step.
 *
 * Memory model:
 *   - Shift body array: copy [i] ← [i-1] (tail moves up the chain).
 *   - Head is updated at index 0.
 *   - If food eaten: snake_len++ (grow; no extra alloc needed since
 *     MAX_SNAKE_LEN was allocated upfront — the new tail slot is valid).
 *
 * Scoring: +10 per food; level increments every 5 foods (bitwise mod).
 */
void update_snake(void) {
    int i;
    int new_head_x = snake_body[0].x + snake_dx;
    int new_head_y = snake_body[0].y + snake_dy;

    int ate_food = 0;
    for (i = 0; i < FOOD_COUNT; i++) {
        if (new_head_x == foods[i].x && new_head_y == foods[i].y) {
            ate_food = 1;
            spawn_food_at(i);
            break;
        }
    }

    int shift_len = ate_food ? snake_len : snake_len - 1;
    for (i = shift_len; i > 0; i--) {
        snake_body[i] = snake_body[i - 1];
    }
    snake_body[0].x = new_head_x;
    snake_body[0].y = new_head_y;

    if (ate_food) {
        score       += my_mul(10, level);
        if (score > high_score) high_score = score;
        foods_eaten++;
        if (snake_len < MAX_SNAKE_LEN) snake_len++;
        if (my_mod(foods_eaten, 5) == 0) level++;
    }
}

/* ── Collision Detection ──────────────────────────────────────────────── */
/**
 * check_collision — returns 1 if game should end.
 *
 * Two checks:
 *   1. Wall collision: head outside board bounds (my_clamp-based).
 *   2. Self collision: head overlaps any body segment.
 *
 * my_abs() and boundary comparisons use bitwise math.c functions.
 */
int check_collision(void) {
    int hx = snake_body[0].x;
    int hy = snake_body[0].y;
    int i;

    if (hx <= BOARD_X || hx >= BOARD_X + BOARD_W - 1 ||
        hy <= BOARD_Y || hy >= BOARD_Y + BOARD_H - 1) {
        return 1;
    }

    for (i = 0; i < OBSTACLE_COUNT; i++) {
        if (hx == obstacles[i].x && hy == obstacles[i].y) return 1;
    }

    for (i = 1; i < snake_len; i++) {
        if (hx == snake_body[i].x && hy == snake_body[i].y) return 1;
    }

    return 0;
}

/* ── Draw Board ───────────────────────────────────────────────────────── */
/**
 * draw_board — render the entire game state to the framebuffer.
 *
 * Render order:
 *   1. Border walls (using UTF-8 box-drawing chars as ASCII fallback '#')
 *   2. Food symbol '*'
 *   3. Snake body segments 'o'
 *   4. Snake head '@'
 */
void draw_board(void) {
    int x, y, i;

    for (x = BOARD_X; x < BOARD_X + BOARD_W; x++) {
        screen_draw_char(x, BOARD_Y, '#', SCR_COLOR_CYAN);
        screen_draw_char(x, BOARD_Y + BOARD_H - 1, '#', SCR_COLOR_CYAN);
    }
    for (y = BOARD_Y; y < BOARD_Y + BOARD_H; y++) {
        screen_draw_char(BOARD_X, y, '#', SCR_COLOR_CYAN);
        screen_draw_char(BOARD_X + BOARD_W - 1, y, '#', SCR_COLOR_CYAN);
    }

    for (i = 0; i < OBSTACLE_COUNT; i++) {
        screen_draw_char(obstacles[i].x, obstacles[i].y, 'X', SCR_COLOR_RED);
    }

    for (i = 0; i < FOOD_COUNT; i++) {
        screen_draw_char(foods[i].x, foods[i].y, '*', SCR_COLOR_YELLOW);
    }

    for (i = 1; i < snake_len; i++) {
        screen_draw_char(snake_body[i].x, snake_body[i].y, 'o', SCR_COLOR_GREEN);
    }

    screen_draw_char(snake_body[0].x, snake_body[0].y, '@', SCR_COLOR_GREEN);
}
