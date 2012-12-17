#ifndef _ECU_H
#define _ECU_H

#include "config.h"
#include "sensors.h"

#define MODE_CRANK  0x01
#define MODE_IDLE   0x02
#define MODE_POWER  0x03

#define STATUS1_AFTERSTART_ENRICH   0x00000001
#define STATUS1_WARMUP_ENRICH       0x00000002

typedef struct
{
    config_t config;
    sensors_t sensors;
    uint8_t mode;
    uint32_t status1;
    uint32_t cycle_time;
} ecu_t;

ecu_t ecu;

#endif
