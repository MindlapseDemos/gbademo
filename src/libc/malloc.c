#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "debug.h"

struct mem_desc {
	size_t size;
	uint32_t magic;
#ifdef MALLOC_DEBUG
	uint32_t dbg;
#endif
	struct mem_desc *next;
};

#ifdef MALLOC_DEBUG
static void check_cycles(struct mem_desc *mem);
static void print_pool(void);
#endif

#define MAGIC_USED	0xdeadf00d
#define MAGIC_FREE	0x1ee7d00d

#define DESC_PTR(b)	((void*)((struct mem_desc*)(b) + 1))
#define PTR_DESC(p)	((struct mem_desc*)(p) - 1)

static struct mem_desc *pool = (void*)0xffffffff;

#define MIN_BLOCK_SIZE		(sizeof(struct mem_desc) * 2)


void *malloc(size_t sz)
{
	size_t total_sz;
	struct mem_desc *mem, *prev, dummy;
	int found = 0;

	if(pool == (void*)0xffffffff) {
		/* first call, initialize pool */
		pool = (struct mem_desc*)0x2000000;		/* start of EWRAM */
		pool->magic = MAGIC_FREE;
		pool->next = 0;
		pool->size = 256 * 1024;
	}

	total_sz = (sz + sizeof(struct mem_desc) + 3) & 0xfffffffc;

	dummy.next = pool;
	prev = &dummy;
	while(prev->next) {
		mem = prev->next;
		/* give the whole block to the allocation if mem->size == total_sz or
		 * if it's larger, but not large enough to fit another mem_desc in there
		 * for the new block that we're going to split off + some reasonable
		 * amount of memory for the new block.
		 */
		if(mem->size >= total_sz && mem->size < total_sz + MIN_BLOCK_SIZE) {
			prev->next = mem->next;
			found = 1;
			break;
		}
		/* if we have enough space, split the block and give the upper part
		 * to the allocation
		 */
		if(mem->size > total_sz) {
			void *ptr = (char*)mem + mem->size - total_sz;
			mem->size -= total_sz;
			mem = ptr;
			found = 1;
			break;
		}
		prev = prev->next;
	}
	pool = dummy.next;

	if(found) {
		mem->size = total_sz;
		mem->magic = MAGIC_USED;
		mem->next = 0;
		return DESC_PTR(mem);
	}

	/* did not find a free block */
	return 0;
}

void free(void *p)
{
	struct mem_desc *mem, *prev, *cur, dummy;
	char *end;

	if(!p) return;
	mem = PTR_DESC(p);

	if(mem->magic != MAGIC_USED) {
		if(mem->magic == MAGIC_FREE) {
			panic("free(%p): double-free\n", p);
		} else {
			panic("free(%p): corrupted magic (%x)!\n", p, mem->magic);
		}
	}
	mem->magic = MAGIC_FREE;
	mem->next = 0;

	/* nothing in the pool, just add this one */
	if(!pool) {
		pool = mem;
		return;
	}

	end = (char*)mem + mem->size;

	dummy.next = pool;
	prev = &dummy;

	while(prev->next) {
		cur = prev->next;

		/* block starts right at the end of mem: coalesce */
		if((char*)cur == end) {
			mem->size += cur->size;
			mem->next = cur->next;
			cur->magic = 0;
			prev->next = mem;
			goto done;
		}

		/* block starts *after* the end of mem: add in front */
		if((char*)cur > end) {
			mem->next = cur;
			prev->next = mem;
			goto done;
		}

		prev = prev->next;
	}

	/* our block starts at the end of the last block in the pool: coalesce */
	if((char*)prev + prev->size == (char*)mem) {
		mem->magic = 0;
		prev->size += mem->size;
		goto done;
	}

	/* so our block starts after the end of the last block: append */
	prev->next = mem;

done:
	pool = dummy.next;

#ifdef MALLOC_DEBUG
	print_pool();
#endif
}

void *calloc(size_t n, size_t sz)
{
	void *ptr = malloc(n * sz);
	if(ptr) {
		memset(ptr, 0, n * sz);
	}
	return ptr;
}

void *realloc(void *ptr, size_t size)
{
	struct mem_desc *mem;
	void *newp;

	if(!ptr) {
		return malloc(size);
	}

	mem = PTR_DESC(ptr);
	if(mem->size >= size) {
		return ptr;	/* TODO: shrink */
	}

	if(!(newp = malloc(size))) {
		return 0;
	}
	memcpy(newp, ptr, mem->size);
	free(ptr);
	return newp;
}

#ifdef MALLOC_DEBUG
static void check_cycles(struct mem_desc *mem)
{
	static uint32_t dbg = 42;
	uint32_t cur_dbg = dbg++;

	while(mem) {
		if(mem->magic != MAGIC_FREE) {
			panic("check_cycles: NON-FREE MAGIC!\n");
		}
		if(mem->dbg == cur_dbg) {
			panic("CYCLE DETECTED\n");
		}
		mem->dbg = cur_dbg;
		mem = mem->next;
	}
}

static void print_pool(void)
{
	struct mem_desc *mem = pool;

	printf("DBG: malloc pool:\n");
	while(mem) {
		printf(" %p (%ld) [%x]\n", mem, mem->size, mem->magic);
		mem = mem->next;

		assert((uint32_t)mem != MAGIC_USED);
	}
}
#endif	/* MALLOC_DEBUG */
