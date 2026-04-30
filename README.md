# Snake OS — Terminal Game in C (OS Project)

A real-time Snake game that runs entirely in the terminal, written in C to practically demonstrate Operating System principles like Process Scheduling, Memory Tracking, and Device Drivers through interactive limits.

---

### **Project Team**
- **Abhijeet Kumar Shah**
- **Aditya Raj Sharma**
- **Ayush Dev**

---

## Features (Phase 2)

| Feature | Details |
| :--- | :--- |
| **Monolithic Kernel** | Central `while` loop simulating CPU Process switching and kernel-level task management. |
| **Virtual RAM (VRAM)** | 64 KB static heap (`memory.c`) with a custom bump + free-list allocator. |
| **Bitwise Math Engine** | Arithmetic (`*`, `/`, `%`) implemented using only bit-shifts and logical gates. |
| **XOR-Shift PRNG** | High-performance bitwise random number generation for game entity placement. |
| **ANSI Color Driver** | Full foreground color support (Cyan, Green, Red, Yellow) for a premium arcade feel. |
| **Smooth Gameplay** | Non-blocking keyboard input via direct driver polling and `termios` raw mode. |
| **System HUD** | Real-time tracking of memory usage, allocation counts, and CPU "Kernel" state. |

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
│   ├── math.c          # Bitwise math engine (mul, div, mod, sqrt)
│   ├── memory.c        # Virtual RAM subsystem & custom allocator
│   ├── screen.c        # ANSI terminal hardware driver & framebuffer
│   ├── keyboard.c      # Non-blocking IO interrupt driver
│   ├── string.c        # Custom string library (no <string.h>)
│   ├── snake.c         # Process logic, body management & rules
│   └── main.c          # Main Kernel file, OS Scheduler loop
├── include/
│   ├── game.h          # Process API & Game state definitions
│   ├── memory.h        # Allocator interface
│   ├── keyboard.h      # Keyboard signal macros
│   ├── maths.h         # Arithmetic prototypes & PRNG
│   ├── mystring.h      # String manipulation prototypes
│   └── screen.h        # Renderer & Color commands
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
math.c      → bitwise arithmetic for coordinate calculation & PRNG food placement
    ↓
memory.c    → manages heap blocks for dynamic snake body segments
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

#### Memory Model (Phase 2)
In Phase 2, the system implements a **Static 64 KB Virtual RAM region**. 
- **Heap Allocation**: Uses `my_alloc()` and `my_dealloc()` to manage memory without `malloc`.
- **Snake Body**: Managed as a dynamic array in VRAM, allowing the snake to grow during runtime.
- **Metadata**: In-band block headers track allocation size and state with `0xDEADBEEF` magic sentinels.

## Library Reference

### `memory.c / memory.h`
| Function | Purpose |
| :--- | :--- |
| `my_alloc(size)` | Allocates `size` bytes from VRAM using first-fit free-list logic. |
| `my_dealloc(ptr)` | Returns a block to the free-list for reuse. |
| `mem_reset()` | Wipes the entire Virtual RAM region (System Reboot). |

### `math.c / maths.h`
| Function | Implementation |
| :--- | :--- |
| `my_mul(a, b)` | Russian Peasant / Shift-and-Add algorithm. |
| `my_div(a, b)` | Non-restoring long division. |
| `xor_rand()` | 32-bit XOR-Shift pseudo-random number generator. |

### `screen.c / screen.h`
| Function | Purpose |
| :--- | :--- |
| `screen_draw_char()` | Drops a character with specific foreground color into the buffer. |
| `screen_flush()` | Synchronizes logical changes to the display visibly avoiding tearing. |

---

