#ifndef _ECU_H
#define _ECU_H

#include <inttypes.h>
#include "config.h"
#include "status.h"
#include "sensors.h"

typedef struct ecu_s
{
    config_t config;
    status_t status;
    sensors_t sensors;
} ecu_t;

extern ecu_t ecu;

#endif
