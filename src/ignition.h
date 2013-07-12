#ifndef IGNITION_H
#define IGNITION_H

#include "cmsis/stm32f4xx.h"
#include "sync.h"

#define IGN_COUNT   4 

typedef struct ign_state_s
{
    // Dwell events
    sync_event_t dwell_events[IGN_COUNT];
    // Current dwell event
    sync_event_t *dwell_event;
    // Dwell timing
    uint16_t dwell_timing;
    // Spark events
    sync_event_t spark_events[IGN_COUNT];
    // Current spark event
    sync_event_t *spark_event;
    // Spark timing
    uint16_t spark_timing;

} ign_state_t;

void ign_init(void);

#endif
