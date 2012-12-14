#include "ign_inj.h"
#include "ecu.h"
#include "mathext.h"

extern ecu_t ecu;
uint16_t crank_cycles;

void calc_pw(void)
{
    uint32_t load, pw;

    // Cranking mode
    if (ecu.mode == MODE_CRANKING)
    {
        load = table1d_lookup(
            ecu.sensors.ect, CRANK_TEMP_SCALE_SIZE,
            (int16_t*)ecu.config.crank_temp_scale, (int16_t*)ecu.config.crank_base_load
        );

        uint16_t crank_cycle_enrich = table1d_lookup(
            (crank_cycles++), CRANK_CYCLE_SCALE_SIZE,
            (int16_t*)ecu.config.crank_cycle_scale, (int16_t*)ecu.config.crank_cycle_enrich
        );
        load = (load * crank_cycle_enrich) / 100;
    }
    else
    {
        uint16_t flow = table1df_lookup(
            ecu.sensors.maf, MAFS_VOLTAGE_SCALE_SIZE,
            0, 1024, (int16_t*)ecu.config.mafs_calibration
        );
        //load = flow * ecu.cycle_time;

        // Idling mode
        if (ecu.mode == MODE_IDLING)
        {

        }
        // Power control mode
        else if (ecu.mode == MODE_POWER)
        {

        }
    }

    // Sequental
    pw = load / ecu.config.inj_mult;

    // Semi-sequential
    if (1)
    {
        pw /= 2;
    }
    // Simultaneous
    else if (2)
    {
        pw /= 4;
    }

    pw += table1d_lookup(
        ecu.sensors.vbat, INJ_VOLTAGE_SCALE_SIZE,
        (int16_t*)ecu.config.inj_voltage_scale, (int16_t*)ecu.config.inj_deadtime
    );

    if (pw > 0xFFFF)
    {
        pw = 0xFFFF;
    }
}

void calc_ign_adv(void)
{

}

void afterstart(void)
{

}

void warmup(void)
{

}

void accel(void)
{

}
