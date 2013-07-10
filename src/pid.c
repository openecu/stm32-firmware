#include "pid.h"

void pid_init(pid_t *pid, pid_config_t *config)
{
    pid->config = config;
    pid->prev_error = 0;
    pid->error_sum = 0;
}

int16_t pid_do(pid_t *pid, int16_t target, int16_t current)
{
    int32_t result, error, delta, error_sum;

    error = target - current;

    if (error > 32767)
    {
        error = 32767;
    }
    else if (error < -32768)
    {
        error = -32768;
    }

    error_sum = pid->error_sum + error;

    if (error_sum > pid->config->i_max)
    {
        error_sum = pid->config->i_max;
    }
    else if (error_sum < pid->config->i_min)
    {
        error_sum = pid->config->i_min;
    }

    pid->error_sum = error_sum;
    delta = pid->prev_error - error;
    pid->prev_error = error;

    result = (error * pid->config->kp / 256) 
        + (error_sum * pid->config->ki / 256) 
        + (delta * pid->config->kd / 256);

    if (result > 32767)
    {
        result = 32767;
    }
    else if (result < -32768)
    {
        result = -32768;
    }

    return (int16_t)result;
}
