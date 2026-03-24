#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "gba.h"
#include "timer.h"
#include "util.h"
#include "debug.h"
#include "g3d/g3d.h"
#include "xmath.h"
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
#define ITER	3000

void mat_mult_asm(int32_t *ma, const int32_t *mb);

static void run_test(void)
{
	int i, row = 0;
	unsigned int res;
	int32_t ma[16], mb[16];

	glyphcolor = 0xff;
	glyphfb = (void*)VRAM_LFB_FB0_ADDR;
	gba_setbgcolor(0, 0, 0, 0);
	gba_setbgcolor(0xff, 31, 31, 31);

	mat_rotz(mb, 3);

	row = 16;

	reset_msec_timer();
	for(i=0; i<ITER; i++) {
		mat_identity(ma);
		mat_mult(ma, mb);
		mat_mult(ma, mb);
		mat_mult(ma, mb);
		mat_mult(ma, mb);
	}
	res = timer_msec;
	dbg_drawstr(0, row, "matrix mult C: %u ms", res);
	printf("matrix mult C: %u ms\n", res);
	row += 12;

	reset_msec_timer();
	for(i=0; i<ITER; i++) {
		mat_identity(ma);
		mat_mult_asm(ma, mb);
		mat_mult_asm(ma, mb);
		mat_mult_asm(ma, mb);
		mat_mult_asm(ma, mb);
	}
	res = timer_msec;
	dbg_drawstr(0, row, "matrix mult ASM: %u ms", res);
	printf("matrix mult ASM: %u ms\n", res);
	row += 12;

	for(;;);
}
#endif
