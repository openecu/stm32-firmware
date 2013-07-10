#include "cmsis/stm32f4xx.h"
#include "sync.h"
#include "config.h"
#include "injection.h"
#include "status.h"

/*
    Initialize synchronization
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

/*
    Calculate and update RPM
*/
void calc_rpm(void)
{
    uint32_t stroke_time;

    stroke_time = status.sync.stroke_time;
    status.rpm = (stroke_time > 0) ? (30000000 / stroke_time) : 0;
}

/*
    Update event
*/
void update_event(sync_event_t *event, uint16_t target, uint8_t step)
{
    uint16_t tmp;

    tmp = event->timing;

    if (target > tmp)
    {
        tmp += step;

        if (tmp > target)
        {
            tmp = target;
        }
    }
    else if (target < tmp)
    {
        if (tmp < step)
        {
            tmp = target;
        }
        else
        {
            tmp -= step;

            if (tmp < target)
            {
                tmp = target;
            }
        }
    }

    event->timing   = tmp;
    event->cogs     = (tmp % 180) / 6;
    event->stroke   = (tmp / 180) + event->offset;

    if (event->stroke > 3)
    {
        event->stroke -= 4;
    }
}

/*
    Stroke ISR
*/
void TIM1_UP_TIM10_IRQHandler(void)
{
    if ((TIM10->SR & TIM_SR_CC1IF))
    {
        TIM10->SR &= ~TIM_SR_CC1IF;

        // Synced
        if (TESTBIT(status.sync.flags1, SYNC_FLAGS1_SYNCED))
        {
            // Rising edge
            if ((GPIOB->IDR & GPIO_IDR_IDR_8))
            {
                TIM10->CNT = 0;
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
                status.sync.stroke_time = ((uint32_t)status.sync.stroke_ovf << 16) + TIM10->CCR1;
                status.sync.stroke_ovf = 0;

                TIM10->SR &= ~TIM_SR_UIF;
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
                uint8_t pos;

                TIM10->CCER &= ~TIM_CCER_CC1P;
                pos = TIM9->CNT;

                // Cyl. 1
                if ((pos >= 15) && (pos <= 17))
                {
                    status.sync.stroke = 0;
                    SETBIT(status.sync.flags1, SYNC_FLAGS1_SYNCED);
                }
                // Cyl. 2
                else if ((pos >= 11) && (pos <= 13))
                {
                    status.sync.stroke = 1;
                    SETBIT(status.sync.flags1, SYNC_FLAGS1_SYNCED);
                }
                // Cyl. 3
                else if ((pos >= 7) && (pos <= 9))
                {
                    status.sync.stroke = 2;
                    SETBIT(status.sync.flags1, SYNC_FLAGS1_SYNCED);
                }
                // Cyl. 4
                else if ((pos >= 3) && (pos <= 5))
                {
                    status.sync.stroke = 3;
                    SETBIT(status.sync.flags1, SYNC_FLAGS1_SYNCED);
                }
            }
        }
    }

    if ((TIM10->SR & TIM_SR_UIF))
    {
        TIM10->SR &= ~TIM_SR_UIF;

        status.sync.stroke_ovf++;

        if (status.sync.stroke_ovf >= 16)
        {
            CLEARBIT(status.flags1, FLAGS1_RUN);
        }
    }
}

/*
    Position ISR
*/
void TIM1_BRK_TIM9_IRQHandler(void)
{
    if ((TIM9->SR & TIM_SR_CC2IF))
    {
        uint8_t i, k;
        uint16_t ccr;
        sync_event_t *event;

        TIM9->SR = ~TIM_SR_CC2IF;

        ccr = TIM9->CCR2;
        TIM9->CCR2 = (ccr >= 174) ? 0 : ccr + 6;

        // Injection
        for (i = 0; i < INJ_COUNT; i++)
        {
            event = &status.inj.events[i];

            if (
                (event->cogs == status.sync.cogs)
                && (event->stroke == status.sync.stroke)
            )
            {
                inj_start(i);
                update_event(event->next, status.inj.timing, config.inj_timing_step);
            }
        }

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

    if ((TIM9->SR & TIM_SR_UIF))
    {
        TIM9->SR = ~TIM_SR_UIF;
    }
}

/*
    Events ISR
*/
void TIM1_CC_IRQHandler(void)
{
    if ((TIM1->SR & TIM_SR_CC1IF))
    {
        TIM1->SR = ~TIM_SR_CC1IF;
    }

    if ((TIM1->SR & TIM_SR_CC2IF))
    {
        TIM1->SR = ~TIM_SR_CC2IF;
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
