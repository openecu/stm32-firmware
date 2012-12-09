#ifndef _HWCONF_H
#define _HWCONF_H

#include "pwm.h"

/* Actuators */
#define ECU_RELAY_PIN   0
#define FUEL_PUMP_PIN   1
#define COOLING_FAN_PIN 2

#define ACTUATORS_GPIO      GPIOA
#define ACTUATORS_PORT_MASK ((1 << ECU_RELAY_PIN) | (1 << FUEL_PUMP_PIN) | (1 << COOLING_FAN_PIN))

/* Idle */
#define PWM_IDLE    PWM_CH0

#endif
