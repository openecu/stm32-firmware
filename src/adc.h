#ifndef ADC_H
#define ADC_H

#define ADC_CH_MAF  0
#define ADC_CH_TP   1
#define ADC_CH_ECT  2

#define ADC_MAF_BUF_SIZE    4
#define ADC_TP_BUF_SIZE     4
#define ADC_ECT_BUF_SIZE    16

typedef struct
{
    uint16_t maf_buf[ADC_MAF_BUF_SIZE];
    uint16_t tp_buf[ADC_TP_BUF_SIZE];
    uint16_t ect_buf[ADC_ECT_BUF_SIZE];

    uint16_t maf;
    uint16_t tp;
    uint16_t ect;

} adc_state_t;

void adc_init(void);

void adc_first_measure(void);

void adc_conv_sequence(void);

#endif
