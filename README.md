# Snake OS Project 🐍💻

A terminal-based Snake game built in C to practically demonstrate Operating System principles like Process Scheduling, Memory Segregation, and Device Drivers through interactive means.

## 🚀 Features (Current: Phase 1)
- **OS Kernel Loop Simulation**: Preemptive-like execution loops simulating continuous CPU execution.
- **Hardware Interrupts**: Uses raw, non-blocking `termios` configurations to capture inputs instantly.
- **Basic Memory Bound Tracking**: Bounds checking that simulates software-based memory violation detection (Segmentation Fault Simulation via Wall Collisions).
- **Video Buffer Flush**: Manually mapping characters using ANSI Escape codes to simulate video adapter screen flushing.
- **Scoring & Item Generation**: Dynamic coordinate spawning for game states.

## 🎮 Controls
* **SPACE** = Start System/Game
* **W / Arrow Key UP** = Move Up
* **A / Arrow Key LEFT** = Move Left
* **S / Arrow Key DOWN** = Move Down
* **D / Arrow Key RIGHT** = Move Right
* **R** = Restart Process (From Game Over)
* **Q** = Terminate Process Gracefully

## 🛠 Compilation & Execution
To compile the system and boot the OS layer, ensure you have `gcc` and `make` installed.

```bash
cd Snake-Os
make clean
make
./snake_os
```

## 📚 Technical Overview

**Q: How does this project behave as an OS simulation?**
A: The project's design follows a monolithic architecture flow:
1. `main.c` acts as a **CPU Scheduler**. It runs an infinite loop representing the system runtime and performs process context switching (START -> RUNNING -> GAME_OVER).
2. `keyboard.c` serves as our **Input Device Driver**. It disables Canonical mode, allowing the input buffer to pass raw real-time hardware interrupts without blocking the main execution loop.
3. `screen.c` functions as a **Video Driver**. Rather than clearing the entire console array, it directly manipulates frame memory via `ANSI Escapes` (\033 codes) to overwrite specific output pixels safely.
4. `snake.c` represents a standard **Application Process** managed and executed by the Kernel.

**Q: What is the difference between Phase 1 and Phase 2 implementations?**
A: 
* **Phase 1 (DONE)**: Core I/O drivers, scheduler runtime, basic snake point coordinates, and scoring logic (Single block assignment) have been successfully finalized.
* **Phase 2 (REMAINING)**: Heap Memory Allocation (Expanding Snake tail via Array/Linked Lists backends), and Secondary Storage File Systems (`<stdio.h> fopen`, `.dat` file save/load implementation for persisting High Scores).


