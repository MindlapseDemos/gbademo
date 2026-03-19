#include <string.h>
#include <stdint.h>

size_t strlen(const char *s)
{
	const char *end = s;
	while(*end) end++;
	return end - s;
}

char *strcpy(char *dst, const char *src)
{
	char *orig_dst = dst;
	while((*dst++ = *src++));
	return orig_dst;
}

/* TODO compare larger chunks */
int memcmp(const void *s1, const void *s2, size_t n)
{
	const uint8_t *a8 = s1;
	const uint8_t *b8 = s2;

	while(n-- > 0) {
		int d = *a8 - *b8;
		if(d != 0) return d;
		a8++;
		b8++;
	}
	return 0;
}

/* TODO fill larget blocks */
void *memset(void *dst, int c, size_t n)
{
	unsigned char *ptr = dst;
	while(n-- > 0) *ptr++ = c;
	return dst;
}


char *strchr(const char *s, int c)
{
	while(*s) {
		if(*s == c) return (char*)s;
		s++;
	}
	return 0;
}

char *strrchr(const char *s, int c)
{
	const char *ptr;

	ptr = s + strlen(s) - 1;
	while(ptr >= s) {
		if(*ptr == c) return (char*)ptr;
		ptr--;
	}
	return 0;
}
