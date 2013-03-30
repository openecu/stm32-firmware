#include "mathext.h"

int16_t linear_interp(int16_t x, int16_t y0, int16_t y1, int16_t x0, int16_t x1)
{
    return (y0 + (int32_t)((y1 - y0) * (x - x0)) / (x1 - x0));
}

int16_t table1dfix_lookup(int16_t x, uint8_t nx, int16_t xmin, int16_t xmax, int16_t data[])
{
    uint8_t ix;
    int16_t s;

    s = (xmax - xmin) / nx;

    if (x <= xmin)
    {
        return data[0];
    }
    else if (x >= (xmax - s))
    {
        return data[(nx - 1)];
    }

    ix = x / s;

    return linear_interp(x, data[ix], data[ix + 1], (s * ix), (s * (ix + 1)));
}
