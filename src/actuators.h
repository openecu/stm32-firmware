#ifndef _ACTUATORS_H
#define _ACTUATORS_H

#include <stdint.h>

void main_relay(void);

void fuel_pump(uint32_t dtime);

void actuators_init(void);

void cooling_fan(void);

void vvt(void);

#endif
