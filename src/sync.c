#include <stm32f4xx.h>
#include "sync.h"
#include "ecu.h"

extern ecu_t ecu;
sync_t sync;

void sync_init(void)
{

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

void sync_pos_CAPT_ISR(void)
{
    sync.pos++;
}

void sync_ref_CAPT_ISR(void)
{
    // Rising edge
    if (GPIOB->IDR & (1 << 11))
    {
        sync.ref_pos = sync.pos;
    }
    // Falling edge
    else
    {
        sync.ref++;

        if (sync.ref_pos < 85) // 85?
        {
            sync.ref = 0;
        }

        sync.pos = 0;
        sync.ref_dt = TIM1->CNT - sync.ref_time;
        sync.ref_time = TIM1->CNT;
    }
}
