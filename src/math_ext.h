#ifndef MATHEXT_H
#define MATHEXT_H

#include <stdint.h>

uint8_t table_index(int16_t *x, uint8_t n, int16_t vals[]);

int16_t linear_interp(int16_t x, int16_t x0, int16_t x1, int16_t y0, int16_t y1);

int16_t table1d_lookup(int16_t x, uint8_t n, int16_t x_vals[], int16_t y_vals[]);

#endif
