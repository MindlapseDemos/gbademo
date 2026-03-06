#ifndef STRING_H_
#define STRING_H_

#include <stdlib.h>

size_t strlen(const char *s);
char *strcpy(char *dst, const char *src);

void *memcpy(void *dst, const void *src, size_t n);
void *memmove(void *dst, const void *src, size_t n);

#endif	/* STRING_H_ */
