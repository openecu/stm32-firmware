#ifndef STATUS_H
#define STATUS_H

#include "idle.h"
#include "ignition.h"
#include "injection.h"
#include "sensors.h"
#include "sync.h"
#include "comm.h"

#define FLAGS1_RUN      1
#define FLAGS1_STROKE   2

typedef struct status_s
{
    /* Sensors */
    // RPM
    uint16_t rpm;
    // Previous RPM
    uint16_t prev_rpm;
    // Delta RPM
    int16_t delta_rpm;
    // Throttle position
    uint16_t tp;
    // Previous throttle position
    uint16_t prev_tp;
    // Delta throttle position
    int16_t delta_tp;
    // Engine coolant temperature
    int16_t ect;
    // Battery voltage
    uint16_t batv;
    // Injection pulse width
    uint16_t inj_pw;

    /* Peripheral */
    // ADC
    sens_state_t sens;
    // Sync
    sync_state_t sync;
    // Injection
    inj_state_t inj;
    // Ignition
    ign_state_t ign;
    // Idle
    idle_state_t idle;
    // Comm
    comm_state_t comm;

    /* Flags */
    uint32_t flags1;

} status_t;

extern status_t status;

#endif
