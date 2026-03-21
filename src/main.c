#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "gba.h"
#include "timer.h"
#include "util.h"
#include "debug.h"
#include "g3dtest.h"

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
	gba_setbgcolor(0xff, 31, 31, 31);

	reset_msec_timer();
	intr_enable();

	g3dtest_init();

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
	vblperf_begin();

	fill_16byte(gba_lfb_back, 0, 240 * 160 / 16);

	g3dtest_draw();

	vblperf_end();
	gba_vsync();
	gba_pgflip();
}

#ifdef TESTPROG
#define ITER	1000
#define BUFSZ	4096

void *memset_c(void *dst, int c, size_t n);

static void run_test(void)
{
	static unsigned char dstbuf[BUFSZ];
	int i, j, row;
	unsigned int res, sz;

	glyphcolor = 0xff;
	glyphfb = (void*)VRAM_LFB_FB0_ADDR;
	gba_setbgcolor(0, 0, 0, 0);
	gba_setbgcolor(0xff, 31, 31, 31);

	row = 0;
	sz = BUFSZ;

	for(j=0; j<3; j++) {
		dbg_drawstr(0, row, "fill %u bytes %u times", sz, ITER);
		printf("fill %u bytes %u times\n", sz, ITER);
		row += 14;

		reset_msec_timer();
		for(i=0; i<ITER; i++) {
			memset_c(dstbuf, 42, sz);
		}
		res = timer_msec;
		dbg_drawstr(8, row, "memset simple C: %u ms", res);
		printf(" memset C: %u ms\n", res);
		row += 8;

		reset_msec_timer();
		for(i=0; i<ITER; i++) {
			memset(dstbuf, 42, sz);
		}
		res = timer_msec;
		dbg_drawstr(8, row, "memset ASM: %u ms", res);
		printf(" memset ASM: %u ms\n", res);
		row += 8;

		reset_msec_timer();
		for(i=0; i<ITER; i++) {
			fill_16byte(dstbuf, 0xfedcba98, sz >> 4);
		}
		res = timer_msec;
		dbg_drawstr(8, row, "fill_16b: %u ms", res);
		printf(" fill_16b: %u ms\n", res);
		row += 8;

		reset_msec_timer();
		for(i=0; i<ITER; i++) {
			dma_fill32(3, dstbuf, 0x89abcdef, sz >> 2);
		}
		res = timer_msec;
		dbg_drawstr(8, row, "dma_fill32: %u ms", res);
		printf(" dma_fill32: %u ms\n\n", res);

		row += 18;
		sz >>= 3;
	}
	for(;;);
}
#endif
