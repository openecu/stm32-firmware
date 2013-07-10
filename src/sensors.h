#ifndef SENSORS_H
#define SENSORS_H

#include "cmsis/stm32f4xx.h"

#define ADC_CH_MAF  0
#define ADC_CH_TP   1
#define ADC_CH_ECT  2

#define ADC_MAF_BUF_SIZE    4
#define ADC_TP_BUF_SIZE     4
#define ADC_ECT_BUF_SIZE    16

typedef struct
{
	// MAF sensor buffer
    uint16_t maf_buf[ADC_MAF_BUF_SIZE];
    // TP sensor buffer
    uint16_t tp_buf[ADC_TP_BUF_SIZE];
    // ECT sensor buffer
    uint16_t ect_buf[ADC_ECT_BUF_SIZE];
    // MAF sensor
    uint16_t maf;
    // TP sensor
    uint16_t tp;
    // ECT sensor
    uint16_t ect;

} adc_state_t;

typedef struct
{
	// ADC state
	adc_state_t adc;

} sens_state_t;

void sens_init(void);

#endif
