#ifndef _SENSORS_H
#define _SENSORS_H

typedef struct
{
    uint16_t rpm;
    uint16_t tp;
    uint16_t maf;
    int8_t ect;
} sensors_t;

#endif
