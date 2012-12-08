#include "idle.h"
#include "ecu.h"
#include "pid.h"
#include "pwm.h"
#include "mathext.h"

extern ecu_t ecu;
pid_t idle_pid;

void idle_init(void)
{
    pid_init(&idle_pid, &ecu.config.idle_pid_config);
}

void idle_control(void)
{
    uint16_t target_rpm;
    int16_t duty;

    target_rpm = table1d_lookup(
        (int16_t)ecu.sensors.ect, 16,
        (int16_t*)ecu.config.idle_temp_scale, (int16_t*)ecu.config.idle_rpm
    );
    pid_set_target(&idle_pid, target_rpm);
    duty = pid_do(&idle_pid, ecu.sensors.rpm);
    pwm_set_duty(0, (512 + (duty >> 6)));
}
