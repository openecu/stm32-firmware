#ifndef _SYNC_H
#define _SYNC_H

#include <inttypes.h>
#include "ecu.h"

typedef struct
{
    uint8_t new_cycle: 1;
    uint8_t ref;
    uint32_t ref_time;
    uint32_t ref_dt;
    uint16_t ref_pos;
    uint16_t pos;
    uint16_t pos_time;
    uint16_t pos_dt;
} sync_t;

void sync_init(void);

uint8_t sync_is_new_cycle(void);

void sync_wait_cycle(void);

void sync_calc_rpm(void);

void sync_pos_CAPT_ISR(void);

void sync_ref_CAPT_ISR(void);

#endif
