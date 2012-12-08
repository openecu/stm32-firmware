#include "actuators.h"
#include "config.h"
#include "sensors.h"

void cooling_fan(void)
{
    if (sensors.ect >= config.fan_temp)
    {

    }
    else if (sensors.ect <= (config.fan_temp - config.fan_temp_hyst))
    {

    }
}
