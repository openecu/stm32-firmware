#ifndef _SENSORS_H
#define _SENSORS_H

typedef struct
{
    uint16_t rpm;
    uint16_t tp;
    int16_t dtp;
    uint16_t maf;
    int16_t ect;
    uint16_t vbat;
} sensors_t;

#endif
