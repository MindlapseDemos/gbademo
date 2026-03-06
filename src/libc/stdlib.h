#ifndef STDLIB_H_
#define STDLIB_H_

#define RAND_MAX	2147483647

typedef unsigned int size_t;
typedef int ssize_t;

int atoi(const char *str);
long atol(const char *str);

long strtol(const char *str, char **endp, int base);

void itoa(int val, char *buf, int base);
void utoa(unsigned int val, char *buf, int base);

double atof(const char *str);
double strtod(const char *str, char **endp);

void qsort(void *arr, size_t count, size_t itemsz, int (*cmp)(const void*, const void*));

int rand(void);
void srand(unsigned int seed);

void *malloc(size_t sz);
void *calloc(size_t n, size_t sz);
void *realloc(void *p, size_t sz);
void free(void *p);

#endif	/* STDLIB_H_ */
