#ifndef _CONFIG_H
#define _CONFIG_H
    
#include <stdint.h>

typedef struct config_s
{
    uint16_t fuel_pump_prime_time;

    int16_t fan_ect_thres;
    uint8_t fan_ect_hyst;

    uint16_t vvt_rpm_thres;
    uint16_t vvt_rpm_hyst;
} config_t;

#endif
