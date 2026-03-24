#include <string.h>
#include "xmath.h"
#include "util.h"

void mat_mult_asm(int32_t *ma, int32_t *mb);

static const int32_t idmat[] = {
	0x10000, 0, 0, 0,
	0, 0x10000, 0, 0,
	0, 0, 0x10000, 0,
	0, 0, 0, 0x10000
};

void mat_identity(int32_t *m)
{
	memcpy(m, idmat, sizeof idmat);
}

void mat_trans(int32_t *m, int32_t x, int32_t y, int32_t z)
{
	memcpy(m, idmat, sizeof idmat);
	m[3] = x;
	m[7] = y;
	m[11] = z;
}

void mat_rotx(int32_t *m, int32_t theta)
{
	memcpy(m, idmat, sizeof idmat);
	m[5] = XCOS(theta);
	m[6] = XSIN(theta);
	m[9] = -XSIN(theta);
	m[10] = XCOS(theta);
}

void mat_roty(int32_t *m, int32_t theta)
{
	memcpy(m, idmat, sizeof idmat);
	m[0] = XCOS(theta);
	m[2] = -XSIN(theta);
	m[8] = XSIN(theta);
	m[10] = XCOS(theta);
}

void mat_rotz(int32_t *m, int32_t theta)
{
	memcpy(m, idmat, sizeof idmat);
	m[0] = XCOS(theta);
	m[1] = XSIN(theta);
	m[4] = -XSIN(theta);
	m[5] = XCOS(theta);
}

void mat_mul_trans(int32_t *m, int32_t x, int32_t y, int32_t z)
{
	int32_t tmp[16];
	mat_trans(tmp, x, y, z);
	mat_mult_asm(m, tmp);
}

void mat_mul_rotx(int32_t *m, int32_t theta)
{
	int32_t tmp[16];
	mat_rotx(tmp, theta);
	mat_mult_asm(m, tmp);
}

void mat_mul_rotz(int32_t *m, int32_t theta)
{
	int32_t tmp[16];
	mat_rotz(tmp, theta);
	mat_mult_asm(m, tmp);
}

void mat_mul_roty(int32_t *m, int32_t theta)
{
	int32_t tmp[16];
	mat_roty(tmp, theta);
	mat_mult_asm(m, tmp);
}

void mat_perspective(int32_t *m, int vfov, int32_t aspect, int32_t znear, int32_t zfar)
{
	float halffov = (float)vfov * 3.1415926536f / 360.0f;
	float s = 2.144;/*1.0f / (float)tan(halffov);*/
	float zn = (float)znear / 65536.0f;
	float zf = (float)zfar / 65536.0f;
	float dz = zn - zf;

	memset(m, 0, 16 * sizeof *m);
	m[0] = (int32_t)((s / ((float)aspect / 65536.0f)) * 65536.0f);
	m[5] = (int32_t)(s * 65536.0f);
	m[10] = (int32_t)((zn + zf) / dz * 65536.0f);
	m[11] = (int32_t)(2.0f * zn * zf / dz * 65536.0f);
	m[14] = -0x10000;
	m[15] = 0x10000;
}

void mat_mult(int32_t *ma, int32_t *mb)
{
	int i;
	int32_t res[16];
	int32_t *rptr = res;
	int32_t *row = ma;

	for(i=0; i<4; i++) {
		rptr[0] = ((int64_t)(row[0] * mb[0]) >> 16) + ((int64_t)(row[1] * mb[4]) >> 16) + ((int64_t)(row[2] * mb[8] ) >> 16) + ((int64_t)(row[3] * mb[12]) >> 16);
		rptr[1] = ((int64_t)(row[0] * mb[1]) >> 16) + ((int64_t)(row[1] * mb[5]) >> 16) + ((int64_t)(row[2] * mb[9] ) >> 16) + ((int64_t)(row[3] * mb[13]) >> 16);
		rptr[2] = ((int64_t)(row[0] * mb[2]) >> 16) + ((int64_t)(row[1] * mb[6]) >> 16) + ((int64_t)(row[2] * mb[10]) >> 16) + ((int64_t)(row[3] * mb[14]) >> 16);
		rptr[3] = ((int64_t)(row[0] * mb[3]) >> 16) + ((int64_t)(row[1] * mb[7]) >> 16) + ((int64_t)(row[2] * mb[11]) >> 16) + ((int64_t)(row[3] * mb[15]) >> 16);
		rptr += 4;
		row += 4;
	}

	memcpy(ma, res, sizeof res);
}
