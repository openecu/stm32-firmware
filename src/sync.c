#include <stm32f4xx.h>
#include "sync.h"
#include "ecu.h"
#include "hwconf.h"

extern ecu_t ecu;
sync_t sync;

void sync_init(void)
{

}

uint8_t sync_is_new_cycle(void)
{
    return sync.new_cycle;
}

uint8_t sync_is_first_cycle(void)
{
    return sync.first_cycle;
}

void sync_wait_cycle(void)
{
    sync.new_cycle = 0;
}

void sync_calc_rpm(void)
{
    if (!sync.ref_dt)
    {
        ecu.sensors.rpm = 0;
    }
    else
    {
        ecu.sensors.rpm = 9375000L / sync.ref_dt;
    }
}

void sync_pos_trig(void)
{
    sync.pos++;
}

void sync_ref_trig(void)
{
    // Rising edge
    if (SYNC_REF_STATE())
    {
        sync.rising_edge = 1;
        sync.ref_pos = sync.pos;
    }
    // Falling edge
    else
    {
        sync.ref++;

        if (sync.rising_edge && ((sync.pos - sync.ref_pos) > 2))
        {
            sync.ref = 0;
            sync.synced = 1;
        }

        sync.new_cycle = 1;
        sync.pos = 0;
        sync.ref_dt = TIM2->CNT - sync.ref_time;
        sync.ref_time = TIM2->CNT;
    }
}
