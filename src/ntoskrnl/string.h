#ifndef _STRING_H_
#define _STRING_H_

#include <stddef.h>

size_t strlen(const char *str);

int strcmp(const char *str1, const char *str2);
int stricmp(const char *str1, const char *str2);

int strncmp(const char *str1, const char *str2, size_t num);
int strnicmp(const char *str1, const char *str2, size_t num);

char *strcpy(char *destination, const char *source);
char *strncpy(char *destination, const char *source, size_t num);

void *memcpy(void *destination, const void *source, size_t num);
int memcmp(const void *ptr1, const void *ptr2, size_t num);
void *memset(void *ptr, int value, size_t num);

void *memmove(void *dest, const void *src, size_t count);

#endif /* _STDIO_H_ */
