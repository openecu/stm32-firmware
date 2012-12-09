#ifndef _PID_H
#define _PID_H

#include <inttypes.h>
#include "config.h"

typedef struct
{
    pid_config_t *config;
    int16_t target;
    int16_t error;
    int32_t integral;
} pid_t;

void pid_init(pid_t *pid, pid_config_t *config);

void pid_set_target(pid_t *pid, int16_t target);

int16_t pid_do(pid_t *pid, int16_t current);

#endif
