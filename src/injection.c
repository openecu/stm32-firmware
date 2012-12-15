#include "injection.h"
#include "ecu.h"
#include "mathext.h"

extern ecu_t ecu;
uint16_t crank_cycles;

void calc_pw(void)
{
    uint32_t load, fuel, pw;
    uint16_t air_flow, afr;

    // Cranking mode
    if (ecu.mode == MODE_CRANKING)
    {
        load = table1d_lookup(ecu.sensors.ect, CRANK_TEMP_SCALE_SIZE,
            (int16_t*)ecu.config.crank_temp_scale, (int16_t*)ecu.config.crank_load);
        uint16_t crank_cycle_enrich = table1d_lookup((crank_cycles++), CRANK_CYCLE_SCALE_SIZE, 
            (int16_t*)ecu.config.crank_cycle_scale, (int16_t*)ecu.config.crank_cycle_enrich);

        fuel = (load * 100) / ecu.config.crank_afr;
        fuel = (fuel * crank_cycle_enrich) / 100;
    }
    else
    {
        air_flow = table1df_lookup(ecu.sensors.maf, MAFS_VOLTAGE_SCALE_SIZE,
            0, 1024, (int16_t*)ecu.config.mafs_calibration);
        load = air_flow * ecu.cycle_time;

        // Idling mode
        if (ecu.mode == MODE_IDLING)
        {
            afr = ecu.config.idle_afr;
        }
        // Power control mode
        else if (ecu.mode == MODE_POWER)
        {
            afr = table2d_lookup(ecu.sensors.rpm, load, TABLE_AFR_RPM_SCALE_SIZE, TABLE_AFR_LOAD_SCALE_SIZE,
                (int16_t*)ecu.config.table_afr_rpm_scale, (int16_t*)ecu.config.table_afr_load_scale, (int16_t*)ecu.config.table_afr);
        }

        uint16_t warmup_enrich = table1d_lookup(ecu.sensors.ect, WARMUP_TEMP_SCALE_SIZE, 
            (int16_t*)ecu.config.warmup_temp_scale, (int16_t*)ecu.config.warmup_enrich);

        fuel = (load * 100) / afr;
        fuel = (fuel * warmup_enrich) / 100;
    }

    pw = fuel / ecu.config.inj_mult;

    uint16_t deadtime = table1d_lookup(
        ecu.sensors.vbat, INJ_VOLTAGE_SCALE_SIZE,
        (int16_t*)ecu.config.inj_voltage_scale, (int16_t*)ecu.config.inj_deadtime
    );
    pw += deadtime;

    restrict_i32((int32_t*)&pw, 0, 0x8000); // max. 32 ms?
}
