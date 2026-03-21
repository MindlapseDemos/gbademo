#ifndef XMATH_H_
#define XMATH_H_

#include <stdint.h>

#define SINLUT_SIZE		512
#define SINLUT_MASK		(SINLUT_SIZE - 1)
#define SINLUT_SCALE	8192

extern int32_t sintab[];

/* sin(x) lookup -> 16.16 fixed point */
#define XSIN(x)	(sintab[(x) & SINLUT_MASK] << 3)
#define XCOS(x)	(sintab[((x) + SINLUT_SIZE / 4) & SINLUT_MASK] << 3)


void mat_identity(int32_t *m);

void mat_trans(int32_t *m, int32_t x, int32_t y, int32_t z);
void mat_rotx(int32_t *m, int32_t theta);
void mat_roty(int32_t *m, int32_t theta);
void mat_rotz(int32_t *m, int32_t theta);

void mat_mul_trans(int32_t *m, int32_t x, int32_t y, int32_t z);
void mat_mul_rotx(int32_t *m, int32_t theta);
void mat_mul_roty(int32_t *m, int32_t theta);
void mat_mul_rotz(int32_t *m, int32_t theta);

void mat_perspective(int32_t *m, int vfov, int32_t aspect, int32_t znear, int32_t zfar);

void mat_mult(int32_t *ma, int32_t *mb);

#endif	/* XMATH_H_ */
