#ifndef MESH_H_
#define MESH_H_

#include "g3d/g3d.h"

struct vec3 {
	int32_t x, y, z;
};
struct meshface {
	short prim;	/* 3 - triangle / 4 - quad */
	unsigned short vidx[4];
	unsigned char color;
	struct vec3 norm;
};

struct mesh {
	struct g3d_vertex *varr;
	struct meshface *faces;
	int nverts, nfaces;
	unsigned short *zorder;
};

void sort_mesh(struct mesh *m, const int32_t *mvmat);
void draw_mesh(struct mesh *m);
void draw_mesh_zorder(struct mesh *m);

#endif	/* MESH_H_ */
