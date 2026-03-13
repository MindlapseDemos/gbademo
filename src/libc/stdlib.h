#ifndef STDLIB_H_
#define STDLIB_H_

typedef unsigned int size_t;
typedef int ssize_t;

int atoi(const char *str);
long atol(const char *str);

void *malloc(size_t sz);
void *calloc(size_t n, size_t sz);
void *realloc(void *p, size_t sz);
void free(void *p);

#endif	/* STDLIB_H_ */
