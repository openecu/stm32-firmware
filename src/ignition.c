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
    status.ign.dwell_timing = 351;
    event_queue_init(status.ign.dwell_events, IGN_COUNT, status.ign.dwell_timing);

    status.ign.spark_timing = 363;
    event_queue_init(status.ign.spark_events, IGN_COUNT, status.ign.spark_timing);
}

/**
 * Calculate dwell time
 */
void ign_dwell_calc(void)
{
    status.ign.dwell_time = table1d_lookup(status.batv, CONF_IGN_DWELL_SIZE, 
        config.ign_dwell_batv, config.ign_dwell);
}

/**
 * Calculate spark timing
 */
void ign_timing_calc(void)
{
    int16_t timing_adv;

    if (TSTBIT(status.flags1, FLAGS1_CRANK))
    {
        timing_adv = table1d_lookup(status.sync.filt_freq, CONF_CRANK_IGN_TIMING_SIZE, 
            config.crank_ign_timing_rpm, config.crank_ign_timing);
    }
    else if (TSTBIT(status.flags1, FLAGS1_IDLE))
    {
        timing_adv = table1d_lookup(status.sync.filt_freq, CONF_IDLE_IGN_TIMING_SIZE, 
            config.idle_ign_timing_rpm, config.idle_ign_timing);
        timing_adv += status.idle.ign_offset;
    }
    else
    {
        timing_adv = table2d_lookup(status.load, status.sync.filt_freq,
            CONF_IGN_TIMING_LOAD_SIZE, CONF_IGN_TIMING_RPM_SIZE,
            config.ign_timing_load, config.ign_timing_rpm, config.ign_timing);
    }

    //status.ign.spark_timing = 360 - timing_adv;
}
