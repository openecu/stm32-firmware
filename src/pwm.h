#ifndef _PWM_H
#define _PWM_H

#include <inttypes.h>

#define PWM_CH0 0
#define PWM_CH1 1
#define PWM_CH2 2
#define PWM_CH3 3

void pwm_set_duty(uint8_t ch, uint16_t duty);

#endif
