#include "cmsis/stm32f4xx.h"
#include "sync.h"
#include "config.h"
#include "injection.h"
#include "status.h"

/**
 * Initialize synchronization
 */
void sync_init(void)
{
    // Reference input
    // PB8
    GPIOB->MODER |= GPIO_MODER_MODER8_1;
    GPIOB->PUPDR &= ~GPIO_PUPDR_PUPDR8;
     // TIM10_CH1
    GPIOB->AFR[1] |= (0x03 << 0);

    // Position input
    // PE5
    GPIOE->MODER |= GPIO_MODER_MODER5_1;
    GPIOE->PUPDR &= ~GPIO_PUPDR_PUPDR5;
    // TIM9_CH1
    GPIOE->AFR[0] |= (0x03 << 20);

    // Reference timer
    TIM10->PSC = 99;
    TIM10->CCMR1 |= TIM_CCMR1_CC1S_0;
    TIM10->CCER |= TIM_CCER_CC1E;
    TIM10->DIER |= (TIM_DIER_CC1IE | TIM_DIER_UIE);
    TIM10->CR1 |= (TIM_CR1_URS | TIM_CR1_CEN);

    // Position timer
    TIM9->PSC   = 0;
    TIM9->ARR   = 179;
    TIM9->CCR2  = 0;
    TIM9->SMCR  |= (TIM_SMCR_TS_2 | (TIM_SMCR_SMS_2 | TIM_SMCR_SMS_1 | TIM_SMCR_SMS_0));
    TIM9->DIER  |= (TIM_DIER_CC2IE | TIM_DIER_UIE);
    TIM9->CCMR1 |= TIM_CCMR1_CC1S_0;
    TIM9->CCER  |= (TIM_CCER_CC1NP | TIM_CCER_CC1P);
    TIM9->CR1   |= (TIM_CR1_URS | TIM_CR1_CEN);

    // Events timer
    TIM1->PSC = 99;
    TIM1->CR1 |= TIM_CR1_CEN;

    // Interrupts
    NVIC_SetPriority(TIM1_BRK_TIM9_IRQn, 1);
    NVIC_EnableIRQ(TIM1_BRK_TIM9_IRQn);

    NVIC_SetPriority(TIM1_UP_TIM10_IRQn, 2);
    NVIC_EnableIRQ(TIM1_UP_TIM10_IRQn);

    NVIC_SetPriority(TIM1_CC_IRQn, 3);
    NVIC_EnableIRQ(TIM1_CC_IRQn);
}

/**
 * Calculate and update RPM
 */
void sync_freq_calc(void)
{
    uint8_t i;
    uint32_t stroke_time, freq_sum;

    stroke_time = status.sync.stroke_time;
    status.sync.inst_freq = (stroke_time > 0) ? (30000000L / stroke_time) : 0;

    status.sync.freq_buf[status.sync.freq_head] = status.sync.inst_freq;

    if ((++status.sync.freq_head) == SYNC_STROKE_COUNT)
    {
        status.sync.freq_head = 0;
    }

    freq_sum = 0;

    for (i = 0; i < SYNC_STROKE_COUNT; i++)
    {
        freq_sum += status.sync.freq_buf[i];
    }

    status.sync.filt_freq = freq_sum / SYNC_STROKE_COUNT;
    status.rpm = status.sync.inst_freq; // or filt_freq ?
}

/**
 * Initialize event queue
 */
void event_queue_init(sync_event_t events[], uint8_t n, uint16_t timing)
{
    uint8_t i, k;
    sync_event_t *event;

    for (i = 0; i < n; i++)
    {
        event = &events[i];
        k = (i < (n - 1)) ? (i + 1) : 0;
        event->next = &events[k];
        event->offset = i;
        event_update(event, timing, 720);
    }
}

/**
 * Prepare next event in queue
 */
void event_queue_next(sync_event_t *event, uint16_t timing, uint16_t step)
{
    event = event->next;

    if (event->timing != timing)
    {
        event_update(event, timing, step);
    }
}

/**
 * Update event
 */
void event_update(sync_event_t *event, uint16_t target, uint16_t step)
{
    int16_t current;
    uint8_t q, r;

    current = event->timing;

    if (target > current)
    {
        current += step;

        if (current > target)
        {
            current = target;
        }
    }
    else if (target < current)
    {
        current -= step;

        if (current < target)
        {
            current = target;
        }
    }

    q = current / 180;
    r = current % 180;

    event->timing   = current;
    event->stroke   = q + event->offset;
    event->cogs     = r / 6;
    event->ang_mod  = r % 6;

    if (event->stroke >= 4)
    {
        event->stroke -= 4;
    }
}

/**
 * Stroke ISR
 *
 * @todo Add error process
 */
void TIM1_UP_TIM10_IRQHandler(void)
{
    if ((TIM10->SR & TIM_SR_CC1IF))
    {
        static uint16_t prev_ccr;

        TIM10->SR = ~TIM_SR_CC1IF;

        // Synced
        if (TESTBIT(status.sync.flags1, SYNC_FLAGS1_SYNCED))
        {
            // Rising edge
            if ((GPIOB->IDR & GPIO_IDR_IDR_8))
            {
                //TIM10->CNT = 0;
                status.sync.cogs = 0;

                /*if (status.sync.ref == 3)
                {
                    status.sync.ref = 0;
                    TIM10->CCER |= TIM_CCER_CC1P;
                }
                else
                {
                    status.sync.ref++;
                }*/

                SETBIT(status.flags1, FLAGS1_RUN);
                SETBIT(status.flags1, FLAGS1_STROKE);

                status.sync.prev_stroke_time = status.sync.stroke_time;
                status.sync.stroke_time = (TIM10->CCR1 < prev_ccr) 
                    ? ((uint32_t)status.sync.stroke_ovf << 16) - (prev_ccr - TIM10->CCR1)
                    : ((uint32_t)status.sync.stroke_ovf << 16) + (TIM10->CCR1 - prev_ccr);

                prev_ccr = TIM10->CCR1;
                status.sync.stroke_ovf = 0;

                TIM10->SR = ~TIM_SR_UIF;
            }
            // Falling edge
            /*else
            {
                if (status.sync.ref == 0)
                {
                    uint8_t pos;

                    pos = TIM9->CNT;
                    TIM10->CCER &= ~TIM_CCER_CC1P;

                    if ((pos < 15) || (pos > 17))
                    {
                        CLEARBIT(status.sync.flags1, SYNC_FLAGS1_SYNCED);
                    }
                }
            }*/
        }
        // Not synced
        else
        {
            // Rising edge
            if ((GPIOB->IDR & GPIO_IDR_IDR_8))
            {
                TIM9->CNT = 0;
                TIM10->CNT = 0;
                TIM10->CCER |= TIM_CCER_CC1P;
                status.sync.cogs = 0;
                status.sync.stroke_time = 0;
                status.sync.stroke_ovf = 0;
            }
            // Falling edge
            else
            {
                uint8_t pos, i;
                sync_event_t *event;

                TIM10->CCER &= ~TIM_CCER_CC1P;
                pos = TIM9->CNT;

                // Cyl. 1
                if ((pos >= 15) && (pos <= 17))
                {
                    status.sync.stroke = 0;
                }
                // Cyl. 2
                else if ((pos >= 11) && (pos <= 13))
                {
                    status.sync.stroke = 1;
                }
                // Cyl. 3
                else if ((pos >= 7) && (pos <= 9))
                {
                    status.sync.stroke = 2;
                }
                // Cyl. 4
                else if ((pos >= 3) && (pos <= 5))
                {
                    status.sync.stroke = 3;
                }

                for (i = 0; i < INJ_COUNT; i++)
                {
                    event = &status.inj.events[i];

                    if (event->stroke == status.sync.stroke)
                    {
                        status.inj.event = event->next;
                    }
                }

                for (i = 0; i < IGN_COUNT; i++)
                {
                    event = &status.ign.dwell_events[i];

                    if (event->stroke == status.sync.stroke)
                    {
                        status.ign.dwell_event = event->next;
                    }

                    event = &status.ign.spark_events[i];

                    if (event->stroke == status.sync.stroke)
                    {
                        status.ign.spark_event = event->next;
                    }
                }

                SETBIT(status.sync.flags1, SYNC_FLAGS1_SYNCED);
            }
        }
    }

    if ((TIM10->SR & TIM_SR_UIF))
    {
        TIM10->SR = ~TIM_SR_UIF;

        status.sync.stroke_ovf++;

        if (status.sync.stroke_ovf >= 16)
        {
            CLEARBIT(status.flags1, FLAGS1_RUN);
        }
    }
}

/**
 * Position ISR
 */
void TIM1_BRK_TIM9_IRQHandler(void)
{
    if ((TIM9->SR & TIM_SR_CC2IF))
    {
        uint8_t i, k;
        uint16_t ccr, cnt;
        sync_event_t *event;

        TIM9->SR = ~TIM_SR_CC2IF;

        // Do events only if synced
        if (TESTBIT(status.sync.flags1, SYNC_FLAGS1_SYNCED))
        {
            ccr = TIM9->CCR2;
            TIM9->CCR2 = (ccr >= 174) ? 0 : ccr + 6;

            cnt = TIM10->CNT;
            status.sync.cogs_period = (cnt > status.sync.prev_cogs_time) 
                ? (cnt - status.sync.prev_cogs_time) 
                : (65536 - (status.sync.prev_cogs_time - cnt));

            status.sync.prev_cogs_time  = cnt;
            TIM1->CNT = 0;

            // Spark
            event = status.ign.spark_event;
            
            if (
                (event->cogs == status.sync.cogs)
                && (event->stroke == status.sync.stroke)
            )
            {
                /*TIM1->CCR2 = ((uint32_t)event->ang_mod * status.sync.cogs_period) / 6;
                TIM1->SR = ~TIM_SR_CC2IF;
                TIM1->DIER |= TIM_DIER_CC2IE;*/
                GPIOD->BSRRH = GPIO_ODR_ODR_15;

                status.ign.spark_event = event->next;

                if (event->timing != status.ign.spark_timing)
                {
                    event_update(event, status.ign.spark_timing, 6);
                }
            }

            // Dwell
            event = status.ign.dwell_event;

            if (
                (event->cogs == status.sync.cogs)
                && (event->stroke == status.sync.stroke)
            )
            {
                /*TIM1->CCR1 = ((uint32_t)event->ang_mod * status.sync.cogs_period) / 6;
                TIM1->SR = ~TIM_SR_CC1IF;
                TIM1->DIER |= TIM_DIER_CC1IE;*/
                GPIOD->BSRRL = GPIO_ODR_ODR_15;

                status.ign.dwell_event = event->next;

                if (event->timing != status.ign.dwell_timing)
                {
                    event_update(event, status.ign.dwell_timing, 6);
                }
            }

            // Injection
            event = status.inj.event;

            if (
                (event->cogs == status.sync.cogs)
                && (event->stroke == status.sync.stroke)
            )
            {
                inj_start(event->offset);
                status.inj.event = event->next;

                if (event->timing != status.inj.timing)
                {
                    event_update(event, status.inj.timing, config.inj_timing_step);
                }
            }

            // Knock

            // Strobe
            if (status.sync.stroke == 0)
            {
                if (status.sync.cogs == 0)
                {
                    GPIOD->ODR |= GPIO_ODR_ODR_12;
                }
                else if (status.sync.cogs == 15)
                {
                    GPIOD->ODR &= ~GPIO_ODR_ODR_12;
                }
            }

            // Increment cogs
            if ((++status.sync.cogs) == 30)
            {
                status.sync.cogs = 0;

                if ((++status.sync.stroke) == 4)
                {
                    status.sync.stroke = 0;
                }
            }
        }
    }

    if ((TIM9->SR & TIM_SR_UIF))
    {
        TIM9->SR = ~TIM_SR_UIF;
    }
}

/**
 * Events ISR
 */
void TIM1_CC_IRQHandler(void)
{
    if (TIM1->SR & TIM_SR_CC1IF)
    {
        TIM1->SR = ~TIM_SR_CC1IF;
        //TIM1->DIER &= ~TIM_DIER_CC1IE;
        GPIOD->BSRRL = GPIO_ODR_ODR_15;
    }

    if (TIM1->SR & TIM_SR_CC2IF)
    {
        TIM1->SR = ~TIM_SR_CC2IF;
        //TIM1->DIER &= ~TIM_DIER_CC2IE;
        GPIOD->BSRRH = GPIO_ODR_ODR_15;
    }

    if ((TIM1->SR & TIM_SR_CC3IF))
    {
        TIM1->SR = ~TIM_SR_CC3IF;
    }

    if ((TIM1->SR & TIM_SR_CC4IF))
    {
        TIM1->SR = ~TIM_SR_CC4IF;
    }
}
