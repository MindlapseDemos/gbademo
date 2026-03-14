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

void *memcpy_c(void *dst, const void *src, size_t n)
{
	uint32_t *d32;
	const uint32_t *s32;
	uint8_t *d8 = dst;
	const uint8_t *s8 = src;

	/* tiny size or differing alignments: do byte copy */
	if(n < 4 || ((uintptr_t)dst & 3) != ((uintptr_t)src & 3)) {
		while(n-- > 0) *d8++ = *s8++;
		return dst;
	}

	switch((uintptr_t)dst & 3) {
	case 3: *d8++ = *s8++;
	case 2: *d8++ = *s8++;
	case 1: *d8++ = *s8++;
	}
	n -= (uintptr_t)dst & 3;

	d32 = (uint32_t*)d8;
	s32 = (const uint32_t*)s8;
	while(n >= 4) {
		*d32++ = *s32++;
		n -= 4;
	}

	d8 = (uint8_t*)d32;
	s8 = (const uint8_t*)s32;

	switch(n) {
	case 3: *d8++ = *s8++;
	case 2: *d8++ = *s8++;
	case 1: *d8++ = *s8++;
	}
	return dst;
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
