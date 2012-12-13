#ifndef _ECU_H
#define _ECU_H

#include "config.h"
#include "sensors.h"

#define FLAG1_IGN_SWITCH_ON     0x00000001
#define FLAG1_CRANKING          0x00000002
#define FLAG1_IDLING            0x00000004
#define FLAG1_AFTERSTART        0x00000008
#define FLAG1_WARMUP            0x00000010
#define FLAG1_FUEL_PUMP_ON      0x00000020
#define FLAG1_COOLING_FAN_ON    0x00000040
#define FLAG1_NEW_CYCLE         0x00000080
#define FLAG1_FIRST_CYCLE       0x00000100

typedef struct
{
    config_t config;
    sensors_t sensors;
    uint16_t flow;
    uint16_t load;
    int16_t afr;
    uint16_t pw;
    uint32_t flags1;
} ecu_t;

ecu_t ecu;

#endif
