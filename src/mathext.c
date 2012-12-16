#include "mathext.h"

int32_t restrict_value(int32_t value, int32_t min, int32_t max)
{
    if (value < min)
    {
        return min;
    }
    else if (value > max)
    {
        return max;
    }

    return value;
}

int16_t linear_interp(int16_t x, int16_t y0, int16_t y1, int16_t x0, int16_t x1)
{
    return (y0 + (int32_t)((y1 - y0) * (x - x0)) / (x1 - x0));
}

int16_t bilinear_interp(
    int16_t x, int16_t y,
    int16_t z0, int16_t z1, int16_t z2, int16_t z3,
    int16_t x0, int16_t x1, int16_t y0, int16_t y1
)
{
    int16_t z12, z03;

    z12 = z1 + (int32_t)((z2 - z1) * (x - x0)) / (x1 - x0);
    z03 = z0 + (int32_t)((z3 - z0) * (x - x0)) / (x1 - x0);

    return (z03 + (uint32_t)((z12 - z03) * (y - y0)) / (y1 - y0));
}

uint8_t table_index(int16_t *value, int16_t values[], uint8_t size)
{
    uint8_t index, last_index;

    last_index = size - 1;

    if ((*value) <= values[0])
    {
        (*value) = values[0];

        return 0;
    }
    else if ((*value) >= values[last_index])
    {
        (*value) = values[last_index];

        return (last_index - 1);
    }

    for (index = 1; index < size; index++)
    {
        if ((*value) < values[index])
        {
            break;
        }
    }

    return (index - 1);
}

int16_t table1d_lookup(int16_t x, uint8_t nx, int16_t vx[], int16_t data[])
{
    uint8_t ix;
    int16_t _x;

    _x = x;
    ix = table_index(&_x, vx, nx);

    return linear_interp(_x, data[ix], data[ix + 1], vx[ix], vx[ix + 1]);
}

/**
 * 1D table lookup with fixed scale
 */
int16_t table1df_lookup(int16_t x, uint8_t nx, int16_t xmin, int16_t xmax, int16_t data[])
{
    uint8_t ix;
    int16_t s;

    if (x <= xmin)
    {
        return data[0];
    }
    else if (x >= xmax)
    {
        return data[(nx - 1)];
    }

    s = (xmax - xmin) / nx;
    ix = x / s;

    return linear_interp(x, data[ix], data[ix + 1], (s * ix), (s * (ix + 1)));
}

int16_t table2d_lookup(
    int16_t x, int16_t y,
    uint8_t nx, uint8_t ny,
    int16_t vx[], int16_t vy[],
    int16_t *data[]
)
{
    uint8_t ix, iy;
    int16_t _x, _y;

    _x = x;
    _y = y;
    ix = table_index(&_x, vx, nx);
    iy = table_index(&_y, vy, ny);

    return bilinear_interp(
        _x, _y,
        data[iy][ix], data[iy + 1][ix], data[iy + 1][ix + 1], data[iy][ix + 1],
        vx[ix], vx[ix + 1], vy[iy], vy[iy + 1]
    );
}
