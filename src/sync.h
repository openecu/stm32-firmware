#ifndef SYNC_H
#define SYNC_H

#include "cmsis/stm32f4xx.h"

#define SYNC_FLAGS1_SYNCED  0
#define SYNC_FLAGS1_TRIG    1

#define SYNC_STROKE_COUNT   4

typedef struct sync_event_s
{
    // Абсолютное значение угла
    uint16_t timing;
    // Номер такта
    uint8_t stroke;
    // Номер зуба
    uint8_t cogs;
    // Относительный угол (между зубьями)
    uint8_t ang_mod;
    // Смещение номера такта
    uint8_t offset;
    // Указатель на следующее событие в очереди
    struct sync_event_s *next;

} sync_event_t;

typedef struct sync_state_s
{
    // Счётчик тактов
    uint8_t stroke;
    // Предыдущее значение счётчика времени такта
    uint16_t prev_stroke_time;
    // Число переполний счётчика времени такта
    uint8_t stroke_ovf;
    // Время такта
    uint32_t stroke_period;
    // Предыдущее значение периода такта
    uint32_t prev_stroke_period;
    // Счётчик зубов
    uint8_t cogs;
    // Предыдущее значение счётчика времени между зубьями
    uint16_t prev_cogs_time;
    // Время между зубьями
    uint16_t cogs_period;
    // Предыдущее время между зубьями
    uint16_t prev_cogs_period;
    // Мнговенное значение частоты вращения КВ
    uint16_t inst_freq;
    // Буфер числа оборотов КВ
    uint16_t freq_buf[SYNC_STROKE_COUNT];
    // Текущий индекс буфера
    uint16_t freq_head;
    // Фильтрованное значение частоты вращения КВ
    uint16_t freq;
    // Флаги 1
    uint32_t flags1;

} sync_state_t;

void sync_init(void);

void sync_freq_calc(void);

void event_queue_init(sync_event_t events[], uint8_t n, uint16_t timing);

void event_update(sync_event_t *event, uint16_t target, uint16_t step);

#endif
