#ifndef KEYBOARD_H
#define KEYBOARD_H

/**
 * Basic keyboard driver to provide non-blocking input
 * simulating hardware interrupts.
 */

#define KEY_UP 1000
#define KEY_DOWN 1001
#define KEY_LEFT 1002
#define KEY_RIGHT 1003

void keyboard_init(void);
int read_key(void);
void keyboard_restore(void);

#endif // KEYBOARD_H
