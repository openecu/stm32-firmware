#ifndef SYNC_H
#define SYNC_H

#include "cmsis/stm32f4xx.h"

#define SYNC_FLAGS1_SYNCED  0

typedef struct sync_event_s
{
    // Timing
    uint16_t timing;
    // Timing stroke
    uint8_t stroke;
    // Timing cogs
    uint8_t cogs;
    // Stroke offset
    uint8_t offset;
    // Next event in sequence
    struct sync_event_s *next;

} sync_event_t;

typedef struct sync_state_s
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

void event_update(sync_event_t *event, uint16_t target, uint16_t step);

#endif
