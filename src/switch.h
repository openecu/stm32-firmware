#ifndef _SWITCH_H
#define _SWITCH_H

#include <stm32f4xx.h>

#define SWITCH_GPIO GPIOD

#define SWITCH_IGN_ODR      GPIO_ODR_ODR_0
#define SWITCH_START_ODR    GPIO_ODR_ODR_1

void switch_init(void);

void switch_update(void);

#endif
