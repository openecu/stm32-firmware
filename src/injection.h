#ifndef _INJECTION_H
#define _INJECTION_H

#include <stdint.h>

void warmup_enrich(void);

void accel_enrich(uint32_t dtime);

void inj_afr(void);

void inj_trim(void);

void inj_calc_pw(void);

void inj_timing(void);

#endif
