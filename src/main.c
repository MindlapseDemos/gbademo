#include <stdio.h>
#include <stdlib.h>
#include "gba.h"
#include "timer.h"
#include "util.h"
#include "debug.h"

static void draw_frame(void);

#ifdef TESTPROG
static void run_test(void);
#endif

int main(void)
{
	intr_init();

	REG_WAITCNT = WAITCNT_PREFETCH | WAITCNT_ROM_2_1;

	gba_setmode(MODE_LFB_240X160_8, 0);
	gba_setbgcolor(1, 31, 0, 0);

	reset_msec_timer();
	intr_enable();

	vblperf_setcolor(0);

#ifdef TESTPROG
	run_test();
#endif

	for(;;) {
		draw_frame();
	}
	return 0;
}

static void draw_frame(void)
{
	static unsigned int qpix = 1;
	vblperf_begin();

	fill_16byte(gba_lfb_back, qpix, 240 * 160 / 16);
	qpix = (qpix >> 8) | (qpix << 24);

	vblperf_end();
	gba_vsync();
	gba_pgflip();
}

#ifdef TESTPROG
void *memcpy_c(void *dst, const void *src, size_t n);
void *memcpy_asm(void *dst, const void *src, size_t n);

static void run_test(void)
{
	int i, j, start, end, to, sz;
	int srcbuf[1024];
	int dstbuf1[1024], dstbuf2[1024];
	unsigned char *buf1, *buf2;

	printf("test init\n");
	for(i=0; i<1024; i++) {
		srcbuf[i] = rand();
	}

	for(i=0; i<500; i++) {
		start = rand() & 0xfff;
		end = rand() & 0xfff;
		if(start > end) {
			int tmp = start;
			start = end;
			end = tmp;
		}
		sz = end - start;
		to = rand() & 3;
		if(to + sz > 4096) to = 0;

		memcpy_c(dstbuf1 + to, srcbuf + start, sz);
		memcpy_asm(dstbuf2 + to, srcbuf + start, sz);

		buf1 = (unsigned char*)dstbuf1 + to;
		buf2 = (unsigned char*)dstbuf2 + to;
		for(j=0; j<sz; j++) {
			if(buf1[j] != buf2[j]) {
				printf("mismatch at %d\n", j);
				printf("mc %d bytes from %p\n", sz, srcbuf + start);
				printf("  too: %p\n", dstbuf2 + to);
				for(;;);
			}
		}
	}

	printf("test passed\n");
	for(;;);
}
#endif
