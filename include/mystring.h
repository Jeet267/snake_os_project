/**
 * mystring.h — Custom String Library Header
 * ==========================================
 * No <string.h> dependency. All operations use pointer arithmetic.
 * Integer ↔ string conversions use bitwise math from maths.h.
 */

#ifndef MYSTRING_H
#define MYSTRING_H

int         my_strlen(const char *s);
char       *my_strcpy(char *dst, const char *src);
int         my_strcmp(const char *a, const char *b);
char       *my_strcat(char *dst, const char *src);
char       *my_int_to_str(int n, char *buf);
int         my_str_to_int(const char *s);
const char *my_strchr(const char *s, char c);

#endif /* MYSTRING_H */
