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

void aux(void)
{
    uint8_t i;
    aux_t *aux;

    for (i = 0; i < AUX_COUNT; i++)
    {
        aux = &config.aux[i];

        if (
            (sensors.rpm >= aux->rpm_on)
            && (sensors.ect >= aux->ect_on)
        )
        {

        }
        else if (
            sensors.rpm <= aux->rpm_off
            && (sensors.ect <= aux->ect_off)
        )
        {

        }
    }
}
