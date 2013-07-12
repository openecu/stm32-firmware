#include "cmsis/stm32f4xx.h"
#include "ignition.h"
#include "status.h"
#include "config.h"

void ign_init(void)
{
    status.ign.dwell_timing = 331;
    status.ign.spark_timing = 363;
    event_queue_init(status.ign.dwell_events, IGN_COUNT, status.ign.dwell_timing);
    event_queue_init(status.ign.spark_events, IGN_COUNT, status.ign.spark_timing);
}
