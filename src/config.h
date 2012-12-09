#ifndef _CONFIG_H
#define _CONFIG_H

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
#define INJ_RPM_SCALE_SIZE      8

/* Ignition channels */
#define IGN_COUNT               8
#define IGN_VOLTAGE_SCALE_SIZE  8

/* Tables */
#define TABLE_AFR_RPM_SCALE_SIZE    16
#define TABLE_AFR_LOAD_SCALE_SIZE   16
#define TABLE_IGN_RPM_SCALE_SIZE    16
#define TABLE_IGN_LOAD_SCALE_SIZE   16
#define TABLE_VE_RPM_SCALE_SIZE     16
#define TABLE_VE_LOAD_SCALE_SIZE    16

/* Sensors */
#define TPS_FLAG_INV    0x01

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
    // Flags
    // Bit 0 : AUX_FLAG_EN    enable output
    // Bit 1 : AUX_FLAG_INV   invert output
    uint8_t flags;
    // RPM on/off
    uint16_t rpm_on;
    uint16_t rpm_off;
    // Coolant temp on/off
    int8_t ect_on;
    int8_t ect_off;
} aux_config_t;

typedef struct
{
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
    int8_t idle_ign_adv[IDLE_RPM_SCALE_SIZE];
    // Valve PID config
    pid_config_t idle_pid_config;

    /* Fuel pump */
    // Fuel pump duration after ignition switch on
    uint16_t fuel_pump_on_time;
    // Fuel pump duration after ignition switch off
    uint16_t fuel_pump_off_time;

    /* Water cooling */
    // Cooling fan switch temp
    int8_t cooling_fan_temp;
    // Cooling fan temp hysteresis
    uint8_t cooling_fan_temp_hyst;
    // Water pump fan start temp
    int8_t water_pump_temp;
    // Water pump temp hysteresis
    uint8_t water_pump_temp_hyst;

    /* Auxiliary outputs */
    aux_config_t aux[AUX_COUNT];

    /* Injectors */
    // Battery voltage scale
    uint8_t inj_voltage_scale[INJ_VOLTAGE_SCALE_SIZE];
    // Deadtime vs battery voltage
    uint16_t inj_deadtime[INJ_VOLTAGE_SCALE_SIZE];
    // Injection start phase vs RPM
    uint16_t inj_start_phase[INJ_RPM_SCALE_SIZE];
    // Individual injector config
    inj_config_t inj[INJ_COUNT];

    /* Ignition */
    uint8_t ign_min_adv;
    uint8_t ign_max_adv;
    uint8_t ign_adv_step;
    // Dwell time vs battery voltage
    uint16_t ign_dwell[IGN_VOLTAGE_SCALE_SIZE];

    /* AFR table */
    // RPM scale
    uint16_t table_afr_rpm_scale[TABLE_AFR_RPM_SCALE_SIZE];
    // Load scale
    uint16_t table_afr_load_scale[TABLE_AFR_LOAD_SCALE_SIZE];
    // Table
    int16_t table_afr[TABLE_AFR_RPM_SCALE_SIZE][TABLE_AFR_LOAD_SCALE_SIZE];

    /* Ignition timing advance table */
    // RPM scale
    uint16_t table_ign_rpm_scale[TABLE_ING_RPM_SCALE_SIZE];
    // Load scale
    uint16_t table_ign_load_scale[TABLE_IGN_LOAD_SCALE_SIZE];
    // Ignition timing advance
    int16_t table_ign[TABLE_IGN_RPM_SCALE_SIZE][TABLE_IGN_LOAD_SCALE_SIZE];

    /* Volumetric efficiency table */
    // RPM scale
    uint16_t table_ve_rpm_scale[TABLE_VE_RPM_SCALE_SIZE];
    // Load scale
    uint16_t table_ve_load_scale[TABLE_VE_LOAD_SCALE_SIZE];
    // Volumetric efficiency
    int16_t table_ve[TABLE_VE_RPM_SCALE_SIZE][TABLE_VE_LOAD_SCALE_SIZE];

    /* Sensor */
    // TP
    // 0 : TPS_FLAG_INV inverse voltage
    uint8_t tps_flags;
    uint16_t tps_min_voltage;
    uint16_t tps_max_voltage;
    // MAF
    // MAP
    int16_t maps_factor;
    int16_t maps_offset;
    uint16_t maps_min_voltage;
    uint16_t maps_max_voltage;
    // IAT
    // ECT

} config_t;

#endif
