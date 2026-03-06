#ifndef CTYPE_H_
#define CTYPE_H_

static inline int isupper(int c) { return c >= 'A' && c <= 'Z'; }
static inline int islower(int c) { return c >= 'a' && c <= 'z'; }
static inline int isalpha(int c) { return isupper(c) || islower(c); }
static inline int isdigit(int c) { return c >= '0' && c <= '9'; }
static inline int isalnum(int c) { return isalpha(c) || isdigit(c); }
static inline int isascii(int c) { return c < 128; }
static inline int isblank(int c) { return c == ' ' || c == '\t'; }
static inline int isgraph(int c) { return c > ' ' && c <= '~'; }
static inline int isprint(int c) { return isgraph(c) || c == ' '; }

static inline int isspace(int c)
{
	return isblank(c) || c == '\f' || c == '\n' || c == '\r' || c == '\v';
}

static inline int toupper(int c) { return c & 0xdf; }
static inline int tolower(int c) { return c | 0x20; }

#endif	/* CTYPE_H_ */
