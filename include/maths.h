/**
 * maths.h — Custom Math Library Header (Bitwise Implementation)
 * =============================================================
 * All functions are implemented using only bitwise / shift operations.
 * No standard <math.h> is used.
 */

#ifndef MATHS_H
#define MATHS_H

/* ── XOR-Shift PRNG ───────────────────────────────────────────────────── */
void         xor_rand_seed(unsigned int seed);  /* seed the PRNG */
unsigned int xor_rand(void);                    /* generate next random uint */

/* ── Core Arithmetic (Bitwise) ────────────────────────────────────────── */
int my_abs(int value);                          /* branchless absolute value  */
int my_mul(int a, int b);                       /* shift-and-add multiply     */
int my_div(int a, int b);                       /* long-division divide        */
int my_mod(int a, int b);                       /* modulo via div+mul          */

/* ── Utility Math ─────────────────────────────────────────────────────── */
int my_clamp(int value, int min_val, int max_val); /* constrain to [min,max] */
int my_pow2(int n);                             /* 2^n via left shift          */
int my_sqrt(int n);                             /* integer sqrt via bit search */
int my_log2(int n);                             /* floor(log2(n)) via shifts   */
int my_sign(int n);                             /* -1 / 0 / +1 sign            */

#endif /* MATHS_H */
