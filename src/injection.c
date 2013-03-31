#include "injection.h"
#include "ecu.h"
#include "mathext.h"

#define INJ_FLAGS1_ACCEL_ENRICH_DELAY   0x00000001

extern ecu_t ecu;

uint32_t flags1;
uint16_t acce, acce_delay;

/**
 * Обогащение при прогреве.
 */
void warmup_enrich(void)
{
    uint16_t wuee, asee;

    wuee = table1dfix_lookup(ecu.sensors.ect, 16, -40, 120, 
        (int16_t*)ecu.config.warmup_enrich);

    ecu.status.wue = wuee;
}

/**
 * Обогащение по скорости открытия дроссельной заслонки.
 */
void accel_enrich(uint32_t dtime)
{
    int16_t dtp;

    dtp = ecu.sensors.tp - ecu.sensors.tp_prev[0];

    // Если скорость открытия превысила порог, то активируем обогащение
    if (dtp >= ecu.config.accel_enrich_delta_thres)
    {
        uint16_t _acce, acced, accer, accee;

        acced = table1dfix_lookup(dtp, 16, 0, 256, 
            (int16_t*)ecu.config.accel_delta_enrich);
        accer = table1dfix_lookup(ecu.sensors.rpm, 16, 0, 8000, 
            (int16_t*)ecu.config.accel_rpm_enrich);
        accee = table1dfix_lookup(ecu.sensors.ect, 16, -40, 120, 
            (int16_t*)ecu.config.accel_ect_enrich);

        // Суммарное обогащение вычисляется на основе обогащения в зависимости 
        // от скорости открытия заслонки и поправок от оборотов двигателя и
        // температуры ОЖ
        _acce = (((acced * accer) / 1000) * accee) / 1000;

        if (_acce > acce)
        {
            acce = _acce;
        }

        // Инициализация обогащения
        acce_delay = ecu.config.accel_enrich_delay;
        flags1 |= INJ_FLAGS1_ACCEL_ENRICH_DELAY;
        ecu.status.flags3 |= STATUS_FLAGS3_ACCEL_ENRICH;
    }
    // Если скорость открытия не превысила порог и обогащение активно, то
    // выполняем задержку перед началом уменьшения обогащения
    else if ((flags1 & INJ_FLAGS1_ACCEL_ENRICH_DELAY))
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

    // Если обогащение активно и задержка выполнена, то постепенно уменьшаем 
    // обогащение до нуля с заданной скоростью
    if (
        (ecu.status.flags3 & STATUS_FLAGS3_ACCEL_ENRICH)
        && !(flags1 & INJ_FLAGS1_ACCEL_ENRICH_DELAY)
    )
    {
        uint32_t enrich_decay;

        enrich_decay = ecu.config.accel_enrich_decay * dtime;

        if ((dtime > 0xFFFF) || (enrich_decay > 0xFFFF))
        {
            enrich_decay = 0xFFFF;
        }

        if (enrich_decay < acce)
        {
            acce -= enrich_decay;
        }
        else
        {
            acce = 0;
            ecu.status.flags1 &= ~STATUS_FLAGS3_ACCEL_ENRICH;
        }
    }

    ecu.status.acce = acce;
}

void inj_afr(void)
{

}

/**
 * Корректировка длительности впрыска.
 */
void inj_trim(void)
{

}

/**
 * Рассчёт длительности впрыска.
 */
void inj_calc_pw(void)
{
    uint16_t load, pw, deadtime;

    deadtime = table1dfix_lookup(ecu.sensors.batv, 16, 0, 1024, 
        (int16_t*)ecu.config.inj_deadtime);

    // Если режим пуска, то время впрыска рассчитывается на основе теоритического 
    // наполнения с корректировкой по температуре ОЖ и количеству оборотов прокрутки.
    if ((ecu.status.flags1 & STATUS_FLAGS1_CRANK))
    {
        uint16_t cre;

        load = table1dfix_lookup(ecu.sensors.ect, 16, -40, 120,
            (int16_t*)ecu.config.crank_load);
        cre = 0;

        pw = ((load * (1000 + cre)) * ecu.config.inj_mult / 1000) / ecu.config.crank_afr;
        ecu.status.injpw = pw + deadtime;

        return;
    }


}

void inj_timing(void)
{

}
