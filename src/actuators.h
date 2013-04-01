#ifndef _ACTUATORS_H
#define _ACTUATORS_H

#include <stm32f4xx.h>

#define ACTS_GPIO   GPIOG

#define ACTS_FAN_ODR        GPIO_ODR_ODR_0
#define ACTS_VVT_ODR        GPIO_ODR_ODR_1
#define ACTS_FUEL_PUMP_ODR  GPIO_ODR_ODR_2
#define ACTS_MAIN_RELAY_ODR GPIO_ODR_ODR_3

#define ACTS_FLAGS1_FAN             0x00000001
#define ACTS_FLAGS1_VVT             0x00000002
#define ACTS_FLAGS1_FUEL_PUMP       0x00000004
#define ACTS_FLAGS1_FUEL_PUMP_PRIME 0x00000008
#define ACTS_FLAGS1_MAIN_RELAY      0x00000010

void main_relay(void);

void fuel_pump(uint32_t dtime);

void actuators_init(void);

void cooling_fan(void);

void vvt(void);

#endif
