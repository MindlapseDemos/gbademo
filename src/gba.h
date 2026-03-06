#ifndef GBA_H_
#define GBA_H_

#include <stdint.h>
#include "gbaregs.h"
#include "intr.h"

#define gba_bgpal		((uint16_t*)CRAM_BG_ADDR)
#define gba_objpal		((uint16_t*)CRAM_OBJ_ADDR)

#define gba_vram		((uint16_t*)VRAM_START_ADDR)
#define gba_vram_lfb0	((uint16_t*)VRAM_LFB_FB0_ADDR)
#define gba_vram_lfb1	((uint16_t*)VRAM_LFB_FB1_ADDR)

enum {
	MODE_LFB_240X160_16		= 3,
	MODE_LFB_240X160_8		= 4,
	MODE_LFB_160X128_16		= 5
};

extern void *gba_lfb[2];
extern unsigned short gba_lfb_curidx;
extern void *gba_lfb_back;

void gba_setmode(int mode, unsigned int flags);

#define gba_setbgcolor(idx, r, g, b) \
	gba_bgpal[idx] = (uint16_t)(r) | ((uint16_t)(g) << 5) | ((uint16_t)(b) << 10)

#define gba_vsync() \
	do { \
		while(REG_DISPSTAT & DISPSTAT_VBLANK); \
		while(!(REG_DISPSTAT & DISPSTAT_VBLANK)); \
	} while(0)

#define gba_pgflip() \
	do { \
		gba_lfb_back = gba_lfb[gba_lfb_curidx]; \
		gba_lfb_curidx ^= 1; \
		REG_DISPCNT = (REG_DISPCNT & 0xffef) | (gba_lfb_curidx << 4); \
	} while(0)

void dma_copy32(int channel, void *dst, void *src, int words, unsigned int flags);
void dma_copy16(int channel, void *dst, void *src, int halfwords, unsigned int flags);

void dma_fill32(int channel, void *dst, uint32_t val, int words);
void dma_fill16(int channel, void *dst, uint16_t val, int halfwords);

#endif	/* GBA_H_ */
