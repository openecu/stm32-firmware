#include "dispatcher.h"
#include "ecu.h"

void dispatcher(void)
{
    if (ecu.mode == MODE_CRANKING)
    {
    }
    else if (ecu.mode == MODE_IDLING)
    {
        if (ecu.sensors.tp > 0) // tps threshold vs rpm?
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
