#ifndef _ECU_H
#define _ECU_H

#include "config.h"
#include "sensors.h"

typedef struct
{
    config_t config;
    sensors_t sensors;
} ecu_t;

ecu_t ecu;

#endif
