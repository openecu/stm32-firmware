#include "actuators.h"

extern ecu_t ecu;

void actuators_init(void)
{
    ACTUATORS_GPIO->ODR &= ~ACTUATORS_PORT_MASK;
    ACTUATORS_GPIO->ODR |= ACTUATORS_PORT_MASK;
}

void cooling_fan(void)
{
    if (ecu.sensors.ect >= ecu.config.fan_temp)
    {
        ACTUATOR_ON(COOLING_FAN);
    }
    else if (ecu.sensors.ect <= (ecu.config.fan_temp - ecu.config.fan_temp_hyst))
    {
        ACTUATOR_OFF(COOLING_FAN);
    }
}

void aux(void)
{
    uint8_t i;
    aux_t *aux;

    for (i = 0; i < AUX_COUNT; i++)
    {
        aux = &ecu.config.aux[i];

        if (!aux->en)
        {
            continue;
        }

        if (
            (ecu.sensors.rpm >= aux->rpm_on)
            && (ecu.sensors.ect >= aux->ect_on)
        )
        {
            if (aux->inv)
            {
                AUX_OFF(i);
            }
            else
            {
                AUX_ON(i);
            }
        }
        else if (
            (ecu.sensors.rpm <= aux->rpm_off)
            && (ecu.sensors.ect <= aux->ect_off)
        )
        {
            if (aux->inv)
            {
                AUX_ON(i);
            }
            else
            {
                AUX_OFF(i);
            }
        }
    }
}
