#ifndef UTIL_H_
#define UTIL_H_

#include <stdlib.h>
#include <stdint.h>

#define ARM_IWRAM	__attribute__((noinline, target("arm"), section(".iwram")))

int iwram_brk(void *addr);
void *iwram_sbrk(intptr_t delta);

/* fills memory in 16-byte blocks, count in blocks */
void fill_16byte(void *dest, uint32_t val, int count);

void *get_pc(void);
void *get_sp(void);

/* Non-failing versions of malloc/calloc/realloc. They never return 0, they call
 * demo_abort on failure. Use the macros, don't call the *_impl functions.
 */
#define malloc_nf(sz)	malloc_nf_impl(sz, __FILE__, __LINE__)
void *malloc_nf_impl(size_t sz, const char *file, int line);
#define calloc_nf(n, sz)	calloc_nf_impl(n, sz, __FILE__, __LINE__)
void *calloc_nf_impl(size_t num, size_t sz, const char *file, int line);
#define realloc_nf(p, sz)	realloc_nf_impl(p, sz, __FILE__, __LINE__)
void *realloc_nf_impl(void *p, size_t sz, const char *file, int line);
#define strdup_nf(s)	strdup_nf_impl(s, __FILE__, __LINE__)
char *strdup_nf_impl(const char *s, const char *file, int line);


#endif	/* UTIL_H_ */
