#include "math_ext.h"

int16_t linear_interp(int16_t x, int16_t x0, int16_t x1, int16_t y0, int16_t y1)
{
    return (y0 + (int32_t)((y1 - y0) * (x - x0)) / (x1 - x0));
}

int16_t bilinear_interp(int16_t x, int16_t y,
    int16_t z0, int16_t z1, int16_t z2, int16_t z3,
    int16_t x0, int16_t x1, int16_t y0, int16_t y1)
{
    int16_t z12, z03;

    z12 = z1 + (int32_t)((z2 - z1) * (x - x0)) / (x1 - x0);
    z03 = z0 + (int32_t)((z3 - z0) * (x - x0)) / (x1 - x0);

    return (z03 + (uint32_t)((z12 - z03) * (y - y0)) / (y1 - y0));
}

uint8_t table_index(int16_t *x, uint8_t n, int16_t vals[])
{
    if (*x <= vals[0])
    {
        *x = vals[0];

        return 0;
    }

    n--;

    if (*x >= vals[n])
    {
        *x = vals[n];

        return n;
    }
    else
    {
        uint8_t i_x;

        for (i_x = 1; i_x <= n; i_x++)
        {
            if (*x < vals[i_x])
            {
                return (i_x - 1);
            }
        }
    }
}

int16_t table1d_lookup(int16_t x, uint8_t n, int16_t x_vals[], int16_t y_vals[])
{
    uint8_t i_x;
    int16_t y;

    i_x = table_index(&x, n, x_vals);
    y = linear_interp(x, x_vals[i_x], x_vals[i_x + 1], y_vals[i_x], y_vals[i_x + 1]);

    return y;
}

int16_t table2d_lookup(int16_t x, int16_t y, uint8_t nx, uint8_t ny,
    int16_t vx[], int16_t vy[], int16_t data[])
{
    uint8_t ix, iy;
    int16_t z;

    ix = table_index(&x, nx, vx);
    iy = table_index(&y, ny, vy);

    z = bilinear_interp(x, y,
        data[iy][ix], data[iy + 1][ix], data[iy + 1][ix + 1], data[iy][ix + 1],
        vx[ix], vx[ix + 1], vy[iy], vy[iy + 1]);

    return z;
}
