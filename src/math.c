#include "maths.h"

int my_clamp(int value, int min, int max) {
    if (value < min) return min;
    if (value > max) return max;
    return value;
}

int my_abs(int value) {
    return value < 0 ? -value : value;
}
