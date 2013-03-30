#ifndef _CONFIG_H
#define _CONFIG_H
    
#include <stdint.h>

typedef struct config_s
{
    int16_t fan_ect_thres;
    uint8_t fan_ect_hyst;

    uint16_t vvt_rpm_thres;
    uint16_t vvt_rpm_hyst;
} config_t;

#endif
