/**
 * keyboard.h — Keyboard Driver Header
 * =====================================
 * Non-blocking raw terminal input driver.
 */

#ifndef KEYBOARD_H
#define KEYBOARD_H

/* Arrow key escape-sequence codes (> ASCII range to avoid conflicts) */
#define KEY_UP    256
#define KEY_DOWN  257
#define KEY_LEFT  258
#define KEY_RIGHT 259

void keyboard_init(void);     /* switch terminal to raw/non-blocking mode */
void keyboard_restore(void);  /* restore original terminal settings       */
int  read_key(void);          /* non-blocking key read; returns -1 if none */

#endif /* KEYBOARD_H */
