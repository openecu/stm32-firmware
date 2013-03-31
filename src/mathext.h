#ifndef _MATHEXT_H
#define _MATHEXT_H

#include <stdint.h>

int16_t linear_interp(int16_t x, int16_t y0, int16_t y1, int16_t x0, int16_t x1);

int16_t table1dfix_lookup(int16_t x, uint8_t nx, int16_t xmin, int16_t xmax, int16_t data[]);

#endif
