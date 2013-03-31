#include <stm32f4xx.h>
#include "switch.h"
#include "ecu.h"

#define SWITCH_GPIO GPIOD

#define SWITCH_IGN_ODR      GPIO_ODR_ODR_0
#define SWITCH_START_ODR    GPIO_ODR_ODR_1

extern ecu_t ecu;

void switch_init(void)
{
    SWITCH_GPIO->MODER &= ~0x0000FFFF;
    SWITCH_GPIO->OSPEEDR |= 0x00005555;
    SWITCH_GPIO->PUPDR |= 0x00005555;
}

void switch_update(void)
{
    uint32_t swst = SWITCH_GPIO->IDR & 0x000000FF;
    uint32_t flags = 0;

    if ((swst & SWITCH_IGN_ODR))
    {
        flags |= STATUS_FLAGS2_IGN_SW;
    }
    else if ((swst & SWITCH_START_ODR))
    {
        flags |= STATUS_FLAGS2_START_SW;
    }

    __disable_irq();
    ecu.status.flags2 &= ~(STATUS_FLAGS2_IGN_SW | STATUS_FLAGS2_START_SW);
    ecu.status.flags2 |= flags;
    __enable_irq();
}
