#include "gba.h"
#include "timer.h"
#include "util.h"
#include "debug.h"

static void draw_frame(void);

int main(void)
{
	intr_init();

	REG_WAITCNT = WAITCNT_PREFETCH | WAITCNT_ROM_2_1;

	gba_setmode(MODE_LFB_240X160_8, 0);
	gba_setbgcolor(1, 31, 0, 0);

	reset_msec_timer();
	intr_enable();

	vblperf_setcolor(0);

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
