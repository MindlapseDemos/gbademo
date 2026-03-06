#include <stdio.h>
#include "../debug.h"

int putchar(int c)
{
#ifdef EMUBUILD
	static char buf[128];
	static char *bufptr = buf;

	if(c == '\n') {
		*bufptr = 0;
		bufptr = buf;
		emuprint(buf);
	} else {
		if(bufptr < buf + sizeof buf) {
			*bufptr++ = c;
		}
	}
#endif
	return c;
}

int puts(const char *s)
{
	while(*s) putchar(*s++);
	putchar('\n');
	return 0;
}
