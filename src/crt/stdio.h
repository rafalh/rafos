#ifndef _STDIO_H_
#define _STDIO_H_

#include <stdarg.h>
#include <stddef.h>

int getchar(void);
char *gets(char *str);

int putchar(int c);
int puts(const char *s);
int printf(const char *format, ...);
int sprintf(char *str, const char *format, ...);
int vprintf(const char *format, va_list arg);
int vsprintf(char *str, const char *format, va_list arg);

int _snprintf(char *buffer, size_t count, const char *format, ...);
int _vsnprintf(char *buffer, size_t count, const char *format, va_list arg);

#endif /* _STDIO_H_ */
