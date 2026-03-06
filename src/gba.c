#include "gba.h"

void *gba_lfb[2] = {gba_vram_lfb0, gba_vram_lfb1};
unsigned short gba_lfb_curidx;
void *gba_lfb_back;

void gba_setmode(int mode, unsigned int flags)
{
	if(mode >= 3) {
		flags |= DISPCNT_BG2;
		gba_lfb_curidx = 0;
		gba_lfb_back = gba_lfb[1];
	}
	REG_DISPCNT = mode | flags;
}

