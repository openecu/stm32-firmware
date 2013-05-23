#include <stm32f4xx.h>
#include "actuators.h"
#include "ecu.h"

extern ecu_t ecu;

uint32_t flags1;
uint32_t fuel_pump_time;

/**
 * Инициализация исполнительных устройств.
 */
void actuators_init(void)
{
    ACTS_GPIO->MODER = 0x55555555;
    ACTS_GPIO->OTYPER &= ~0x0000FFFF;
    ACTS_GPIO->OSPEEDR = 0x55555555;
    ACTS_GPIO->ODR = 0x00000000;

    flags1 |= ACTS_FLAGS1_FUEL_PUMP_PRIME;
    fuel_pump_time = 0;
}

/**
 * Управление главным реле.
 */
void main_relay()
{
    // Если зажигание включено, то включаем главное реле 
    if ((ecu.status.flags2 & STATUS_FLAGS2_IGN_SW))
    {
        flags1 |= ACTS_FLAGS1_MAIN_RELAY;
    }
    // Если зажигание выключено, то выключаем главное реле после остановки двигателя?
    else
    {
        flags1 &= ~ACTS_FLAGS1_MAIN_RELAY;
    }

    if ((flags1 & ACTS_FLAGS1_MAIN_RELAY))
    {
        ecu.status.flags2 |= STATUS_FLAGS2_MAIN_RELAY;
        ACTS_GPIO->ODR |= ACTS_MAIN_RELAY_ODR;
    }
    else
    {
        ecu.status.flags2 &= ~STATUS_FLAGS2_MAIN_RELAY;
        ACTS_GPIO->ODR &= ~ACTS_MAIN_RELAY_ODR;   
    }
}

/**
 * Управление бензонасосом.
 */
void fuel_pump(uint32_t dtime)
{
    // Если двигатель запущен или в режиме пуска, то включаем бензонасос
    if (
        (ecu.status.flags1 & STATUS_FLAGS1_RUN)
        || (ecu.status.flags1 & STATUS_FLAGS1_CRANK)
    )
    {
        flags1 |= ACTS_FLAGS1_FUEL_PUMP;
    }
    else
    {
        // Если зажигание включено, то включаем бензонасос на заданное время
        if (
            (ecu.status.flags2 & STATUS_FLAGS2_IGN_SW)
            && (flags1 & ACTS_FLAGS1_FUEL_PUMP_PRIME)
        )
        {
            flags1 |= ACTS_FLAGS1_FUEL_PUMP;

            // Проверяем время работы бензонасоса
            if (
                (dtime >= ecu.config.fuel_pump_prime_time)
                || (fuel_pump_time >= ecu.config.fuel_pump_prime_time)
            )
            {
                flags1 &= ~(ACTS_FLAGS1_FUEL_PUMP | ACTS_FLAGS1_FUEL_PUMP_PRIME);
            }

            fuel_pump_time += dtime;
        }
        // Если зажигание выключено или истекло заданное время, то выключаем бензонасос
        else
        {
            flags1 &= ~ACTS_FLAGS1_FUEL_PUMP;
        }
    }

    if ((flags1 & ACTS_FLAGS1_FUEL_PUMP))
    {
        ecu.status.flags2 |= STATUS_FLAGS2_FUEL_PUMP;
        ACTS_GPIO->ODR |= ACTS_FUEL_PUMP_ODR;
    }
    else
    {
        ecu.status.flags2 &= ~STATUS_FLAGS2_FUEL_PUMP;
        ACTS_GPIO->ODR &= ~ACTS_FUEL_PUMP_ODR;   
    }
}

/**
 * Управление вентилятором системы охлаждения.
 */
void cooling_fan(void)
{
    // Если двигатель не запущен или в режиме пуска, то отключаем вентилятор
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
    // Если двигатель не запущен или в режиме пуска, то отключаем соленоид
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
