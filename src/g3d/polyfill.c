#include <stdio.h>
#include <string.h>
#include "g3d.h"
#include "util.h"

static void filltop(struct g3d_vertex *v0, struct g3d_vertex *v1, struct g3d_vertex *v2);
static void fillbot(struct g3d_vertex *v0, struct g3d_vertex *v1, struct g3d_vertex *v2);
static void fillspan(unsigned char *dest, unsigned int x, unsigned int len);


void g3d_polyfill(struct g3d_vertex *verts)
{
	int i, topidx, botidx, mididx;
	int32_t dx, dy, dym;
	int32_t v01x, v01y, v02x, v02y;
	struct g3d_vertex vtmp;

	/* calculate winding */
	v01x = verts[1].x - verts[0].x;
	v01y = verts[1].y - verts[0].y;
	v02x = verts[2].x - verts[0].x;
	v02y = verts[2].y - verts[0].y;
	if(!((v01x * v02y - v02x * v01y) & 0x80000000)) {
		return;
	}

	topidx = botidx = 0;
	for(i=1; i<3; i++) {
		if(verts[i].y < verts[topidx].y) {
			topidx = i;
		}
		if(verts[i].y > verts[botidx].y) {
			botidx = i;
		}
	}
	mididx = topidx + 1; if(mididx > 2) mididx = 0;
	if(mididx == botidx) {
		if(++mididx > 2) mididx = 0;
	}

	dy = verts[botidx].y - verts[topidx].y;
	if(dy == 0) return;

	dx = verts[botidx].x - verts[topidx].x;
	dym = verts[mididx].y - verts[topidx].y;
	vtmp.x = dx * dym / dy + verts[topidx].x;
	vtmp.y = verts[mididx].y;

	if(verts[topidx].y != verts[mididx].y) {
		filltop(verts + topidx, verts + mididx, &vtmp);
	}
	if(verts[mididx].y != verts[botidx].y) {
		fillbot(verts + mididx, &vtmp, verts + botidx);
	}
}

static void filltop(struct g3d_vertex *v0, struct g3d_vertex *v1, struct g3d_vertex *v2)
{
	struct g3d_vertex *vtmp;
	int x, line, lasty, len;
	int32_t xl, xr, dxl, dxr, slopel, sloper, dy;
	int32_t y0, yoffs;
	unsigned char *fbptr;

	if(v1->x > v2->x) {
		vtmp = v1;
		v1 = v2;
		v2 = vtmp;
	}

	dy = v1->y - v0->y;
	dxl = v1->x - v0->x;
	dxr = v2->x - v0->x;
	slopel = (dxl << 8) / dy;
	sloper = (dxr << 8) / dy;

	y0 = (v0->y + 0x100) & 0xffffff00;	/* start from the next scanline */
	yoffs = y0 - v0->y;					/* offset of the next scanline */
	xl = v0->x + ((yoffs * slopel) >> 8);
	xr = v0->x + ((yoffs * sloper) >> 8);

	line = y0 >> 8;
	lasty = v1->y >> 8;
	if(lasty >= YRES) lasty = YRES - 1;
	x = xl >> 8;

	fbptr = g3d_fbpixels + line * XRES;

	while(line <= lasty) {
		if(line >= 0) {
			len = ((xr + 0x100) >> 8) - (xl >> 8);
			if(len > 0) fillspan(fbptr, x, len);
		}

		xl += slopel;
		xr += sloper;
		x = xl >> 8;
		fbptr += XRES;
		line++;
	}
}

static void fillbot(struct g3d_vertex *v0, struct g3d_vertex *v1, struct g3d_vertex *v2)
{
	struct g3d_vertex *vtmp;
	int x, line, lasty, len;
	int32_t xl, xr, dxl, dxr, slopel, sloper, dy;
	int32_t y0, yoffs;
	unsigned char *fbptr;

	if(v0->x > v1->x) {
		vtmp = v0;
		v0 = v1;
		v1 = vtmp;
	}

	dy = v2->y - v0->y;
	dxl = v2->x - v0->x;
	dxr = v2->x - v1->x;
	slopel = (dxl << 8) / dy;
	sloper = (dxr << 8) / dy;

	y0 = (v0->y + 0x100) & 0xffffff00;	/* start from the next scanline */
	yoffs = y0 - v0->y;					/* offset of the next scanline */
	xl = v0->x + ((yoffs * slopel) >> 8);
	xr = v1->x + ((yoffs * sloper) >> 8);

	line = y0 >> 8;
	lasty = v2->y >> 8;
	if(lasty >= YRES) lasty = YRES - 1;
	x = xl >> 8;

	fbptr = g3d_fbpixels + line * XRES;

	while(line <= lasty) {
		if(line >= 0) {
			len = ((xr + 0x100) >> 8) - (xl >> 8);
			if(len > 0) fillspan(fbptr, x, len);
		}

		xl += slopel;
		xr += sloper;
		x = xl >> 8;
		fbptr += XRES;
		line++;
	}
}

static void fillspan(unsigned char *dest, unsigned int x, unsigned int len)
{
	uint16_t pair, *pptr;

	if(!len) return;

	pptr = (uint16_t*)dest + (x >> 1);

	if(x & 1) {
		/* handle odd start of span with read-mask-write */
		pair = *pptr & 0xff;
		*pptr++ = pair | (g3d_curcidx << 8);
		len--;
	}

	pair = g3d_curcidx | (g3d_curcidx << 8);

	while(len >= 2) {
		*pptr++ = pair;
		len -= 2;
	}

	if(len) {
		/* handle off end of span ... */
		pair = *pptr & 0xff00;
		*pptr = pair | g3d_curcidx;
	}
}
