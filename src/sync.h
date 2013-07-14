#ifndef SYNC_H
#define SYNC_H

#include "cmsis/stm32f4xx.h"

#define SYNC_FLAGS1_SYNCED  0

#define SYNC_STROKE_COUNT   4

typedef struct sync_event_s
{
    // Timing
    uint16_t timing;
    // Timing stroke
    uint8_t stroke;
    // Timing cogs
    uint8_t cogs;
    // Angle between cogs
    uint8_t ang_mod;
    // Stroke offset
    uint8_t offset;
    // Next event in sequence
    struct sync_event_s *next;

} sync_event_t;

typedef struct sync_state_s
{
    // Stroke counter
    uint8_t stroke;
    // Previous stroke time
    uint16_t prev_stroke_time;
    // Stroke period overflow counter
    uint8_t stroke_ovf;
    // Stroke period
    uint32_t stroke_period;
    // Previous period time
    uint32_t prev_stroke_period;
    // Cogs counter
    uint8_t cogs;
    // Previous cogs time
    uint16_t prev_cogs_time;
    // Time between cogs
    uint16_t cogs_period;
    // Previous cogs period
    uint16_t prev_cogs_period;
    // Instant frequency
    uint16_t inst_freq;
    // Frequency buffer
    uint16_t freq_buf[SYNC_STROKE_COUNT];
    // Frequency buffer head index
    uint16_t freq_head;
    // Filtered frequency
    uint16_t filt_freq;
    // Flags 1
    uint32_t flags1;

} sync_state_t;

void sync_init(void);

void sync_freq_calc(void);

void event_queue_init(sync_event_t events[], uint8_t n, uint16_t timing);

void event_update(sync_event_t *event, uint16_t target, uint16_t step);

#endif
