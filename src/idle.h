#ifndef IDLE_H
#define IDLE_H

#include "cmsis/stm32f4xx.h"
#include "pid.h"

typedef struct
{
    // PID
    pid_t pid;
    // Target RPM
    uint16_t target_rpm;
    // PWM duty
    uint16_t pwm_duty;
    // Delta RPM
    int16_t delta_rpm;
    // Ignition timing offset
    int16_t ign_offset;

} idle_state_t;

void idle_init(void);

void idle_control(void);

void idle_ign_timing_adjust(void);

#endif
