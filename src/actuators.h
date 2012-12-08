#ifndef _ACTUATORS_H
#define _ACTUATORS_H

#include <stm32f4xx.h>
#include <inttypes.h>

/* Actuators */

#define ECU_RELAY_PIN   0
#define CE_LAMP_PIN     1
#define FUEL_PUMP_PIN   2
#define COOLING_FAN_PIN 3
#define WATER_PUMP_PIN  4

#define ACTUATORS_GPIO      GPIOA
#define ACTUATORS_PORT_MASK ((1 << ECU_RELAY_PIN) \
    | (1 << CE_LAMP_PIN) \
    | (1 << FUEL_PUMP_PIN) \
    | (1 << COOLING_FAN_PIN) \
    | (1 << WATER_PUMP_PIN))

#define ACTUATORS_OFF() ACTUATORS_GPIO->ODR &= ~ACTUATORS_PORT_MASK

#define ACTUATOR_ON(name)   ACTUATORS_GPIO->ODR |= (1 << name##_PIN)
#define ACTUATOR_OFF(name)  ACTUATORS_GPIO->ODR &= ~(1 << name##_PIN)

void actuators_init(void);

static inline void ecu_relay_on(void)
{
    ACTUATOR_ON(ECU_RELAY);
}

static inline void ecu_relay_off(void)
{
    ACTUATOR_OFF(ECU_RELAY);
}

static inline void fuel_pump_relay_on(void)
{
    ACTUATOR_ON(FUEL_PUMP);
}

static inline void fuel_pump_relay_off(void)
{
    ACTUATOR_OFF(FUEL_PUMP);
}

static inline void ce_lamp_on(void)
{
    ACTUATOR_ON(CE_LAMP);
}

static inline void ce_lamp_off(void)
{
    ACTUATOR_OFF(CE_LAMP);
}

void cooling_fan(void);

void water_pump(void);

/* Auxiliary outputs */

#define AUX_COUNT   8

#define AUX_GPIO        GPIOA
#define AUX_PORT_OFFSET 8
#define AUX_PORT_MASK   (0xFF << AUX_PORT_OFFSET)

#define AUX_OFF()   AUX_GPIO->ODR &= ~(1 << AUX_PORT_OFFSET);

#define AUX_CH_ON(num)     AUX_GPIO->ODR |= (1 << (num + AUX_PORT_OFFSET))
#define AUX_CH_OFF(num)    AUX_GPIO->ODR &= ~(1 << (num + AUX_PORT_OFFSET))

void aux(void);

#endif
