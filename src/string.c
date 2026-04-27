/**
 * string.c — Custom String Library
 * =================================
 * Implements essential string operations WITHOUT <string.h>.
 * Integer conversions use my_div / my_mod from math.c (bitwise).
 */

#include "mystring.h"
#include "maths.h"

/* ── String Length ────────────────────────────────────────────────────── */
/**
 * my_strlen — count bytes until null terminator.
 * Walks the pointer forward; no array indexing arithmetic with *.
 */
int my_strlen(const char *s) {
    const char *p = s;
    while (*p != '\0') p++;
    return (int)(p - s);
}

/* ── String Copy ──────────────────────────────────────────────────────── */
/**
 * my_strcpy — copy src into dst including null terminator.
 * Returns dst.
 */
char *my_strcpy(char *dst, const char *src) {
    char *ret = dst;
    while ((*dst++ = *src++) != '\0');
    return ret;
}

/* ── String Compare ───────────────────────────────────────────────────── */
/**
 * my_strcmp — lexicographic compare.
 * Returns 0 if equal, negative if a < b, positive if a > b.
 */
int my_strcmp(const char *a, const char *b) {
    while (*a && (*a == *b)) { a++; b++; }
    return (unsigned char)*a - (unsigned char)*b;
}

/* ── String Concat ────────────────────────────────────────────────────── */
/**
 * my_strcat — append src to end of dst; dst must have enough room.
 * Returns dst.
 */
char *my_strcat(char *dst, const char *src) {
    char *p = dst;
    while (*p) p++;            /* walk to end of dst */
    while ((*p++ = *src++) != '\0');
    return dst;
}

/* ── Integer to String ────────────────────────────────────────────────── */
/**
 * my_int_to_str — convert integer n to decimal string in buf.
 * Uses my_div / my_mod (bitwise) for digit extraction.
 * Returns pointer to buf.
 */
char *my_int_to_str(int n, char *buf) {
    char tmp[24];
    int  i   = 0;
    int  neg  = 0;
    int  idx  = 0;

    if (n == 0) { buf[0] = '0'; buf[1] = '\0'; return buf; }
    if (n < 0)  { neg = 1; n = my_abs(n); }

    while (n > 0) {
        tmp[i++] = '0' + (char)my_mod(n, 10);   /* LSB digit */
        n = my_div(n, 10);
    }
    if (neg) tmp[i++] = '-';

    /* reverse into buf */
    while (i > 0) buf[idx++] = tmp[--i];
    buf[idx] = '\0';
    return buf;
}

/* ── String to Integer ────────────────────────────────────────────────── */
/**
 * my_str_to_int — parse decimal string to integer.
 * Handles optional leading '-' sign.
 */
int my_str_to_int(const char *s) {
    int result = 0;
    int neg    = 0;
    if (*s == '-') { neg = 1; s++; }
    while (*s >= '0' && *s <= '9') {
        result = my_mul(result, 10) + (*s - '0');
        s++;
    }
    return neg ? -result : result;
}

/* ── String Contains ──────────────────────────────────────────────────── */
/**
 * my_strchr — find first occurrence of char c in s.
 * Returns pointer to char, or 0 (NULL) if not found.
 */
const char *my_strchr(const char *s, char c) {
    while (*s) {
        if (*s == c) return s;
        s++;
    }
    return 0;
}
