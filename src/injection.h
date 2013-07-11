#ifndef INJECTION_H
#define INJECTION_H

#include "cmsis/stm32f4xx.h"
#include "sync.h"

#define INJ_COUNT	4

typedef struct
{
	// Injection events
	sync_event_t events[INJ_COUNT];
	// Current event
	sync_event_t *event;
	// Target injection timing
	uint16_t timing;
	// Injection pulse width
	uint16_t pw;

} inj_state_t;

void inj_init(void);

void inj_start(uint8_t no);

void inj_stop(uint8_t no);

#endif
