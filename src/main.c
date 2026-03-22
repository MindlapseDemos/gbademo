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

void g3d_xform_loop(struct g3d_vertex *v, const int32_t *m);
void g3d_xform_lowreg(struct g3d_vertex *v, const int32_t *m);
void g3d_xform_lowreg_loop(struct g3d_vertex *v, const int32_t *m);
void g3d_xform_nosav(struct g3d_vertex *v, const int32_t *m);

static void run_test(void)
{
	int i, row = 0;
	unsigned int res;
	struct g3d_vertex v2, v1 = {3, -5, 2, 1};
	int32_t matrix[16];

	glyphcolor = 0xff;
	glyphfb = (void*)VRAM_LFB_FB0_ADDR;
	gba_setbgcolor(0, 0, 0, 0);
	gba_setbgcolor(0xff, 31, 31, 31);

	mat_rotz(matrix, 3);

	dbg_drawstr(0, row, "mat4x4 x vec4 transform tests");
	printf("mat4x4 x vec4 transform tests\n");
	row += 16;

	reset_msec_timer();
	for(i=0; i<ITER; i++) {
		v2 = v1;
		g3d_xform(&v2, matrix);
		g3d_xform(&v2, matrix);
		g3d_xform(&v2, matrix);
		g3d_xform(&v2, matrix);
		g3d_xform(&v2, matrix);
		g3d_xform(&v2, matrix);
		g3d_xform(&v2, matrix);
		g3d_xform(&v2, matrix);
	}
	res = timer_msec;
	dbg_drawstr(0, row, "many regs unrolled: %u ms", res);
	printf("many regs unrolled: %u ms", res);
	row += 10;

	reset_msec_timer();
	for(i=0; i<ITER; i++) {
		v2 = v1;
		g3d_xform_loop(&v2, matrix);
		g3d_xform_loop(&v2, matrix);
		g3d_xform_loop(&v2, matrix);
		g3d_xform_loop(&v2, matrix);
		g3d_xform_loop(&v2, matrix);
		g3d_xform_loop(&v2, matrix);
		g3d_xform_loop(&v2, matrix);
		g3d_xform_loop(&v2, matrix);
	}
	res = timer_msec;
	dbg_drawstr(0, row, "many regs loop: %u ms", res);
	printf("many regs loop: %u ms", res);
	row += 10;

	reset_msec_timer();
	for(i=0; i<ITER; i++) {
		v2 = v1;
		g3d_xform_lowreg(&v2, matrix);
		g3d_xform_lowreg(&v2, matrix);
		g3d_xform_lowreg(&v2, matrix);
		g3d_xform_lowreg(&v2, matrix);
		g3d_xform_lowreg(&v2, matrix);
		g3d_xform_lowreg(&v2, matrix);
		g3d_xform_lowreg(&v2, matrix);
		g3d_xform_lowreg(&v2, matrix);
	}
	res = timer_msec;
	dbg_drawstr(0, row, "fewer regs unroll: %u ms", res);
	printf("fewer regs unroll: %u ms", res);
	row += 10;

	reset_msec_timer();
	for(i=0; i<ITER; i++) {
		v2 = v1;
		g3d_xform_lowreg_loop(&v2, matrix);
		g3d_xform_lowreg_loop(&v2, matrix);
		g3d_xform_lowreg_loop(&v2, matrix);
		g3d_xform_lowreg_loop(&v2, matrix);
		g3d_xform_lowreg_loop(&v2, matrix);
		g3d_xform_lowreg_loop(&v2, matrix);
		g3d_xform_lowreg_loop(&v2, matrix);
		g3d_xform_lowreg_loop(&v2, matrix);
	}
	res = timer_msec;
	dbg_drawstr(0, row, "fewer regs loop: %u ms", res);
	printf("fewer regs loop: %u ms", res);
	row += 10;

	reset_msec_timer();
	for(i=0; i<ITER; i++) {
		v2 = v1;
		g3d_xform_nosav(&v2, matrix);
		g3d_xform_nosav(&v2, matrix);
		g3d_xform_nosav(&v2, matrix);
		g3d_xform_nosav(&v2, matrix);
		g3d_xform_nosav(&v2, matrix);
		g3d_xform_nosav(&v2, matrix);
		g3d_xform_nosav(&v2, matrix);
		g3d_xform_nosav(&v2, matrix);
	}
	res = timer_msec;
	dbg_drawstr(0, row, "nosave unrolled: %u ms", res);
	printf("nosave unrolled: %u ms", res);
	row += 10;

	for(;;);
}
#endif
