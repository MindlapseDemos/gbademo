#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "mesh.h"

static struct mesh *smesh;
static const int32_t *smat;

static int zsortcmp(const void *a, const void *b)
{
	struct meshface *fa, *fb;
	struct g3d_vertex *va, *vb;
	int32_t za, zb;

	fa = smesh->faces + *(unsigned short*)a;
	fb = smesh->faces + *(unsigned short*)b;

	va = smesh->varr + fa->vidx[0];
	vb = smesh->varr + fb->vidx[0];

	za = (smat[8] >> 8) * (va->x >> 8) + (smat[9] >> 8) * (va->y >> 8) +
		(smat[10] >> 8) * (va->z >> 8);
	zb = (smat[8] >> 8) * (vb->x >> 8) + (smat[9] >> 8) * (vb->y >> 8) +
		(smat[10] >> 8) * (vb->z >> 8);

	return za - zb;
}

void sort_mesh(struct mesh *m, const int32_t *mvmat)
{
	int i;

	if(!m->zorder) {
		if(!(m->zorder = malloc(m->nfaces * sizeof *m->zorder))) {
			return;
		}
		for(i=0; i<m->nfaces; i++) {
			m->zorder[i] = i;
		}
	}

	smesh = m;
	smat = mvmat;
	qsort(m->zorder, m->nfaces, sizeof *m->zorder, zsortcmp);
}

void draw_mesh(struct mesh *m)
{
	int i;
	struct meshface *face = m->faces;
	for(i=0; i<m->nfaces; i++) {
		g3d_color(face->color);
		g3d_draw_indexed(face->prim, m->varr, face->vidx, face->prim);
		face++;
	}
}

void draw_mesh_zorder(struct mesh *m)
{
	int i;
	struct meshface *face;

	for(i=0; i<m->nfaces; i++) {
		face = m->faces + m->zorder[i];
		g3d_color(face->color);
		g3d_draw_indexed(face->prim, m->varr, face->vidx, face->prim);
	}
}
