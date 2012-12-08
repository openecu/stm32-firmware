#ifndef _CONFIG_H
#define _CONFIG_H

#include "actuators.h"
#include "pid.h"

typedef struct
{
    uint8_t en: 1;
    uint8_t inv: 1;
    uint16_t rpm_on;
    uint16_t rpm_off;
    int8_t ect_on;
    int8_t ect_off;
} aux_t;

typedef struct
{
    uint16_t idle_rpm[16];
    int8_t idle_temp_scale[16];
    pid_config_t idle_pid_config;
    int8_t cooling_fan_temp;
    uint8_t cooling_fan_temp_hyst;
    int8_t water_pump_temp;
    uint8_t water_pump_temp_hyst;
    aux_t aux[AUX_COUNT];
} config_t;

#endif
