#include <stm32f4xx.h>
#include "ecu.h"
#include "hwconf.h"
#include "actuators.h"
#include "mathext.h"

extern ecu_t ecu;

int main(void)
{
    hw_init();
    actuators_init();

    uint16_t load, pw;
    uint16_t crank_cycles;

    ecu.flags1 |= FLAG1_FIRST_CYCLE;

    for (;;)
    {
        // вычисления на каждый цикл
        if (ecu.flags1 & FLAG1_NEW_CYCLE)
        {
            ecu.flags1 &= ~FLAG1_NEW_CYCLE;

            if (ecu.flags1 & FLAG1_FIRST_CYCLE)
            {
                ecu.flags1 &= ~(FLAG1_FIRST_CYCLE | FLAG1_IDLING);
                ecu.flags1 |= FLAG1_CRANKING;
                crank_cycles = 0;

                // на первом цикле данные ещё не готовы?
                continue;
            }

            if (ecu.flags1 & FLAG1_CRANKING)
            {
                // базовый цикловый расход воздуха
                load = table1d_lookup(
                    ecu.sensors.ect, CRANK_TEMP_SCALE_SIZE,
                    ecu.config.crank_temp_scale, ecu.config.crank_base_load
                );
                // базовая длительность впрыска
                pw = (load * 100) / (ecu.config.inj_mult * ecu.config.idle_afr);

                // поправка по числу рабочих циклов
                crank_cycles++;
                uint16_t crank_cycle_enrich = table1d_lookup(
                    crank_cycles, CRANK_CYCLE_SCALE_SIZE,
                    ecu.config.crank_cycle_scale, ecu.config.crank_cycle_enrich
                );
                pw = (pw * crank_cycle_enrich) / 100;

                // условие выхода с пуска
                if (ecu.sensors.rpm >= ecu.config.crank_thres_rpm)
                {
                    ecu.flags1 &= ~FLAG1_CRANKING;
                    ecu.flags1 |= (FLAG1_IDLING | FLAG1_AFTERSTART);
                }
            }
            else if ((ecu.flags1 & FLAG1_IDLING))
            {
                if (ecu.flags1 & FLAG1_AFTERSTART)
                {

                }
                else if (ecu.flags1 & FLAG1_WARMUP)
                {

                }
            }

            // поправка на лаг форсунки
            uint16_t deadtime = table1d_lookup(
                ecu.sensors.vbat, INJ_VOLTAGE_SCALE_SIZE,
                ecu.config.inj_voltage_scale, ecu.config.inj_deadtime
            );
            pw += deadtime;
        }

        cooling_fan();
    }
}
