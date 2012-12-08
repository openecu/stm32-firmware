#ifndef _CONFIG_H
#define _CONFIG_H

#define AUX_COUNT   8

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
    uint8_t cooling_fan_temp;
    uint8_t cooling_fan_temp_hyst;
    uint8_t water_pump_temp;
    uint8_t water_pump_temp_hyst;
    aux_t aux[AUX_COUNT];
} config_t;

#endif
