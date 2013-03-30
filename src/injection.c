#include "injection.h"
#include "ecu.h"
#include "mathext.h"

#define INJ_FLAGS1_ACCEL_ENRICH_DELAY   0x00000001

extern ecu_t ecu;

uint32_t flags1;
uint16_t acce, acce_delay;

void warmup_enrich(void)
{

}

/**
 * Обогащение по скорости открытия дроссельной заслонки.
 */
void accel_enrich(uint32_t dtime)
{
    int16_t dtp;
    uint32_t enrich_decay;

    dtp = ecu.sensors.tp - ecu.sensors.tp_prev[0];

    if (dtp >= ecu.config.accel_enrich_delta_thres)
    {
        if (!(ecu.status.flags3 & STATUS_FLAGS3_ACCEL_ENRICH))
        {
            uint16_t acced, accer, accee;

            acced = table1dfix_lookup(dtp, 16, 0, 128, (int16_t*)ecu.config.accel_delta_enrich);
            accer = table1dfix_lookup(ecu.sensors.rpm, 16, 0, 8000, (int16_t*)ecu.config.accel_rpm_enrich);
            accee = table1dfix_lookup(ecu.sensors.ect, 16, -40, 120, (int16_t*)ecu.config.accel_ect_enrich);

            acce = (uint16_t)((uint16_t)(acced * accer / 1000) * accee / 1000);
        }

        acce_delay = ecu.config.accel_enrich_delay;
        flags1 |= INJ_FLAGS1_ACCEL_ENRICH_DELAY;
        ecu.status.flags3 |= STATUS_FLAGS3_ACCEL_ENRICH;
    }
    else
    {
        if ((flags1 & INJ_FLAGS1_ACCEL_ENRICH_DELAY))
        {
            if (dtime >= acce_delay)
            {
                flags1 &= ~INJ_FLAGS1_ACCEL_ENRICH_DELAY;
            }
            else
            {
                acce_delay -= dtime;
            }
        }
    }

    if (
        (ecu.status.flags3 & STATUS_FLAGS3_ACCEL_ENRICH)
        && !(flags1 & INJ_FLAGS1_ACCEL_ENRICH_DELAY)
    )
    {
        enrich_decay = ecu.config.accel_enrich_decay * dtime;

        if (enrich_decay > 0xFFFF)
        {
            enrich_decay = 0xFFFF;
        }

        if (enrich_decay < acce)
        {
            acce -= (int16_t)enrich_decay;
        }
        else
        {
            acce = 0;
            ecu.status.flags1 &= ~STATUS_FLAGS3_ACCEL_ENRICH;
        }
    }

}

void inj_afr(void)
{

}

void inj_trim(void)
{

}

void inj_calc_pw(void)
{

}

void inj_timing(void)
{

}