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

## 📚 Technical Overview (Hinglish Viva Prep)

**Q: Ye Project exactly kaise behave karta hai as an OS simulation?**
A: Is project ka design monolithic architecture flow follow karta hai:
1. `main.c` ek **CPU Scheduler** ki tarah infinite loop me process context switch kar raha hai (START -> RUNNING -> GAME_OVER).
2. `keyboard.c` hamara **Input Device Driver** hai. Ye Canonical mode disable kar deta hai jisse input buffer block hoke wait kiye bina sedha real-time hardware interrupt pass karta hai.
3. `screen.c` ka kaam **Video Driver** jaisa hai `ANSI Escapes` \033 codes use karke frame memory directly overwrite karta hai (avoiding full clear stutter).
4. `snake.c` ek simple **Application Process** hai jisko Kernel run karta hai. 

**Q: Phase 1 vs Phase 2 me kya farq hoga?**
A: 
* **Phase 1 (DONE)**: Core I/O drivers, scheduler runtime, snake head single variable movement, aur point/score system (Single block assignment) successfully implemented hain.
* **Phase 2 (REMAINING)**: Heap Memory Allocation (Snake tail Linked Lists/Arrays) backend implementation, aur Secondary Storage File Systems (`<stdio.h> fopen`, `.dat` file save/load for High Scores).

---
*Developed as a foundational Systems Programming project.*
