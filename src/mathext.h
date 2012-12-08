#ifndef _MATHEXT_H
#define _MATHEXT_H

#include <inttypes.h>

uint16_t linear_interp(uint16_t x, uint16_t y0, uint16_t y1, uint16_t x0, uint16_t x1);

uint16_t bilinear_interp(
    uint16_t x, uint16_t y,
    uint16_t z0, uint16_t z1, uint16_t z2, uint16_t z3,
    uint16_t x0, uint16_t x1,
    uint16_t y0, uint16_t y1
);

uint8_t table_index(uint16_t *value, uint16_t values[], uint8_t size);

uint16_t table_lookup(
    uint16_t x, uint16_t y,
    uint8_t nx, uint8_t ny,
    uint16_t vx[], uint16_t vy[],
    uint16_t **data
);

#endif
