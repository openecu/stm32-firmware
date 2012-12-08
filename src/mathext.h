#ifndef _MATHEXT_H
#define _MATHEXT_H

#include <inttypes.h>

int16_t linear_interp(int16_t x, int16_t y0, int16_t y1, int16_t x0, int16_t x1);

int16_t bilinear_interp(
    int16_t x, int16_t y,
    int16_t z0, int16_t z1, int16_t z2, int16_t z3,
    int16_t x0, int16_t x1,
    int16_t y0, int16_t y1
);

uint8_t table_index(int16_t *value, int16_t values[], uint8_t size);

int16_t table1d_lookup(int16_t x, uint8_t nx, int16_t vx[], int16_t *data);

int16_t table2d_lookup(
    int16_t x, int16_t y,
    uint8_t nx, uint8_t ny,
    int16_t vx[], int16_t vy[],
    int16_t **data
);

#endif
