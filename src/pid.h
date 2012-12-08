#ifndef _PID_H
#define _PID_H

#include <inttypes.h>

typedef struct
{
	uint8_t kp;
	uint8_t ki;
	uint8_t kd;
	uint32_t integral_max;
} pid_config_t;

typedef struct
{
	pid_config_t *config;
	uint16_t target;
	int16_t error;
	int32_t integral;
} pid_t;

void pid_init(pid_t *pid, pid_config_t *config);

void pid_set_target(pid_t *pid, uint16_t target);

int16_t pid_do(pid_t *pid, uint16_t current);

#endif
