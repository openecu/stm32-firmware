#ifndef _CONFIG_H
#define _CONFIG_H

/* Cranking */
#define CRANK_TEMP_SCALE_SIZE   8
#define CRANK_CYCLE_SCALE_SIZE  8

/* Afterstart and Warm-Up */
#define AFTERSTART_TEMP_SCALE_SIZE  8
#define WARMUP_TEMP_SCALE_SIZE      8

/* Idling */
#define IDLE_TEMP_SCALE_SIZE    8
#define IDLE_RPM_SCALE_SIZE     8

/* Injectors */
#define INJ_COUNT               4
#define INJ_VOLTAGE_SCALE_SIZE  8
#define INJ_RPM_SCALE_SIZE      8

/* Ignition channels */
#define IGN_COUNT               1
#define IGN_VOLTAGE_SCALE_SIZE  8

/* Tables */
#define TABLE_VE_RPM_SCALE_SIZE     16
#define TABLE_VE_LOAD_SCALE_SIZE    16

/* Sensors */
#define MAFS_VOLTAGE_SCALE_SIZE 64
#define ECTS_VOLTAGE_SCALE_SIZE 64

typedef struct
{
    // Proportional gain
    uint8_t kp;
    // Integral gain
    uint8_t ki;
    // Differential gain
    uint8_t kd;
    // Maximum integral sum
    uint32_t integral_max;
} pid_config_t;

typedef struct
{
    /* Cranking */
    // Cranking threshold RPM
    uint16_t crank_thres_rpm;
    // Engine coolant temp scale
    int8_t crank_temp_scale[CRANK_TEMP_SCALE_SIZE];
    // Base load vs coolant temp
    uint16_t crank_base_load[CRANK_TEMP_SCALE_SIZE];
    //
    uint16_t crank_cycle_scale[CRANK_CYCLE_SCALE_SIZE];
    //
    int16_t crank_cycle_enrich[CRANK_CYCLE_SCALE_SIZE];
    //
    uint16_t crank_inj_angle;
    // Ignition timing advance vs coolant temp
    int8_t crank_ign_adv[CRANK_TEMP_SCALE_SIZE];

    /* Afterstart and Warm-Up */
    // Afterstart enrichment vs coolant temp
    int16_t afterstart_enrich[AFTERSTART_TEMP_SCALE_SIZE];
    // Afterstart enrichment decay vs coolant temp
    uint16_t afterstart_enrich_decay[AFTERSTART_TEMP_SCALE_SIZE];
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
    int16_t idle_ign_adv[IDLE_RPM_SCALE_SIZE];
    // Target AFR
    uint16_t idle_afr;
    // Valve PID config
    pid_config_t idle_pid_config;

    /* Fuel pump */
    // Fuel pump duration after ignition switch on
    uint16_t fuel_pump_on_time;
    // Fuel pump duration after engine stop
    uint16_t fuel_pump_off_time;

    /* Water cooling */
    // Cooling fan switch temp
    int8_t cooling_fan_temp;
    // Cooling fan temp hysteresis
    uint8_t cooling_fan_temp_hyst;

    // Battery voltage scale
    uint8_t inj_voltage_scale[INJ_VOLTAGE_SCALE_SIZE];
    // Deadtime vs battery voltage
    uint16_t inj_deadtime[INJ_VOLTAGE_SCALE_SIZE];
    // Injection start angle vs RPM
    uint16_t inj_angle[INJ_RPM_SCALE_SIZE];
    // Maximum pulsewidth vs RPM
    uint16_t inj_max_pw[INJ_RPM_SCALE_SIZE];
    // Injector multiplier
    uint16_t inj_mult;

    /* Ignition */
    // Minimum timing advance
    uint8_t ign_min_adv;
    // Maximum timing advance
    uint8_t ign_max_adv;
    // Limit timing advance change step
    uint8_t ign_adv_step;
    // Dwell time vs battery voltage
    uint16_t ign_dwell[IGN_VOLTAGE_SCALE_SIZE];

    /* Volumetric efficiency table */
    // RPM scale
    uint16_t table_ve_rpm_scale[TABLE_VE_RPM_SCALE_SIZE];
    // Load scale
    uint16_t table_ve_load_scale[TABLE_VE_LOAD_SCALE_SIZE];
    // Volumetric efficiency
    int16_t table_ve[TABLE_VE_RPM_SCALE_SIZE][TABLE_VE_LOAD_SCALE_SIZE];

    /* Sensors */
    // Throttle position sensor
    // Minimum voltage
    uint16_t tps_min_voltage;
    // Maximum voltage
    uint16_t tps_max_voltage;

    // Mass air flow sensor
    // Minimum voltage
    uint16_t mafs_min_voltage;
    // Maximum voltage
    uint16_t mafs_max_voltage;
    // Mass air flow vs output voltage
    uint16_t mafs_calibration[MAFS_VOLTAGE_SCALE_SIZE];

    // Engine coolant temp sensor
    // Minimum voltage
    uint16_t ects_min_voltage;
    // Maximum voltage
    uint16_t ects_max_voltage;
    // Mass air flow vs output voltage
    uint16_t ects_calibration[ECTS_VOLTAGE_SCALE_SIZE];

} config_t;

#endif
