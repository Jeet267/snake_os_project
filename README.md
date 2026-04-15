# Snake OS — Terminal Game in C (OS Project)

A real-time Snake game that runs entirely in the terminal, written in C to practically demonstrate Operating System principles like Process Scheduling, Memory Tracking, and Device Drivers through interactive limits. 

## Features

| Feature | Details |
| :--- | :--- |
| **Monolithic Kernel** | Central `while` loop running continuously simulating CPU Process switching |
| **Smooth Gameplay** | Non-blocking keyboard input via direct driver polling |
| **Scoring Logic** | Dynamic random coordinates mapped to item generation for pointing (+10 points/item) |
| **Start Screen** | Title layout and control instructions acting as the Initial Boot State |
| **Process Exits** | Graceful Game-over states leading to Restart (`R`) or System Terminate (`Q`) |
| **Driver Isolation** | Hardware signals encoded into clean interfaces (ANSI video, termios input) |

## Controls

| Key | Action |
| :--- | :--- |
| **W / ↑** | Move Up |
| **S / ↓**| Move Down |
| **A / ←** | Move Left |
| **D / →**| Move Right |
| **R** | Restart Process (from Game Over) |
| **Q** | Quit/Terminate Process |
| **SPACE** | Start game on boot menu |

## Build & Run

```bash
# Compile the project
make

# Boot the application
./snake_os

# Remove compiled binaries
make clean
```
*Requirements: GCC (or any POSIX C-compatible compiler) and a POSIX terminal (macOS / Linux).*

## Project Structure

```text
Snake-Os/
├── src/
│   ├── math.c          # Simple math engine boundaries
│   ├── screen.c        # ANSI terminal hardware driver representation
│   ├── keyboard.c      # Non-blocking IO interrupt driver
│   ├── snake.c         # Process logic handling states & memory bounds
│   └── main.c          # Main Kernel file, OS Scheduler loop
├── include/
│   ├── game.h          # Process API definitions
│   ├── keyboard.h      # Keyboard signal macros
│   ├── maths.h         # Arithmetic prototypes
│   └── screen.h        # Renderer commands
├── Makefile            # Build routines
└── README.md
```

## Architecture

#### Library Pipeline
Each module handles specific System functions directly similar to an OS kernel stack:

```text
keyboard.c  → captures WASD / arrow key interrupts (via raw termios mode)
    ↓
snake.c     → updates the virtual memory map (snake pointer coordinates) & applies rules
    ↓
math.c      → basic checks protecting operations (like clamping)
    ↓
screen.c    → forces memory out to the actual frame buffer using ANSI controls
```

#### Process State Machine
```text
  ┌─────────────┐                      
  │  START      │ ────(SPACE)───► ┌───────────────┐
  └─────────────┘                 │   RUNNING     │◄─────┐
                                  │ (Event Loop)  │      │
                                  └───────┬───────┘      │
                                          │ (Collision)  │(Press R)
                                  ┌───────▼───────┐      │
                                  │   GAME OVER   │──────┘
                                  └───────┬───────┘      
                                          │ (Press Q)
                                  ┌───────▼───────┐ 
                                  │    QUIT       │
                                  └───────────────┘
```

#### Memory Model (Phase 1)
Currently, in Phase 1, backend logic utilizes global process state variables to represent the fundamental Memory unit pointer tracking (`snake_x`, `snake_y`, `food_x`). Phase 2 scopes the inclusion of exact Heap Allocations mechanisms (Arrays/Linked Lists for body tail generation).

## Library Reference

### `screen.c / screen.h`
A pure-ANSI renderer relying on strings and fast buffer flushes.
| Function | Purpose |
| :--- | :--- |
| `screen_clear()` | ESC[2J — Clears entire terminal canvas |
| `screen_move_cursor(x,y)` | ESC[row;colH — Absolute cursor jump to coordinate |
| `screen_draw_char(x,y,c)` | Drops a character representation directly into video area |
| `screen_flush()` | Synchronizes logical changes to the display visibly avoiding tearing |

### `keyboard.c / keyboard.h`
Interacts using `termios.h` standard to intercept IO.
| Function | Purpose |
| :--- | :--- |
| `keyboard_init()` | Saves default Terminal state and activates Raw, non-canonical, NOECHO modes |
| `keyboard_restore()` | Necessary Teardown loop resetting environment variables to normal |
| `read_key()` | Asynchronously runs `getchar()`. Converts complex bytes (UP ARROW \033[A) to numeric Keys |

### `math.c / maths.h`
Abstracted calculation functions helping define logical ceilings.
| Function | Purpose |
| :--- | :--- |
| `my_clamp(v, lo, hi)` | Forces memory values inside maximum allocated limits |
| `my_abs(x)` | Value absolutes useful in delta comparisons |

---

