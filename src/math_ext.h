#ifndef MATHEXT_H
#define MATHEXT_H

#include <stdint.h>

#define ABS(a)      ((a > 0) ? a : -a)
#define MAX(a, b)   ((a > b) ? a : b)
#define MIN(a, b)   ((a < b) ? a : b)

int16_t linear_interp(int16_t x, int16_t x0, int16_t x1, int16_t y0, int16_t y1);

int16_t bilinear_interp(int16_t x, int16_t y,
    int16_t z0, int16_t z1, int16_t z2, int16_t z3,
    int16_t x0, int16_t x1, int16_t y0, int16_t y1);

uint8_t table_index(int16_t *x, uint8_t n, int16_t vals[]);

int16_t table1d_lookup(int16_t x, uint8_t n, int16_t x_vals[], int16_t y_vals[]);

int16_t table2d_lookup(int16_t x, int16_t y, uint8_t nx, uint8_t ny,
    int16_t vx[], int16_t vy[], int16_t data[]);

#endif
