#include <stm32f4xx.h>
#include "ecu.h"
#include "actuators.h"

extern ecu_t ecu;

int main(void)
{
    actuators_init();

    for (;;)
    {
        cooling_fan();
        water_pump();
        aux();
    }
}
