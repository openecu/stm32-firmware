#ifndef _HWCONF_H
#define _HWCONF_H

#include <stm32f4xx.h>
#include "pwm.h"

/* Input switches */
#define IGN_SWITCH_PIN      0
#define START_SWITCH_PIN    1
#define NEUTRAL_SWITCH_PIN  2

#define SWITCHES_GPIO GPIOD

#define SWITCH_STATE(name)    (SWITCHES_GPIO->IDR & (1 << name##_PIN))

/* Actuators */
#define ECU_RELAY_PIN   0
#define FUEL_PUMP_PIN   1
#define COOLING_FAN_PIN 2

#define ACTUATORS_GPIO      GPIOG
//#define ACTUATORS_PORT_MASK ((1 << ECU_RELAY_PIN) | (1 << FUEL_PUMP_PIN) | (1 << COOLING_FAN_PIN))

#define ACTUATORS_OFF()     ACTUATORS_GPIO->ODR = 0 //~ACTUATORS_PORT_MASK
#define ACTUATOR_ON(name)   ACTUATORS_GPIO->ODR |= (1 << name##_PIN)
#define ACTUATOR_OFF(name)  ACTUATORS_GPIO->ODR &= ~(1 << name##_PIN)

/* Idle */
#define PWM_IDLE    PWM_CH0


static inline void hw_init(void)
{
    RCC->APB2ENR |= RCC_AHB1ENR_GPIOGEN | RCC_AHB1ENR_GPIODEN;
    GPIOG->MODER |= 0x55555555;
    GPIOD->MODER &= ~0x0000FFFF;
}

#endif
