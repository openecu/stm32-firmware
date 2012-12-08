#include "actuators.h"
#include "ecu.h"

extern ecu_t ecu;

void actuators_init(void)
{
    ACTUATORS_OFF();
    AUX_OFF();
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

void water_pump(void)
{
    if (ecu.sensors.ect >= ecu.config.water_pump_temp)
    {
        ACTUATOR_ON(WATER_PUMP);
    }
    else if (ecu.sensors.ect < (ecu.config.water_pump_temp - ecu.config.water_pump_temp_hyst))
    {
        ACTUATOR_OFF(WATER_PUMP);
    }
}

void aux(void)
{
    uint8_t i;
    aux_config_t *aux;

    for (i = 0; i < AUX_COUNT; i++)
    {
        aux = &ecu.config.aux[i];

        if (!(aux->flags & AUX_FLAG_EN))
        {
            AUX_CH_OFF(i);
        }
        else
        {
            if (
                (ecu.sensors.rpm >= aux->rpm_on)
                && (ecu.sensors.ect >= aux->ect_on)
            )
            {
                if ((aux->flags & AUX_FLAG_INV))
                {
                    AUX_CH_OFF(i);
                }
                else
                {
                    AUX_CH_ON(i);
                }
            }
            else if (
                (ecu.sensors.rpm <= aux->rpm_off)
                && (ecu.sensors.ect <= aux->ect_off)
            )
            {
                if ((aux->flags & AUX_FLAG_INV))
                {
                    AUX_CH_ON(i);
                }
                else
                {
                    AUX_CH_OFF(i);
                }
            }
        }
    }
}
