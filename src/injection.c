#include "cmsis/stm32f4xx.h"
#include "injection.h"
#include "status.h"
#include "config.h"

/** 
 * Initialize injection 
 */
void inj_init(void)
{
    uint8_t i, k;
    sync_event_t *event;

    status.inj.pw = 20000;
    status.inj.timing = 360;

    // Initialize injection start events
    for (i = 0; i < INJ_COUNT; i++)
    {
        event = &status.inj.events[i];
        event->timing   = 0;
        event->cogs     = 0;
        event->stroke   = i;
        event->offset   = i;
        k = (i < (INJ_COUNT - 1)) ? (i + 1) : 0;
        event->next = &status.inj.events[k];
        event_update(event, status.inj.timing, 720);
    }

    // Initialize injection stop events timer
    TIM2->PSC = 49;
    TIM2->CR1 |= TIM_CR1_CEN;

    NVIC_SetPriority(TIM2_IRQn, 4);
    NVIC_EnableIRQ(TIM2_IRQn);
}

/**
 * Calculate injector dead time
 */
void inj_deadtime_calc(void)
{
    status.inj.deadtime = table1d_lookup(status.batv, CONF_INJ_DEAD_SIZE, 
        config.inj_dead_batv, config.inj_dead);
}

/**
 * Start injection
 */
void inj_start(uint8_t no)
{
    if ((no == 0) || (no == 2))
    {
        TIM2->CCR1 = TIM2->CNT + status.inj.pw;
        TIM2->SR = ~TIM_SR_CC1IF;
        TIM2->DIER |= TIM_DIER_CC1IE;
        GPIOD->ODR |= GPIO_ODR_ODR_13;
    }
    else
    {
        TIM2->CCR2 = TIM2->CNT + status.inj.pw;
        TIM2->SR = ~TIM_SR_CC1IF;
        TIM2->DIER |= TIM_DIER_CC2IE;
        GPIOD->ODR |= GPIO_ODR_ODR_14;
    }
}

/**
 * Stop injection
 */
void inj_stop(uint8_t no)
{
    if ((no == 0) || (no == 2))
    {
        TIM2->DIER &= ~TIM_DIER_CC1IE;
        GPIOD->ODR &= ~GPIO_ODR_ODR_13;
    }
    else
    {
        TIM2->DIER &= ~TIM_DIER_CC2IE;
        GPIOD->ODR &= ~GPIO_ODR_ODR_14;
    }
}

/**
 * Injection stop events timer ISR
 */
void TIM2_IRQHandler(void)
{
    if ((TIM2->DIER & TIM_DIER_CC1IE) && (TIM2->SR & TIM_SR_CC1IF))
    {
        TIM2->SR = ~TIM_SR_CC1IF;
        inj_stop(0);
    }

    if ((TIM2->DIER & TIM_DIER_CC2IE) && (TIM2->SR & TIM_SR_CC2IF))
    {
        TIM2->SR = ~TIM_SR_CC2IF;
        inj_stop(1);
    }
}
