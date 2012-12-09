#include "actuators.h"
#include "ecu.h"

extern ecu_t ecu;

void actuators_init(void)
{
    ACTUATORS_OFF();
}

void cooling_fan(void)
{
    if (ecu.sensors.ect >= ecu.config.cooling_fan_temp)
    {
        ACTUATOR_ON(COOLING_FAN);
    }
    else if (ecu.sensors.ect < (ecu.config.cooling_fan_temp - ecu.config.cooling_fan_temp_hyst))
    {
        ACTUATOR_OFF(COOLING_FAN);
    }
}
