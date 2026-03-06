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

/* --- DMA --- */

/* DMA Register Parts */
#define DMA_SRC		0
#define DMA_DST		1
#define DMA_CTRL	2

static volatile uint32_t *reg_dma[4] = {(void*)0x040000b0, (void*)0x040000bc, (void*)0x040000c8, (void*)0x040000d4 };

/* --- perform a copy of words or halfwords using DMA --- */

void dma_copy32(int channel, void *dst, void *src, int words, unsigned int flags)
{
	reg_dma[channel][DMA_SRC] = (uint32_t)src;
	reg_dma[channel][DMA_DST] = (uint32_t)dst;
	reg_dma[channel][DMA_CTRL] = words | flags | DMACNT_32BIT | DMACNT_EN;
}

void dma_copy16(int channel, void *dst, void *src, int halfwords, unsigned int flags)
{
	reg_dma[channel][DMA_SRC] = (uint32_t)src;
	reg_dma[channel][DMA_DST] = (uint32_t)dst;
	reg_dma[channel][DMA_CTRL] = halfwords | flags | DMACNT_16BIT | DMACNT_EN;
}

/* --- fill a buffer with an ammount of words and halfwords using DMA --- */

static uint32_t fill[4];

void dma_fill32(int channel, void *dst, uint32_t val, int words)
{
	fill[channel] = val;
	reg_dma[channel][DMA_SRC] = (uint32_t)(fill + channel);
	reg_dma[channel][DMA_DST] = (uint32_t)dst;
	reg_dma[channel][DMA_CTRL] = words | DMACNT_SRC_FIXED | DMACNT_32BIT | DMACNT_EN;
}

void dma_fill16(int channel, void *dst, uint16_t val, int halfwords)
{
	fill[channel] = val;
	reg_dma[channel][DMA_SRC] = (uint32_t)(fill + channel);
	reg_dma[channel][DMA_DST] = (uint32_t)dst;
	reg_dma[channel][DMA_CTRL] = halfwords | DMACNT_SRC_FIXED | DMACNT_16BIT | DMACNT_EN;
}
