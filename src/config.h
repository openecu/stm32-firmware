#ifndef _CONFIG_H
#define _CONFIG_H

#include "actuators.h"
#include "pid.h"

/* Cranking */
#define CRANK_TEMP_SCALE_SIZE   8

/* Afterstart and Warm-Up */
#define AFTERSTART_TEMP_SCALE_SIZE  8
#define WARMUP_TEMP_SCALE_SIZE      8

/* Idling */
#define IDLE_TEMP_SCALE_SIZE    8
#define IDLE_RPM_SCALE_SIZE     8

/* Auxiliary outputs */
#define AUX_COUNT       8
#define AUX_FLAG_EN     0x01
#define AUX_FLAG_INV    0x02

/* Injectors */
#define INJ_COUNT               8
#define INJ_VOLTAGE_SCALE_SIZE  8

/* Ignition channels */
#define IGN_COUNT               8
#define IGN_VOLTAGE_SCALE_SIZE  8

typedef struct
{
    uint8_t flags;
    uint16_t rpm_on;
    uint16_t rpm_off;
    int8_t ect_on;
    int8_t ect_off;
} aux_config_t;

typedef struct
{
    // Deadtime vs battery voltage
    uint16_t deadtime[INJ_VOLTAGE_SCALE_SIZE];
    // Multiplier
    uint16_t multiplier;
} inj_config_t;

typedef struct
{
    /* Cranking */
    // Cranking threshold RPM
    uint16_t crank_thres_rpm;
    // Engine coolant temp scale
    int8_t crank_temp_scale[CRANK_TEMP_SCALE_SIZE];
    // Injector pulse width vs coolant temp
    uint16_t crank_pw[CRANK_TEMP_SCALE_SIZE];
    // Enrichment vs coolant temp
    //int16_t crank_enrich[CRANK_TEMP_SCALE_SIZE];
    // Ignition timing advance vs coolant temp
    int8_t crank_ign_adv[CRANK_TEMP_SCALE_SIZE];

    /* Afterstart and Warm-Up */
    // Afterstart enrichment vs coolant temp
    int16_t afterstart_enrich[AFTERSTART_TEMP_SCALE_SIZE];
    // Afterstart enrichment decay
    uint8_t afterstart_enrich_decay;
    // Warm-Up enrichment vs coolant temp
    int16_t warmup_enrich[WARMUP_TEMP_SCALE_SIZE];

    /* Idling */
    // Coolant temp scale
    int8_t idle_temp_scale[IDLE_TEMP_SCALE_SIZE];
    // RPM scale
    uint16_t idle_rpm_scale[IDLE_RPM_SCALE_SIZE];
    // Target RPM vs coolant temp
    uint16_t idle_rpm[IDLE_TEMP_SCALE_SIZE];
    // Ignition timing advance vs RPM
    int8_t idle_ign_adv[IDLE_RPM_SCALE_SIZE];
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
    aux_config_t aux[AUX_COUNT];

    /* Injectors */
    // Battery voltage scale size
    uint8_t inj_voltage_scale[INJ_VOLTAGE_SCALE_SIZE];
    // Individual injectors config
    inj_config_t injectors[INJ_COUNT];

    /* Ignition channels */
    // Dwell time vs battery voltage
    uint16_t ign_dwell[IGN_VOLTAGE_SCALE_SIZE];

} config_t;

#endif
