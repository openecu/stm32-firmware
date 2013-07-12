#include "cmsis/stm32f4xx.h"
#include "ignition.h"
#include "status.h"
#include "config.h"
#include "math_ext.h"

/**
 * Initialize ignition
 */
void ign_init(void)
{
    status.ign.dwell_timing = 300;
    status.ign.spark_timing = 360;
    event_queue_init(status.ign.dwell_events, IGN_COUNT, status.ign.dwell_timing);
    event_queue_init(status.ign.spark_events, IGN_COUNT, status.ign.spark_timing);
}

/**
 * Calculate dwell time
 */
void ign_dwell_calc(void)
{

}

/**
 * Calculate spark timing
 */
void ign_timing_calc(void)
{
    int16_t timing;

    if (TESTBIT(status.flags1, FLAGS1_CRANK))
    {
        timing = table1d_lookup(status.rpm, CONF_CRANK_IGN_TIMING_SIZE, 
            config.crank_ign_timing_rpm, config.crank_ign_timing);
    }
    else if (TESTBIT(status.flags1, FLAGS1_IDLE))
    {
        timing = table1d_lookup(status.rpm, CONF_IDLE_IGN_TIMING_SIZE, 
            config.idle_ign_timing_rpm, config.idle_ign_timing);

        timing += status.idle.ign_offset;
    }
    else
    {
        timing = table2d_lookup(status.load, status.rpm, 
            CONF_IGN_TIMING_LOAD_SIZE, CONF_IGN_TIMING_RPM_SIZE
            config.ign_timing_load, config.ign_timing_rpm, config.ign_timing);
    }

    status.ign.spark_timing = 360 - timing;
}
