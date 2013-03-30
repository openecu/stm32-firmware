#include <stm32f4xx.h>
#include "ecu.h"
#include "calc.h"
#include "injection.h"
#include "ignition.h"
#include "actuators.h"
#include "comm.h"

volatile ecu_t ecu;
uint32_t execount;

int main(void)
{
    __disable_irq();

    actuators_init();

    __enable_irq();

    uint32_t mtime, mtime1, dtime1;
    execount = 0;

    for (;;)
    {
        /* События на каждый новый такт двигателя */
        if ((ecu.status.flags1 & STATUS_FLAGS1_STROKE))
        {
            ecu.status.flags1 &= ~STATUS_FLAGS1_STROKE;
            calc_rpm();
        }

        /* События с заданным интервалом */
        mtime = ecu.status.mtime;
        dtime1 = (mtime1 > mtime) ? (mtime1 - mtime) : (mtime - mtime1);

        // Каждые 10 мс
        if (dtime1 >= 10)
        {
            mtime1 = ecu.status.mtime;

            fuel_pump(dtime1);
            //accel_enrich(dtime1);
            //idle(dtime1);
        }

        /* 
            События на каждую итерацию.
            Часть событий распределена по итерациям, для более точного выполнения
            событий с заданным интервалом.
        */
        calc_load();

        if (execount == 1)
        {
            warmup_enrich();
            inj_afr();
            inj_trim();
            inj_timing();
            inj_calc_pw();
        } 
        else if (execount == 2)
        {
            ign_dwell();
            ign_timing();
        }
        else if (execount == 3)
        {
            cooling_fan();
            vvt();
        }
        else
        {
            comm();
            execount = 0;
        }

        execount++;
    }

    return 0;
}
