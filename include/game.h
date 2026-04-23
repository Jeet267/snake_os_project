/**
 * game.h — Game State & Entity Definitions
 * =========================================
 * Central header for the Snake OS game.
 * Defines process states, board dimensions, and all game entities.
 */

#ifndef GAME_H
#define GAME_H

/* ── Process / Game States ────────────────────────────────────────────── */
typedef enum {
    START,          /* Initial idle state — simulates boot/idle process */
    RUNNING,        /* Active CPU scheduling — game loop executing       */
    GAME_OVER,      /* Exception state — process crashed (collision)     */
    QUIT            /* Kernel shutdown / exit syscall                    */
} GameState;

/* ── Board Dimensions ─────────────────────────────────────────────────── */
/* Game board is inset from the framebuffer; borders drawn at edges */
#define BOARD_X       1           /* board left edge (inside framebuffer) */
#define BOARD_Y       2           /* board top  edge (below HUD)           */
#define BOARD_W       60          /* playable width  (cols)                */
#define BOARD_H       20          /* playable height (rows)                */

/* Aliases used throughout code */
#define SCREEN_WIDTH  BOARD_W
#define SCREEN_HEIGHT BOARD_H

/* ── Snake Constants ──────────────────────────────────────────────────── */
#define MAX_SNAKE_LEN  256
#define FOOD_COUNT     3          /* Multiple food items */
#define OBSTACLE_COUNT 5          /* Static obstacles */

/* ── Snake Segment (allocated via memory.c) ───────────────────────────── */
typedef struct {
    int x;
    int y;
} SnakeSegment;

typedef struct {
    int x;
    int y;
} Entity;

/* ── Game State Globals ───────────────────────────────────────────────── */
extern SnakeSegment *snake_body;
extern int           snake_len;
extern int           snake_dx;
extern int           snake_dy;

extern int           score;
extern int           high_score;
extern Entity        foods[FOOD_COUNT];
extern Entity        obstacles[OBSTACLE_COUNT];
extern int           level;

/* ── Keyboard Key Codes ───────────────────────────────────────────────── */
/* (Defined here so game.h is the single include for logic files) */
#ifndef KEY_UP
#define KEY_UP    256
#define KEY_DOWN  257
#define KEY_LEFT  258
#define KEY_RIGHT 259
#endif

/* ── Function Prototypes ──────────────────────────────────────────────── */
void spawn_food(void);
void reset_snake(void);
void change_direction(int key);
void update_snake(void);
int  check_collision(void);
void draw_board(void);

#endif /* GAME_H */
