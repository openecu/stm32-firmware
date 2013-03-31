#ifndef _SENSORS_H
#define _SENSORS_H
    
#include <stdint.h>

typedef struct sensors_s
{
    uint16_t rpm;
    int16_t ect;
    uint16_t tp;
    uint16_t tp_prev[8];
    uint16_t batv;
} sensors_t;

#endif
