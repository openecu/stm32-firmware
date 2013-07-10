#ifndef CONFIG_H
#define CONFIG_H

#include "cmsis/stm32f4xx.h"
#include "pid.h"

#define SETBIT(addr, bit)       (*(vu32 *)(SRAM1_BB_BASE | ((((u32)&addr) - SRAM1_BASE) << 5) | ((bit) << 2)) = 1)    
#define CLEARBIT(addr, bit)     (*(vu32 *)(SRAM1_BB_BASE | ((((u32)&addr) - SRAM1_BASE) << 5) | ((bit) << 2)) = 0)    
#define TESTBIT(addr, bit)      (*(vu32 *)(SRAM1_BB_BASE | ((((u32)&addr) - SRAM1_BASE) << 5) | ((bit) << 2)))

/* Injection */
#define CONF_INJ_COUNT	4
#define CONF_INJ_DEAD_SIZE	16
#define CONF_INJ_AFR_RPM_SIZE 	20
#define CONF_INJ_AFR_LOAD_SIZE	20
#define CONF_INJ_TIMING_RPM_SIZE	20
#define CONF_INJ_TIMING_LOAD_SIZE	20
#define CONF_INJ_TRIM_RPM_SIZE	20
#define CONF_INJ_TRIM_LOAD_SIZE	20

/* Ignition */
#define CONF_IGN_DWELL_SIZE	16
#define CONF_IGN_TIMING_RPM_SIZE 	20
#define CONF_IGN_TIMING_LOAD_SIZE	20

/* Idle */
#define CONF_IDLE_RPM_SIZE	16
#define CONF_IDLE_IGN_TIMING_SIZE	16
#define CONF_IDLE_IGN_OFFSET_SIZE	16

/* Cranking */
#define CONF_CRANK_INJ_PW_SIZE	16
#define CONF_CRANK_INJ_TIMING_SIZE	16
#define CONF_CRANK_IGN_TIMING_SIZE	16

/* Sensors */
#define CONF_MAFS_CONV_SIZE	64
#define CONF_ECTS_CONV_SIZE	32

typedef struct
{
	/* Injection */
	// Injector flow
	uint16_t inj_flow[CONF_INJ_COUNT];
	// Injector deadtime
	uint16_t inj_dead[CONF_INJ_DEAD_SIZE];
	// Injection AFR
	int16_t inj_afr[CONF_INJ_AFR_RPM_SIZE * CONF_INJ_AFR_LOAD_SIZE];
	// Injection AFR rpm
	uint16_t inj_afr_rpm[CONF_INJ_AFR_RPM_SIZE];
	// Injection AFR load
	uint16_t inj_afr_load[CONF_INJ_AFR_LOAD_SIZE];
	// Injection timing
	uint16_t inj_timing[CONF_INJ_TIMING_RPM_SIZE * CONF_INJ_TIMING_LOAD_SIZE];
	// Injection timing rpm
	uint16_t inj_timing_rpm[CONF_INJ_TIMING_RPM_SIZE];
	// Injection timing load
	uint16_t inj_timing_load[CONF_INJ_TIMING_LOAD_SIZE];
	// Injection trim
	uint16_t inj_trim[CONF_INJ_TRIM_RPM_SIZE * CONF_INJ_TRIM_LOAD_SIZE];
	// Injection trim rpm
	uint16_t inj_trim_rpm[CONF_INJ_TRIM_RPM_SIZE];
	// Injection trim load
	uint16_t inj_trim_load[CONF_INJ_TRIM_LOAD_SIZE];

	/* Ignition*/
	// Ignition dwell
	uint16_t ign_dwell[CONF_IGN_DWELL_SIZE];
	// Ignition timing
	int16_t ign_timing[CONF_IGN_TIMING_RPM_SIZE * CONF_IGN_TIMING_LOAD_SIZE];
	// Ignition timing rpm
	uint16_t ign_timing_rpm[CONF_IGN_TIMING_RPM_SIZE];
	// Ignition timing load
	uint16_t ign_timing_load[CONF_IGN_TIMING_LOAD_SIZE];

	/* Idle */
	// Idle PID
	pid_config_t idle_pid;
    // Idle pwm frequency divider
    uint16_t idle_pwm_frq_div;
    // Idle pwm duty maximum
    uint16_t idle_pwm_duty_max;
    // Idle pwm duty minimum
    uint16_t idle_pwm_duty_min;
	// Idle target rpm
	uint16_t idle_rpm[CONF_IDLE_RPM_SIZE];
    // Idle target rpm ect
    int16_t idle_rpm_ect[CONF_IDLE_RPM_SIZE];
	// Idle ignition timing
	int16_t idle_ign_timing[CONF_IDLE_IGN_TIMING_SIZE];
	// Idle ignition timing rpm
	int16_t idle_ign_timing_rpm[CONF_IDLE_IGN_TIMING_SIZE];
	// Idle ignition timing offset
	int16_t idle_ign_offset[CONF_IDLE_IGN_OFFSET_SIZE];
	// Idle ignition timing offset rpm
	int16_t idle_ign_offset_rpm[CONF_IDLE_IGN_OFFSET_SIZE];
	// Idle recovery rpm threshold
	uint16_t idle_rec_rpm_thres;

	/* Cranking */
	// Crank injection pulse width
	uint16_t crank_inj_pw[CONF_CRANK_INJ_PW_SIZE];
	// Crank injection timing
	uint16_t crank_inj_timing[CONF_CRANK_INJ_TIMING_SIZE];
	// Crank ignition timing
	int16_t crank_ign_timing[CONF_CRANK_IGN_TIMING_SIZE];

	/* Sensors */
	// MAF sensor conversion
	uint16_t mafs_conv[CONF_MAFS_CONV_SIZE];
	// MAF sensor flow
	uint16_t mafs_flow;
	// MAF sensor maximum voltage
	uint16_t mafs_max_v;
	// MAF sensor minimum voltage
	uint16_t mafs_min_v;
	// TP sensor closed voltage
	uint16_t tps_closed_v;
	// TP sensor opened voltage
	uint16_t tps_opened_v;
	// ECT sensor conversion
	int16_t ects_conv[CONF_ECTS_CONV_SIZE];
	// ECT sensor maximum voltage
	uint16_t ects_max_v;
	// ECT sensor minimum voltage
	uint16_t ects_min_v;
} config_t;

extern config_t config;

#endif
