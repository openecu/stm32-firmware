#include <stm32f4xx.h>
#include "ecu.h"
#include "hwconf.h"
#include "actuators.h"
#include "sync.h"
#include "injection.h"
#include "ignition.h"
#include "dispatcher.h"
#include "mathext.h"

extern ecu_t ecu;

int main(void)
{
    hw_init();
    actuators_init();

    for (;;)
    {
        cooling_fan();
        // evap();
        // ego_heater();

        if (sync_is_new_cycle())
        {
            sync_wait_cycle();
            sync_calc_rpm();
            calc_pw();
            calc_ign_adv();
            dispatcher();
        }
    }
}
