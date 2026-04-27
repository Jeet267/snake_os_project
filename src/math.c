/**
 * math.c — Custom Math Library (Bitwise Implementation)
 * =====================================================
 * All arithmetic is implemented using ONLY bitwise and logical operators.
 * No *, /, % operators are used for core computation.
 * 
 * Techniques:
 *   - Multiplication : Russian-peasant shift-and-add
 *   - Division       : Non-restoring long division (bit-by-bit)
 *   - Modulo         : Derived from division: a - (a/b)*b
 *   - Absolute Value : Sign-bit extension mask (branchless)
 *   - Clamp          : Composed from abs + comparisons
 *   - Power of 2     : Left-shift
 *   - Log base 2     : Count highest set bit via right-shift
 *   - PRNG           : XOR-shift (Marsaglia, 32-bit)
 */

#include "maths.h"

/* ── XOR-Shift PRNG State ─────────────────────────────────────────────── */
static unsigned int xor_state = 2463534242u;   /* Non-zero default seed */

/**
 * xor_rand_seed — seed the PRNG with a non-zero value.
 * If zero is passed, we force a safe default to avoid degenerate state.
 */
void xor_rand_seed(unsigned int seed) {
    xor_state = (seed != 0) ? seed : 2463534242u;
}

/**
 * xor_rand — XOR-shift generator (period 2^32-1).
 * Formula: x ^= x << 13; x ^= x >> 17; x ^= x << 5;
 * Returns a pseudo-random unsigned int.
 */
unsigned int xor_rand(void) {
    xor_state ^= (xor_state << 13);
    xor_state ^= (xor_state >> 17);
    xor_state ^= (xor_state << 5);
    return xor_state;
}

/* ── Absolute Value (Branchless Bitwise) ──────────────────────────────── */
/**
 * my_abs — branchless absolute value using sign-bit mask.
 * For a 32-bit signed int:
 *   mask = value >> 31  →  0x00000000 if positive, 0xFFFFFFFF if negative
 *   (value ^ mask) - mask  →  negates if negative, identity if positive
 */
int my_abs(int value) {
    int mask = value >> 31;          /* arithmetic right-shift fills with sign */
    return (value ^ mask) - mask;
}

/* ── Multiplication (Russian Peasant / Shift-and-Add) ─────────────────── */
/**
 * my_mul — multiply two signed integers using bit shifts only.
 * Algorithm: while b > 0, if LSB of b is set, add a to result;
 *            then double a (left shift) and halve b (right shift).
 * Handles negative operands by tracking sign separately.
 */
int my_mul(int a, int b) {
    int negative = 0;

    if (a < 0) { a = my_abs(a); negative ^= 1; }
    if (b < 0) { b = my_abs(b); negative ^= 1; }

    int result = 0;
    while (b > 0) {
        if (b & 1) {          /* if LSB of b is set */
            result += a;      /* accumulate current 'a' */
        }
        a <<= 1;              /* a = a * 2 */
        b >>= 1;              /* b = b / 2 */
    }
    return negative ? -result : result;
}

/* ── Division (Non-Restoring Long Division) ───────────────────────────── */
/**
 * my_div — integer division using bit-by-bit long division.
 * Processes from the most significant bit to the least.
 * Returns 0 for division by zero (safe fail).
 */
int my_div(int a, int b) {
    if (b == 0) return 0;    /* guard against division by zero */

    int negative = 0;
    if (a < 0) { a = my_abs(a); negative ^= 1; }
    if (b < 0) { b = my_abs(b); negative ^= 1; }

    int quotient  = 0;
    int remainder = 0;
    int i;

    /* Process 31 bits (bit 31 is sign bit, already handled) */
    for (i = 30; i >= 0; i--) {
        remainder <<= 1;
        remainder |= (a >> i) & 1;  /* bring down next bit of dividend */
        if (remainder >= b) {
            remainder -= b;
            quotient |= (1 << i);   /* set corresponding bit in quotient */
        }
    }
    return negative ? -quotient : quotient;
}

/* ── Modulo ───────────────────────────────────────────────────────────── */
/**
 * my_mod — compute a % b using: a - (a/b)*b
 * Uses my_div and my_mul (both bitwise).
 */
int my_mod(int a, int b) {
    if (b == 0) return 0;
    return a - my_mul(my_div(a, b), b);
}

/* ── Clamp ────────────────────────────────────────────────────────────── */
/**
 * my_clamp — constrain value within [min_val, max_val].
 * Uses if-comparisons (bitwise abs is used in my_abs internally).
 */
int my_clamp(int value, int min_val, int max_val) {
    if (value < min_val) return min_val;
    if (value > max_val) return max_val;
    return value;
}

/* ── Power of 2 ───────────────────────────────────────────────────────── */
/**
 * my_pow2 — compute 2^n via left shift.
 * Returns 1 for n < 0 (safe floor), max shift 30 for int safety.
 */
int my_pow2(int n) {
    if (n < 0)  return 1;
    if (n > 30) return (1 << 30);
    return 1 << n;
}

/* ── Integer Square Root (Bitwise Binary Search) ─────────────────────── */
/**
 * my_sqrt — integer square root via binary search on bits.
 * Builds result bit by bit from MSB to LSB.
 */
int my_sqrt(int n) {
    if (n < 0) return 0;
    if (n == 0) return 0;
    int result = 0;
    int bit = 1 << 15;   /* start from highest feasible bit for int range */
    while (bit > n) bit >>= 2;
    while (bit > 0) {
        int candidate = result | bit;
        if (my_mul(candidate, candidate) <= n) {
            result = candidate;
        }
        bit >>= 2;
    }
    return result;
}

/* ── Log Base 2 (Count Highest Set Bit) ──────────────────────────────── */
/**
 * my_log2 — floor(log2(n)) via counting how many times we can right-shift
 * before n becomes 0. Returns -1 for n <= 0.
 */
int my_log2(int n) {
    if (n <= 0) return -1;
    int count = 0;
    while (n > 1) {
        n >>= 1;
        count++;
    }
    return count;
}

/* ── Sign ─────────────────────────────────────────────────────────────── */
/**
 * my_sign — returns -1, 0, or 1 for negative, zero, or positive.
 * Uses bit tricks: negative flag from sign bit, positive from !!n & !neg.
 */
int my_sign(int n) {
    int neg = (n >> 31) & 1;   /* 1 if negative */
    int pos = (!!n) & (~neg);   /* 1 if positive and non-zero */
    return pos - neg;
}
