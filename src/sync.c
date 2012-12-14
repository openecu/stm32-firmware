#include <stm32f4xx.h>
#include "sync.h"
#include "ecu.h"

extern ecu_t ecu;
sync_t sync;

void sync_init(void)
{
    sync.new_cycle = 0;
    sync.pos = 0;
    sync.ref = 0;
    sync.ref_time = 0;
    sync.ref_dt = 0;
    sync.pos_time = 0;
    sync.pos_dt = 0;
    sync.ref_pos = 0;
}

uint8_t sync_is_new_cycle(void)
{
    return sync.new_cycle;
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

void tim2_ic0_irq_isr(void)
{
    sync.pos++;
}

void tim2_ic1_irq_isr(void)
{
    sync.ref++;

    // Rising edge
    if (1)
    {
        sync.ref_pos = sync.pos;

        if (sync.ref_pos < 85) // 85?
        {
            sync.ref = 0;
        }
    }
    // Falling edge
    else
    {
        sync.ref_dt = TIM1->CNT - sync.ref_time;
        sync.ref_time = TIM1->CNT;
    }
}
