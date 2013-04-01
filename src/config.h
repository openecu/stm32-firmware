#ifndef _CONFIG_H
#define _CONFIG_H
    
#include <stdint.h>

typedef struct config_s
{
    // Время работы бензонасоса при включении зажигания
    uint16_t fuel_pump_prime_time;

    // Температура включения вентилятор системы охлаждения
    int16_t fan_ect_thres;
    // Гистерезис для выключения вентилятора
    uint8_t fan_ect_hyst;

    // Обороты включения клапана VVT
    uint16_t vvt_rpm_thres;
    // Гистерезис для выключения клапана VVT
    uint16_t vvt_rpm_hyst;

    // Обогащение в зависимости от температуры ОЖ
    uint16_t warmup_enrich[16];

    // Порог скорости открытия дроссельной заслонки для активации обогащения
    uint16_t accel_enrich_delta_thres;
    // Задержка перед уменьшением обогащения
    uint16_t accel_enrich_delay;
    // Скорость уменьшения обогащения
    uint16_t accel_enrich_decay;
    // Обогащение в зависимости от скорости открытия дроссельной заслонки
    uint16_t accel_delta_enrich[16];
    // Коррекция обогащения в зависимости от оборотов ДВС
    uint16_t accel_rpm_enrich[16];
    // Коррекция обогащения в зависимости от температуры ОЖ
    uint16_t accel_ect_enrich[16];

    // Множитель впрыска
    uint16_t inj_mult;
    // Лаг форсунки
    uint16_t inj_deadtime[16];
    //
    int16_t inj_afr[256];
    //
    int16_t inj_trim[256];
    //
    uint16_t inj_timing[256];

    // Наполнение в режиме пуска
    uint16_t crank_load[16];
    // Базовый состав смеси в режиме пуска
    uint16_t crank_afr;

} config_t;

#endif
