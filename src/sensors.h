#ifndef _SENSORS_H
#define _SENSORS_H

typedef struct
{
    uint16_t rpm;
    uint16_t map;
    uint16_t maf;
    int8_t ect;
    int8_t iat;
    uint8_t ego;
} sensors_t;

#endif
