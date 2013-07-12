#ifndef PID_H
#define PID_H

#include <inttypes.h>

typedef struct 
{
    int16_t kp;
    int16_t ki;
    int16_t kd;
    int32_t i_max;
    int32_t i_min;

} pid_config_t;

typedef struct
{
    pid_config_t *config;
    int16_t prev_error;
    int32_t error_sum;

} pid_t;

void pid_init(pid_t *pid, pid_config_t *config);

int16_t pid_do(pid_t *pid, int16_t target, int16_t current);

#endif
