#include <stdio.h>
#include "gba.h"
#include "g3d/g3d.h"
#include "mesh.h"
#include "util.h"
#include "xmath.h"

#define VERT(x, y, z) { (x) << 16, (y) << 16, (z) << 16, 0x10000 }
static struct g3d_vertex cube_varr[] = {
	VERT(-1, -1, 1), VERT(1, -1, 1), VERT(1, -1, -1), VERT(-1, -1, -1),
	VERT(-1, 1, 1), VERT(1, 1, 1), VERT(1, 1, -1), VERT(-1, 1, -1)
};
static struct meshface cube_faces[] = {
	{4, {0, 1, 5, 4}},
	{4, {1, 2, 6, 5}},
	{4, {2, 3, 7, 6}},
	{4, {3, 0, 4, 7}},
	{4, {4, 5, 6, 7}},
	{4, {3, 2, 1, 0}}
};

static struct mesh mesh;


int g3dtest_init(void)
{
	int i;
	int32_t proj[16];

	mesh.varr = cube_varr;
	mesh.nverts = sizeof cube_varr / sizeof *cube_varr;
	mesh.faces = cube_faces;
	mesh.nfaces = sizeof cube_faces / sizeof *cube_faces;

	gba_setbgcolor(8, 31, 0, 0);
	gba_setbgcolor(9, 0, 31, 0);
	gba_setbgcolor(10, 0, 0, 31);
	gba_setbgcolor(11, 31, 0, 31);
	gba_setbgcolor(12, 31, 31, 0);
	gba_setbgcolor(13, 0, 31, 31);

	for(i=0; i<mesh.nfaces; i++) {
		mesh.faces[i].color = i + 8;
	}

	g3d_init();
	g3d_framebuffer(240, 160, 0);

	mat_perspective(proj, 50, (4 << 16) / 3, 0x8000, 0x100000);
	g3d_projection(proj);

	return 0;
}

void g3dtest_draw(void)
{
	static unsigned int nframes;
	int32_t xform[16];

	mat_trans(xform, 0, 0, -0x70000);
	mat_mul_rotx(xform, nframes);
	mat_mul_roty(xform, nframes);
	g3d_modelview(xform);

	g3d_fbpixels = gba_lfb_back;

	//sort_mesh(&mesh, xform);
	draw_mesh(&mesh);

	nframes++;
}
