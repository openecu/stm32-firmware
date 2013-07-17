#include "status.h"
#include "config.h"
#include "sync.h"
#include "ignition.h"
#include "injection.h"
#include "idle.h"
#include "comm.h"

status_t status;
uint8_t sync_psc = 100;

int main(void)
{
    RCC->AHB1ENR |= (RCC_AHB1ENR_GPIOAEN | RCC_AHB1ENR_GPIOBEN | RCC_AHB1ENR_GPIOCEN 
        | RCC_AHB1ENR_GPIODEN | RCC_AHB1ENR_GPIOEEN | RCC_AHB1ENR_DMA1EN);
    RCC->APB1ENR |= (RCC_APB1ENR_USART2EN | RCC_APB1ENR_TIM2EN | RCC_APB1ENR_TIM13EN 
        | RCC_APB1ENR_TIM6EN | RCC_APB1ENR_TIM7EN);
    RCC->APB2ENR |= (RCC_APB2ENR_USART1EN | RCC_APB2ENR_TIM10EN | RCC_APB2ENR_TIM9EN 
        | RCC_APB2ENR_TIM1EN | RCC_APB2ENR_ADC1EN);

    /* Independed watchdog */
    /*IWDG->KR    = 0x5555;
    IWDG->PR    = 0;
    IWDG->RLR   = 0xFF;
    IWDG->KR    = 0xAAAA;
    IWDG->KR    = 0xCCCC;*/

    /* Loop timer */
    TIM7->PSC   = 4999;
    TIM7->ARR   = 1;
    TIM7->DIER  |= TIM_DIER_UIE;
    TIM7->CR1   |= (TIM_CR1_URS | TIM_CR1_CEN);

    NVIC_SetPriority(TIM7_IRQn, 5);
    NVIC_EnableIRQ(TIM7_IRQn);

    /* LED */
    GPIOD->MODER    |= (GPIO_MODER_MODER12_0 | GPIO_MODER_MODER13_0 
        | GPIO_MODER_MODER14_0 | GPIO_MODER_MODER15_0);
    GPIOD->OSPEEDR  |= (GPIO_OSPEEDER_OSPEEDR12_0 | GPIO_OSPEEDER_OSPEEDR13_0 
        | GPIO_OSPEEDER_OSPEEDR14_0 | GPIO_OSPEEDER_OSPEEDR15_0);

    /* Sync emulation (debug) */
    GPIOD->MODER    |= (GPIO_MODER_MODER0_0 | GPIO_MODER_MODER1_0);
    GPIOD->OSPEEDR  |= (GPIO_OSPEEDER_OSPEEDR0_0 | GPIO_OSPEEDER_OSPEEDR1_0);

    TIM6->PSC   = 2499;
    TIM6->ARR   = 10;
    TIM6->DIER  |= TIM_DIER_UIE;
    TIM6->CR1   |= (TIM_CR1_URS | TIM_CR1_CEN);

    NVIC_SetPriority(TIM6_DAC_IRQn, 15);
    NVIC_EnableIRQ(TIM6_DAC_IRQn);

    /* Init I/O */
    sync_init();
    /*inj_init();
    ign_init();
    idle_init();*/
    comm_init();

    /* ADC */
    GPIOB->MODER |= GPIO_MODER_MODER0;

    // 10-bit
    ADC1->CR1   |= ADC_CR1_RES_0;
    ADC1->SQR3  |= ADC_SQR3_SQ1_3;
    ADC1->CR2   |= ADC_CR2_ADON;

    __enable_irq();

    for (;;)
    {
        //IWDG->KR = 0xAAAA;

        inj_deadtime_calc();
        inj_afr_calc();
        ign_dwell_calc();
        ign_timing_calc();

        uint16_t c;
        c = uart_getc();

        if (!(c & UART_NO_DATA))
        {
            sync_psc = c;
        }
    }

    return 0;
}

/**
 * Loop timer ISR
 */
void TIM7_IRQHandler(void)
{
    if ((TIM7->SR & TIM_SR_UIF))
    {
	    static uint8_t ovf2;
	    static uint8_t ovf1;

        TIM7->SR = ~TIM_SR_UIF;
	    //IWDG->KR = 0xAAAA;

	    // 1 kHz loop
	    if ((++ovf1) == 10)
	    {
	        ovf1 = 0;

	        // 100 Hz loops
	        if ((++ovf2) == 10)
	        {
	            ovf2 = 0;

                ADC1->CR2 |= ADC_CR2_SWSTART;
                while (!(ADC1->SR & ADC_SR_EOC));
                //uart_putc(ADC1->DR >> 2);
	        }

	        switch (ovf2)
	        {
	            case 0:
	            case 5:
	                idle_control();
	                break;
	        }

		    // Communication
		    if (TSTBIT(status.comm.flags1, COMM_FLAGS1_READY))
		    {
		        CLRBIT(status.comm.flags1, COMM_FLAGS1_READY);
                //DMA1_Stream6->CR |= DMA_SxCR_EN;
		    }
	    }

	    // Stroke loop
	    if (TSTBIT(status.flags1, FLAGS1_STROKE))
	    {
	        CLRBIT(status.flags1, FLAGS1_STROKE);

	        sync_freq_calc();
	        inj_pw_calc();
	        idle_ign_timing_adjust();
	    }
	}
}

/** 
 * Sync emulation timer ISR
 *
 * For debugging purposes only.
 */
void TIM6_DAC_IRQHandler(void)
{
    if ((TIM6->SR & TIM_SR_UIF))
    {
        static uint16_t sync_pos;
        static uint16_t sync_ref;

        TIM6->SR = ~TIM_SR_UIF;

        if (
            ((sync_ref == 0) && (sync_pos == (179-16)))
            || ((sync_ref == 1) && (sync_pos == (179-12)))
            || ((sync_ref == 2) && (sync_pos == (179-8)))
            || ((sync_ref == 3) && (sync_pos == (179-4)))
        )
        {
            GPIOD->ODR |= GPIO_ODR_ODR_0;
        }

        sync_pos++;
        GPIOD->ODR ^= GPIO_ODR_ODR_1;

        if (sync_pos == 180)
        {
            sync_pos = 0;
            sync_ref++;

            if (sync_ref == 4)
            {
                sync_ref = 0;
            }

            GPIOD->ODR &= ~GPIO_ODR_ODR_0;
        }
    }
}
