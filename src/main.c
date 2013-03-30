#include <stm32f4xx.h>
#include "ecu.h"
#include "calc.h"
#include "injection.h"
#include "ignition.h"
#include "actuators.h"
#include "comm.h"

volatile ecu_t ecu;

int main(void)
{
    __disable_irq();

    actuators_init();

    __enable_irq();

    for (;;)
    {
        if ((ecu.status.flags1 & STATUS_FLAGS1_STROKE))
        {
            ecu.status.flags1 &= ~STATUS_FLAGS1_STROKE;
            calc_rpm();
        }

        calc_load();

        /*if ((ecu.status1.times1 & STATUS_TIMES1_))
        {
            accel_enrich();
            idle();
        }*/

        warmup_enrich();
        inj_afr();
        inj_trim();
        inj_timing();
        inj_calc_pw();

        ign_dwell();
        ign_timing();

        cooling_fan();
        vvt();

        comm();
    }

    return 0;
}
