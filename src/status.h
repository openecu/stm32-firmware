#ifndef STATUS_H
#define STATUS_H

#include "idle.h"
#include "ignition.h"
#include "injection.h"
#include "sensors.h"
#include "sync.h"
#include "comm.h"

#define SETBIT(addr, bit)   (*(vu32 *)(SRAM1_BB_BASE | ((((u32)&addr) - SRAM1_BASE) << 5) | ((bit) << 2)) = 1)    
#define CLRBIT(addr, bit)   (*(vu32 *)(SRAM1_BB_BASE | ((((u32)&addr) - SRAM1_BASE) << 5) | ((bit) << 2)) = 0)    
#define TSTBIT(addr, bit)   (*(vu32 *)(SRAM1_BB_BASE | ((((u32)&addr) - SRAM1_BASE) << 5) | ((bit) << 2)))

#define SETREG(addr, bit)   (*(vu32 *)(PERIPH_BB_BASE | ((((u32)&addr) - PERIPH_BASE) << 5) | ((bit) << 2)) = 1)    
#define CLRREG(addr, bit)   (*(vu32 *)(PERIPH_BB_BASE | ((((u32)&addr) - PERIPH_BASE) << 5) | ((bit) << 2)) = 0)    
#define TSTREG(addr, bit)   (*(vu32 *)(PERIPH_BB_BASE | ((((u32)&addr) - PERIPH_BASE) << 5) | ((bit) << 2)))

#define FLAGS1_RUN          0
#define FLAGS1_STROKE       1
#define FLAGS1_CRANK        2
#define FLAGS1_IDLE         3

typedef struct status_s
{
    /* Sensors */
    // RPM
    uint16_t rpm;
    // Previous RPM
    uint16_t prev_rpm;
    // Delta RPM
    int16_t delta_rpm;
    // Load
    uint16_t load;
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
