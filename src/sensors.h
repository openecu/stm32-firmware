#ifndef _SENSORS_H
#define _SENSORS_H
    
#include <stdint.h>

typedef struct sensors_s
{
    uint16_t rpm;
    int16_t ect;
} sensors_t;

#endif
