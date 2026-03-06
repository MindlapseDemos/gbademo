#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
#define ITER	1000
#define BUFSZ	4096
void *memcpy_c(void *dst, const void *src, size_t n);
void *memcpy_asm(void *dst, const void *src, size_t n);

static void run_test(void)
{
	static unsigned char srcbuf[BUFSZ], dstbuf[BUFSZ];
	int i, j, row;
	unsigned int res, sz;

	glyphcolor = 0xff;
	glyphfb = (void*)VRAM_LFB_FB0_ADDR;
	gba_setbgcolor(0, 0, 0, 0);
	gba_setbgcolor(0xff, 31, 31, 31);

	for(i=0; i<BUFSZ/4; i++) {
		((int*)srcbuf)[i] = rand();
	}

	row = 0;
	sz = BUFSZ;

	for(j=0; j<3; j++) {
		dbg_drawstr(0, row, "copy %u bytes %u times", sz, ITER);
		printf("test %u bytes %u times\n", sz, ITER);
		row += 14;

		reset_msec_timer();
		for(i=0; i<ITER; i++) {
			memcpy(dstbuf, srcbuf, sz);
		}
		res = timer_msec;
		dbg_drawstr(8, row, "simple: %u ms", res);
		printf(" simple: %u ms\n", res);
		row += 8;

		reset_msec_timer();
		for(i=0; i<ITER; i++) {
			memcpy_c(dstbuf, srcbuf, sz);
		}
		res = timer_msec;
		dbg_drawstr(8, row, "memcpy_c: %u ms", res);
		printf(" memcpy_c: %u ms\n", res);
		row += 8;

		reset_msec_timer();
		for(i=0; i<ITER; i++) {
			memcpy_asm(dstbuf, srcbuf, sz);
		}
		res = timer_msec;
		dbg_drawstr(8, row, "memcpy_asm: %u ms", res);
		printf(" memcpy_asm: %u ms\n", res);
		row += 8;

		reset_msec_timer();
		for(i=0; i<ITER; i++) {
			dma_copy32(3, dstbuf, srcbuf, sz >> 2, 0);
		}
		res = timer_msec;
		dbg_drawstr(8, row, "dma_copy32: %u ms", res);
		printf(" dma_copy32: %u ms\n\n", res);

		row += 18;
		sz >>= 3;
	}

	for(;;);
}
#endif
