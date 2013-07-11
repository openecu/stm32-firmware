#include "cmsis/stm32f4xx.h"
#include "ignition.h"
#include "status.h"
#include "config.h"

void ign_init(void)
{
    uint8_t i, k;
    sync_event_t *event;

    status.ign.dwell_timing = 331;
    status.ign.spark_timing = 363;

    // Initialize dwell events
    for (i = 0; i < INJ_COUNT; i++)
    {
        event = &status.ign.dwell_events[i];
        event->timing   = 0;
        event->cogs     = 0;
        event->stroke   = i;
        event->offset   = i;
        k = (i < (INJ_COUNT - 1)) ? (i + 1) : 0;
        event->next = &status.ign.dwell_events[k];
        event_update(event, status.ign.dwell_timing, 720);
    }

    // Initialize spark events
    for (i = 0; i < INJ_COUNT; i++)
    {
        event = &status.ign.spark_events[i];
        event->timing   = 0;
        event->cogs     = 0;
        event->stroke   = i;
        event->offset   = i;
        k = (i < (INJ_COUNT - 1)) ? (i + 1) : 0;
        event->next = &status.ign.spark_events[k];
        event_update(event, status.ign.spark_timing, 720);
    }
}
