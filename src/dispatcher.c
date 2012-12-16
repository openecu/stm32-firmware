#include "dispatcher.h"
#include "ecu.h"

void dispatcher(void)
{
    if (ecu.mode == MODE_CRANKING)
    {
        if (ecu.sensors.rpm >= ecu.config.crank_rpm_thres)
        {
            ecu.mode = MODE_IDLING;
        }
    }
    else if (ecu.mode == MODE_IDLING)
    {
        if (ecu.sensors.tp > 0)
        {
            ecu.mode = MODE_POWER;
        }
    }
    else if (ecu.mode == MODE_POWER)
    {
        if (ecu.sensors.tp == 0)
        {
            ecu.mode = MODE_IDLING;
        }
    }
}
