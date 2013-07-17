#include "idle.h"
#include "math_ext.h"
#include "pid.h"
#include "status.h"
#include "config.h"

/*
    Idle control init
*/
void idle_init(void)
{
    pid_init(&status.idle.pid, &config.idle_pid);

    TIM13->PSC = config.idle_pwm_frq_div;
    TIM13->ARR = 1024;
    TIM13->CCR1 = 0;
    TIM13->CCMR1 |= TIM_CCMR1_OC1PE | (TIM_CCMR1_OC2M_2 | TIM_CCMR1_OC2M_1);
    TIM13->CCER |= TIM_CCER_CC1E;
    TIM13->CR1 |= TIM_CR1_CEN;
}

/*
    PID idle control
*/
void idle_control(void)
{
    uint16_t target_rpm, pwm_duty;

    target_rpm = table1d_lookup(status.ect, CONF_IDLE_RPM_SIZE, 
        config.idle_rpm_ect, config.idle_rpm);

    pwm_duty = pid_do(&status.idle.pid, target_rpm, status.rpm);

    if (pwm_duty < config.idle_pwm_duty_min)
    {
        pwm_duty = config.idle_pwm_duty_min;
    }
    else if (pwm_duty > config.idle_pwm_duty_max)
    {
        pwm_duty = config.idle_pwm_duty_max;
    }

    TIM13->CCR1 = pwm_duty;

    status.idle.target_rpm = target_rpm;
    status.idle.pwm_duty = pwm_duty;
}

/*
    Idle ignition timing adjust
*/
void idle_ign_timing_adjust(void)
{
    int16_t delta_rpm, ign_offset;

    delta_rpm = status.idle.target_rpm - 0; //status.sync.inst_freq;

    if (ABS(delta_rpm) < config.idle_ign_adj_rpm_thres)
    {
        ign_offset = 0;
    }
    else
    {
        ign_offset = table1d_lookup(delta_rpm, CONF_IDLE_IGN_OFFSET_SIZE, 
            config.idle_ign_offset_rpm, config.idle_ign_offset);
    }

    status.idle.delta_rpm = delta_rpm;
    status.idle.ign_offset = ign_offset;
}
