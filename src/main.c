#include <stm32f4xx.h>
#include "ecu.h"
#include "hwconf.h"
#include "actuators.h"

extern ecu_t ecu;

int main(void)
{
    hw_init();
    actuators_init();

    for (;;)
    {
        if (SWITCH_STATE(IGN_SWITCH))
        {

        }

        cooling_fan();
    }
}
