#ifndef _HWCONF_H
#define _HWCONF_H

#include <stm32f4xx.h>
#include "pwm.h"
#include "sync.h"

/* Input switches */
#define IGN_SWITCH
#define START_SWITCH
#define NEUTRAL_SWITCH

#define IGN_SWITCH_PIN      0
#define START_SWITCH_PIN    1
#define NEUTRAL_SWITCH_PIN  2

#define SWITCHES_GPIO GPIOD
#define SWITCH_STATE(name)    (SWITCHES_GPIO->IDR & (1 << name##_PIN))

/* Actuators */
#define ECU_RELAY
#define FUEL_PUMP
#define COOLING_FAN

#define ECU_RELAY_PIN   0
#define FUEL_PUMP_PIN   1
#define COOLING_FAN_PIN 2

#define ACTUATORS_GPIO      GPIOG

#define ACTUATORS_OFF()     ACTUATORS_GPIO->ODR = 0
#define ACTUATOR_ON(name)   ACTUATORS_GPIO->ODR |= (1 << name##_PIN)
#define ACTUATOR_OFF(name)  ACTUATORS_GPIO->ODR &= ~(1 << name##_PIN)

/* Idle */
#define PWM_IDLE    PWM_CH0

/* Synchronization */
#define SYNC_GPIO           GPIOB
#define SYNC_POS_PIN        10
#define SYNC_REF_PIN        11
#define SYNC_REF_STATE()    (SYNC_GPIO->IDR & (1 << SYNC_REF_PIN))

static inline void hw_init(void)
{
    RCC->APB2ENR |= RCC_AHB1ENR_GPIOGEN | RCC_AHB1ENR_GPIODEN;
    GPIOG->MODER |= 0x55555555;
    GPIOD->MODER &= ~0x0000FFFF;

    // TIM2
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
    TIM2->CR1 |= TIM_CR1_URS;
    //TIM2->CR2 = 0x0000;
    //TIM2->CCMR1 |= TIM_CCMR1_CC2S_0 | TIM_CCMR1_CC1S_0;
    TIM2->CCMR2 |= TIM_CCMR2_CC4S_0 | TIM_CCMR2_CC3S_0; // capture 3, 4
    TIM2->CCER |= (TIM_CCER_CC4P | TIM_CCER_CC4NP | TIM_CCER_CC4E)
            | (TIM_CCER_CC3P | TIM_CCER_CC3NP | TIM_CCER_CC3E); // capture on both edges
    TIM2->PSC = 21; // 168/21 MHz, 0.125 us resolution
    TIM2->CR1 |= TIM_CR1_CEN; // enable clock
}

static void TIM2_IRQHandler(void)
{
    if (TIM2->SR & TIM_SR_CC1IF)
    {
        TIM2->SR &= ~TIM_SR_CC1IF;
    }
    else if (TIM2->SR & TIM_SR_CC2IF)
    {
        TIM2->SR &= ~TIM_SR_CC2IF;
    }
    else if (TIM2->SR & TIM_SR_CC3IF)
    {
        TIM2->SR &= ~TIM_SR_CC3IF;
        sync_pos_trig();
    }
    else if (TIM2->SR & TIM_SR_CC4IF)
    {
        TIM2->SR &= ~TIM_SR_CC4IF;
        sync_ref_trig();
    }
}

static void TIM3_IRQHandler(void)
{

}

static void TIM5_IRQHandler(void)
{

}

static void TIM4_IRQHandler(void)
{

}

static void TIM8_IRQHandler(void)
{

}

#endif
