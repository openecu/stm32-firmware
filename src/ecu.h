#ifndef _ECU_H
#define _ECU_H

#include "config.h"
#include "sensors.h"

#define MODE_CRANKING   0x01
#define MODE_IDLING     0x02
#define MODE_POWER      0x03

typedef struct
{
    config_t config;
    sensors_t sensors;
    uint16_t airflow;
    uint16_t load;
    uint16_t pw;
    uint8_t mode;
} ecu_t;

ecu_t ecu;

#endif
