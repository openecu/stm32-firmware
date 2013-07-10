#ifndef SYNC_H
#define SYNC_H

#include "cmsis/stm32f4xx.h"

#define SYNC_FLAGS1_SYNCED  0
#define SYNC_FLAGS1_EVENT1  1

typedef struct
{
    // Timing
    uint16_t timing;
    // Stroke
    uint8_t stroke;
    // Previous stroke
    uint8_t prev_stroke;
    // Cogs
    uint8_t cogs;

} sync_event_t;

typedef struct
{
    // Stroke counter
    uint8_t stroke;
    // Stroke time overflow counter
    uint8_t stroke_ovf;
    // Stroke time
    uint32_t stroke_time;
    // Previous reference time
    uint32_t prev_stroke_time;
    // Cogs
    uint8_t cogs;
    // Flags 1
    uint32_t flags1;

} sync_state_t;

void sync_init(void);

void calc_rpm(void);

#endif
