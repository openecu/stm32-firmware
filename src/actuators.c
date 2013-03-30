#include <stm32f4xx.h>
#include "actuators.h"
#include "ecu.h"

#define ACTS_GPIO   GPIOG

#define ACTS_FAN_ODR    GPIO_ODR_ODR_0
#define ACTS_VVT_ODR    GPIO_ODR_ODR_1

#define ACTS_FLAGS1_FAN  0x00000001
#define ACTS_FLAGS1_VVT  0x00000002

extern ecu_t ecu;
uint32_t flags1;

void actuators_init(void)
{
    ACTS_GPIO->MODER = 0x55555555;
    ACTS_GPIO->OTYPER |= 0x0000FFFF;
    //ACTS_GPIO->OSPEEDR = 0x55555555;
    ACTS_GPIO->ODR = 0x00000000;
}

/**
 * Управление вентилятором системы охлаждения.
 */
void cooling_fan(void)
{
    // Если двигатель не запущен или в режиме пуска принудительно отключаем вентилятор
    if (
        !(ecu.status.flags1 & STATUS_FLAGS1_RUN) 
        || (ecu.status.flags1 & STATUS_FLAGS1_CRANK)
    )
    {
        flags1 &= ~ACTS_FLAGS1_FAN;
    }
    else
    {
        int16_t ect;
        ect = ecu.sensors.ect;

        // Если вентилятор включен, то отключаем с заданным гистерезисом
        if ((flags1 & ACTS_FLAGS1_FAN))
        {
            if ((ect + ecu.config.fan_ect_hyst) < ecu.config.fan_ect_thres)
            {
                flags1 &= ~ACTS_FLAGS1_FAN;
            }
        }
        // Если вентилятор выключен, то включаем по заданному порогу температуры ОЖ
        else
        {
            if (ect >= ecu.config.fan_ect_thres)
            {
                flags1 |= ACTS_FLAGS1_FAN;
            }
        }
    }

    if ((flags1 & ACTS_FLAGS1_FAN))
    {
        ecu.status.flags2 |= STATUS_FLAGS2_FAN;
        ACTS_GPIO->ODR |= ACTS_FAN_ODR;
    }
    else
    {
        ecu.status.flags2 &= ~STATUS_FLAGS2_FAN;
        ACTS_GPIO->ODR &= ~ACTS_FAN_ODR;   
    }
}

/**
 * Управление системой изменения фаз газораспределения.
 *
 * @note Реализация для клапана управляемого по оборотам ДВС.
 * @todo Реализация для фазовращателя.
 */
void vvt(void)
{
    // Если двигатель не запущен или в режиме пуска принудительно отключаем соленоид
    if (
        !(ecu.status.flags1 & STATUS_FLAGS1_RUN) 
        || (ecu.status.flags1 & STATUS_FLAGS1_CRANK)
    )
    {
        flags1 &= ~ACTS_FLAGS1_VVT;
    }
    else
    {
        int16_t rpm;
        rpm = ecu.sensors.rpm;

        // Если соленоид включен, то отключаем с заданным гистерезисом
        if ((flags1 & ACTS_FLAGS1_VVT))
        {
            if ((rpm + ecu.config.vvt_rpm_hyst) < ecu.config.vvt_rpm_thres)
            {
                flags1 &= ~ACTS_FLAGS1_VVT;
            }
        }  
        // Если соленоид выключен, то включаем по заданному порогу оборотов ДВС
        else
        {
            if (rpm >= ecu.config.vvt_rpm_thres)
            {
                flags1 |= ACTS_FLAGS1_VVT;
            }
        }
    }

    if ((flags1 & ACTS_FLAGS1_VVT))
    {
        ecu.status.flags2 |= STATUS_FLAGS2_VVT;
        ACTS_GPIO->ODR |= ACTS_VVT_ODR;
    }
    else
    {
        ecu.status.flags2 &= ~STATUS_FLAGS2_VVT;
        ACTS_GPIO->ODR &= ~ACTS_VVT_ODR;
    }
}
