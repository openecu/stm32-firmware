#include "pid.h"

void pid_init(pid_t *pid, pid_config_t *config)
{
    pid->config = config;
    pid->integral = 0;
}

void pid_set_target(pid_t *pid, int16_t target)
{
    pid->target = target;
}

int16_t pid_do(pid_t *pid, int16_t current)
{
    int16_t result, error, diff;
    int32_t integral;

    error = current - pid->target;
    diff = error - pid->error;
    integral = pid->integral + error;

    if (integral > pid->config->integral_max)
    {
        integral = pid->config->integral_max;
    }
    else if (integral < -pid->config->integral_max)
    {
        integral = -pid->config->integral_max;
    }

    pid->integral = integral;

    result = (error     * pid->config->kp / 256)
            + (integral * pid->config->ki / 256)
            + (diff     * pid->config->kd / 256);

    return result;
}
