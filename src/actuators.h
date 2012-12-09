#ifndef _ACTUATORS_H
#define _ACTUATORS_H

#include <stm32f4xx.h>
#include <inttypes.h>
#include "hwconf.h"

void actuators_init(void);

static inline void ecu_relay_on(void)
{
    ACTUATOR_ON(ECU_RELAY);
}

static inline void ecu_relay_off(void)
{
    ACTUATOR_OFF(ECU_RELAY);
}

static inline void fuel_pump_relay_on(void)
{
    ACTUATOR_ON(FUEL_PUMP);
}

static inline void fuel_pump_relay_off(void)
{
    ACTUATOR_OFF(FUEL_PUMP);
}

void cooling_fan(void);

#endif
