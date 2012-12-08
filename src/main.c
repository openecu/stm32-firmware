#include <stm32f4xx.h>
#include "ecu.h"
#include "actuators.h"

extern ecu_t ecu;

int main(void)
{
    actuators_init();

    for (;;)
    {
        //fuel_pump_on();
        cooling_fan();
        aux();
    }
}
