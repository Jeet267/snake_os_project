% Snake OS: A Terminal-Based OS-Inspired Snake Game with Virtual Memory and Device-Driver Abstractions
% Abhijeet Kumar Shah; Aditya Raj Sharma; Ayush Dev
% April 2026

**Abstract—** This report presents *Snake OS*, a real-time terminal game implemented in C as an operating-systems themed project. The system is organized as a monolithic “kernel” loop that schedules game tasks, a non-blocking keyboard input driver built on POSIX `termios`, and an ANSI terminal “video driver” that renders via a software framebuffer with diff-based flushing to reduce flicker. Phase 2 extends the design with (i) a 64 KB static Virtual RAM (VRAM) region and a custom allocator (bump pointer + free list) to model heap behavior without using `malloc`/`free`, and (ii) a bitwise math library implementing multiplication, division, and modulo using shift-and-add and long-division techniques. A lightweight XOR-shift pseudo-random number generator is used for entity placement. The resulting system demonstrates OS concepts (process state, scheduling loop, memory subsystem, device I/O abstraction) through a playable interactive artifact.

**Index Terms—** operating systems education, terminal graphics, device driver abstraction, custom memory allocator, bitwise arithmetic, XOR-shift PRNG, C programming.

## I. Introduction
Operating Systems courses often teach core abstractions—process control, scheduling, memory management, and device I/O—through theory and small lab exercises. *Snake OS* aims to unify these concepts in a single interactive program. The classic Snake game provides a natural event loop, a state machine, and a workload that benefits from timing control, deterministic updates, and resource tracking.

This project implements a terminal-based Snake game in C while enforcing “OS-like” constraints such as avoiding standard allocation APIs and replacing common library calls with custom equivalents. The architecture maps familiar OS components (kernel loop, drivers, memory subsystem) to modular C libraries.

## II. System Overview
### A. Repository Structure
The repository contains a C implementation of the game and a companion visualization UI.

- **C core**: `src/` and `include/`
  - `main.c`: kernel loop and process state transitions (`START`, `RUNNING`, `GAME_OVER`, `QUIT`)
  - `snake.c`: game rules, entity placement, collision detection, and rendering calls
  - `keyboard.c`: input driver (non-blocking, raw mode)
  - `screen.c`: ANSI framebuffer driver (diff-based flush)
  - `memory.c`: VRAM allocator and diagnostics
  - `math.c`: bitwise math and PRNG
  - `string.c`: custom string helpers and integer formatting
- **UI (optional)**: `ui/` (Next.js 14) providing visual documentation and algorithm visualizers (e.g., bitwise multiplication).

### B. Major Constraints and Goals
The codebase emphasizes OS-style constraints:
- **No dynamic allocation via standard APIs**: the allocator uses a static VRAM region (`memory.c`).
- **No `<string.h>` dependency**: essential string and formatting are implemented in `string.c`.
- **No `<math.h>` dependency**: arithmetic and PRNG are implemented in `math.c`.
- **Terminal-as-hardware**: output is treated as a “device driver” backed by an ANSI framebuffer (`screen.c`).

## III. Design and Implementation
### A. Kernel Loop and Process State Machine
The program uses a single, central loop in `main.c` that repeatedly:
1) polls input, 2) updates the game model, 3) renders to a framebuffer, 4) flushes diffs, and 5) sleeps based on the current difficulty level.

The game is modeled as a process state machine:
- **START**: boot menu and instructions
- **RUNNING**: scheduled update/render loop
- **GAME_OVER**: exception-like state; allows restart or shutdown
- **QUIT**: clean teardown of drivers and terminal state

### B. Keyboard Driver (Non-Blocking Raw Input)
The input subsystem in `keyboard.c` configures the terminal:
- disables canonical mode and echo via `termios`
- enables non-blocking reads using `fcntl(O_NONBLOCK)`

Arrow keys are decoded from the `ESC` sequence (`27`, `'['`, `'A'`/`'B'`/`'C'`/`'D'`) and mapped to extended key codes. This driver permits a responsive real-time update loop without waiting for newline-terminated input.

### C. Screen Driver (Software Framebuffer + ANSI)
The output subsystem in `screen.c` maintains:
- a **back buffer** (`fb`) storing characters and colors
- a **front buffer** (`prev_fb`) storing what was last displayed

On `screen_flush()`, the driver performs **diff-based updates**: only cells that changed are emitted (cursor move + color code + character). This reduces flicker and approximates double-buffer rendering within a terminal.

### D. VRAM Allocator (Bump Pointer + Free List)
To emulate a constrained memory subsystem, `memory.c` implements a 64 KB static array (`virtual_ram`) as the heap. Each allocation stores a header in-band:
- magic sentinel (`0xDEADBEEF`) for basic corruption detection
- payload size
- free flag
- free-list link pointer

Allocation strategy:
- first-fit search in a free list
- otherwise bump allocate from the top pointer

Deallocation marks blocks free and links them into the free list. Diagnostics expose current usage and cumulative allocation totals for HUD display.

### E. Bitwise Math Library and XOR-Shift PRNG
`math.c` implements common arithmetic without using `*`, `/`, or `%` in the core algorithms:
- multiplication: Russian-peasant shift-and-add
- division: bit-by-bit long division
- modulo: derived from division and multiplication

Random placement uses a 32-bit XOR-shift generator (Marsaglia-style) with explicit seeding. In `snake.c`, random values are masked to remain positive and then constrained to the board via `my_mod()` plus a safety clamp.

### F. Snake Model and Entities
The snake’s body is stored as an array allocated once from VRAM:
- `snake_body = my_alloc(MAX_SNAKE_LEN * sizeof(SnakeSegment))`

Movement uses a shift operation over the body array to advance segments. The project also supports:
- multiple food items (`FOOD_COUNT`)
- fixed-count obstacles (`OBSTACLE_COUNT`)
- level progression based on food consumption; score scales with level

## IV. Results and Observations
### A. Responsiveness and Rendering
Non-blocking input avoids stalling the kernel loop, and diff-based screen flush reduces redraw overhead. The fixed framebuffer size (80×24) makes performance stable and predictable across common terminals.

### B. Memory and Safety Signals
The VRAM allocator provides a visible, teachable model:
- bounded memory region (64 KB)
- diagnostics shown in the HUD/game-over screen
- sentinel validation for simple misuse detection

### C. Portability
The C core targets POSIX terminals (macOS/Linux). The UI is optional and runs independently as a Next.js application.

## V. Limitations
- **Allocator fragmentation**: first-fit free list can fragment VRAM; no compaction is performed.
- **Pointer validation**: header sentinel helps detect some invalid frees but is not comprehensive.
- **Terminal assumptions**: relies on ANSI escape support and a terminal size that fits the framebuffer.
- **Single-threaded scheduling**: the “scheduler” is conceptual; no actual preemption or concurrency is modeled.

## VI. Future Work
- implement coalescing of adjacent free blocks in VRAM
- improve input decoding robustness across terminals
- add automated tests for `math.c`, `memory.c`, and `string.c`
- extend HUD to show frame time and update frequency
- integrate UI with recorded gameplay traces for replay/analysis

## VII. Conclusion
*Snake OS* demonstrates OS-inspired design through a playable terminal game. The project combines a kernel-like loop, driver abstractions for input/output, a constrained heap model via VRAM allocation, and bitwise arithmetic utilities. The outcome is a cohesive educational artifact that links core OS topics to concrete implementation choices and observable runtime behavior.

## References
[1] IEEE Computer Society, “IEEE Editorial Style Manual,” 2024.  
[2] IEEE, “IEEE Reference Guide,” 2024.  
[3] D. Ritchie, “The C Programming Language,” 2nd ed., Prentice Hall, 1988.  
[4] IEEE Std 1003.1™-2017, “POSIX Base Specifications,” 2017.  
[5] G. Marsaglia, “Xorshift RNGs,” *Journal of Statistical Software*, vol. 8, no. 14, 2003.  
[6] Next.js Documentation, “App Router,” 2026.  

