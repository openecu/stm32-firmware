#ifndef _SENSORS_H
#define _SENSORS_H

typedef struct
{
    uint16_t rpm;
    int8_t ect;
} sensors_t;

extern sensors_t sensors;

#endif
