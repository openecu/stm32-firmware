#ifndef _CONFIG_H
#define _CONFIG_H

#include "actuators.h"
#include "pid.h"

typedef struct
{
    uint8_t en: 1;
    uint8_t inv: 1;
    uint16_t rpm_on;
    uint16_t rpm_off;
    int8_t ect_on;
    int8_t ect_off;
} aux_t;

typedef struct
{
    /* Cranking */
    #define CRANK_RPM_SCALE_SIZE    8
    #define CRANK_TEMP_SCALE_SIZE   8
    // Cranking threshold RPM
    uint16_t crank_thres_rpm;
    // RPM scale
    int8_t crank_rpm_scale[CRANK_RPM_SCALE_SIZE];
    // Engine coolant temp scale
    uint16_t crank_temp_scale[CRANK_TEMP_SCALE_SIZE];
    // Injector pulse width vs RPM
    uint16_t crank_pw[CRANK_RPM_SCALE_SIZE];
    // Enrichment vs coolant temp
    int16_t crank_enrich[CRANK_TEMP_SCALE_SIZE];

    /* After start and Warm-Up */
    #define AFTERSTART_TEMP_SCALE_SIZE  8
    #define WARMUP_TEMP_SCALE_SIZE  8
    // Warm-Up enrichment vs coolant temp
    int16_t afterstart_enrich[AFTERSTART_TEMP_SCALE_SIZE];
    // Warm-Up enrichment vs coolant temp
    int16_t warmup_enrich[WARMUP_TEMP_SCALE_SIZE];

    /* Idling */
    #define IDLE_TEMP_SCALE_SIZE    8
    #define IDLE_RPM_SCALE_SIZE     8
    // Coolant temp scale
    int8_t idle_temp_scale[IDLE_TEMP_SCALE_SIZE];
    // RPM scale
    uint16_t idle_rpm_scale[IDLE_RPM_SCALE_SIZE];
    // Target RPM vs coolant temp
    uint16_t idle_rpm[IDLE_TEMP_SCALE_SIZE];
    // Timing advance vs RPM
    int8_t idle_timing_adv[IDLE_RPM_SCALE_SIZE];
    // Valve PID config
    pid_config_t idle_pid_config;

    /* Fuel pump */
    // Fuel pump duration after ignition switch on
    uint16_t fuel_pump_on_time;
    // Fuel pump duration after ignition switch off
    uint16_t fuel_pump_off_time;

    /* Water cooling */
    /* Cooling fan switch temp */
    int8_t cooling_fan_temp;
    /* Cooling fan temp hysteresis */
    uint8_t cooling_fan_temp_hyst;
    /* Water pump fan start temp */
    int8_t water_pump_temp;
    /* Water pump temp hysteresis */
    uint8_t water_pump_temp_hyst;

    /* Auxiliary outputs */
    aux_t aux[AUX_COUNT];
} config_t;

#endif
