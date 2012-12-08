#include "mathext.h"

uint16_t linear_interp(uint16_t x, uint16_t y0, uint16_t y1, uint16_t x0, uint16_t x1)
{
    return (y0 + (int32_t)((int16_t)(y1 - y0) * (int16_t)(x - x0)) / (int16_t)(x1 - x0));
}

uint16_t bilinear_interp(
    uint16_t x, uint16_t y,
    uint16_t z0, uint16_t z1, uint16_t z2, uint16_t z3,
    uint16_t x0, uint16_t x1, uint16_t y0, uint16_t y1
)
{
    uint16_t z12, z03;

    z12 = z1 + (int32_t)((int16_t)(z2 - z1) * (int16_t)(x - x0)) / (int16_t)(x1 - x0);
    z03 = z0 + (int32_t)((int16_t)(z3 - z0) * (int16_t)(x - x0)) / (int16_t)(x1 - x0);

    return (z03 + (uint32_t)((uint16_t)(z12 - z03) * (int16_t)(y - y0)) / (int16_t)(y1 - y0));
}

uint8_t table_index(uint16_t *value, uint16_t values[], uint8_t size)
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

        return last_index;
    }
    else
    {
        for (index = 1; index < size; index++)
        {
            if ((*value) < values[index])
            {
                break;
            }
        }
    }

    return (index - 1);
}

uint16_t table_lookup(
    uint16_t x, uint16_t y,
    uint8_t nx, uint8_t ny,
    uint16_t vx[], uint16_t vy[],
    uint16_t **data
)
{
    uint8_t ix, iy;
    uint16_t _x, _y;

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
