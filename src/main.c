#include "status.h"
#include "config.h"
#include "sync.h"
#include "ignition.h"
#include "injection.h"
#include "idle.h"
#include "comm.h"

status_t status;

int main(void)
{
    RCC->AHB1ENR |= (RCC_AHB1ENR_GPIOAEN | RCC_AHB1ENR_GPIOBEN | RCC_AHB1ENR_GPIOCEN 
        | RCC_AHB1ENR_GPIODEN | RCC_AHB1ENR_GPIOEEN | RCC_AHB1ENR_DMA1EN);
    RCC->APB1ENR |= (RCC_APB1ENR_USART2EN | RCC_APB1ENR_TIM2EN | RCC_APB1ENR_TIM13EN 
        | RCC_APB1ENR_TIM6EN | RCC_APB1ENR_TIM7EN);
    RCC->APB2ENR |= (RCC_APB2ENR_USART1EN | RCC_APB2ENR_TIM10EN | RCC_APB2ENR_TIM9EN 
        | RCC_APB2ENR_TIM1EN);

    /* Independed watchdog */
    IWDG->KR = 0x5555;
    IWDG->PR = 0;
    IWDG->RLR = 0xFF;
    IWDG->KR = 0xAAAA;
    IWDG->KR = 0xCCCC;

    /* Loop timer */
    TIM7->PSC = 8400;
    TIM7->ARR = 1;
    TIM7->DIER |= TIM_DIER_UIE;
    TIM7->CR1 |= (TIM_CR1_URS | TIM_CR1_CEN);

    NVIC_SetPriority(TIM7_IRQn, 5);
    NVIC_EnableIRQ(TIM7_IRQn);

    /* LED */
    GPIOD->MODER |= (GPIO_MODER_MODER12_0 | GPIO_MODER_MODER13_0 
        | GPIO_MODER_MODER14_0 | GPIO_MODER_MODER15_0);
    GPIOD->OSPEEDR |= (GPIO_OSPEEDER_OSPEEDR12_0 | GPIO_OSPEEDER_OSPEEDR13_0 
        | GPIO_OSPEEDER_OSPEEDR14_0 | GPIO_OSPEEDER_OSPEEDR15_0);

    /* Sync emulation (debug) */
    GPIOD->MODER |= (GPIO_MODER_MODER0_0 | GPIO_MODER_MODER1_0);
    GPIOD->OSPEEDR |= (GPIO_OSPEEDER_OSPEEDR0_0 | GPIO_OSPEEDER_OSPEEDR1_0);

    TIM6->PSC = 2500;
    TIM6->ARR = 1;
    TIM6->DIER |= TIM_DIER_UIE;
    TIM6->CR1 |= (TIM_CR1_URS | TIM_CR1_CEN);

    NVIC_SetPriority(TIM6_DAC_IRQn, 15);
    NVIC_EnableIRQ(TIM6_DAC_IRQn);

    /* Init I/O */
    sync_init();
    inj_init();
    ign_init();
    idle_init();
    comm_init();

    __enable_irq();

    for (;;)
    {
        //IWDG->KR = 0xAAAA;

        inj_deadtime_calc();
        inj_afr_calc();
        ign_dwell_calc();
        ign_timing_calc();
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
	    IWDG->KR = 0xAAAA;

	    // 1 kHz loop
	    if ((++ovf1) == 10)
	    {
	        ovf1 = 0;

	        // 100 Hz loops
	        if ((++ovf2) == 10)
	        {
	            ovf2 = 0;
	        }

	        switch (ovf2)
	        {
	            case 0:
	            case 5:
	                idle_control();
	                break;
	        }
	    }

	    // Stroke loop
	    if (TESTBIT(status.flags1, FLAGS1_STROKE))
	    {
	        CLEARBIT(status.flags1, FLAGS1_STROKE);

	        sync_freq_calc();
	        inj_pw_calc();
	        idle_ign_timing_adjust();
	    }

	    /*if (TESTBIT(status.comm.flags1, COMM_FLAGS1_READY))
	    {
	        CLEARBIT(status.comm.flags1, COMM_FLAGS1_READY);
	        DMA1_Stream6->CR |= DMA_SxCR_EN;
	    }*/
	}
}

/** 
 * Sync emulation timer ISR
 *
 * For debugging purposes only.
 */
uint16_t tim_div100;
uint16_t tim_div1000;
uint8_t sync_div;
uint16_t sync_pos;
uint8_t sync_ref;

void TIM6_DAC_IRQHandler(void)
{
    if ((TIM6->SR & TIM_SR_UIF))
    {
        TIM6->SR = ~TIM_SR_UIF;

        if ((++sync_div) == 5)
        {
            sync_div = 0;

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

                GPIOD->ODR |= GPIO_ODR_ODR_0;
            }

            if ((sync_ref == 0) && (sync_pos == 16))
            {
                GPIOD->ODR &= ~GPIO_ODR_ODR_0;            
            }
            else if ((sync_ref == 1) && (sync_pos == 12))
            {
                GPIOD->ODR &= ~GPIO_ODR_ODR_0;            
            }
            else if ((sync_ref == 2) && (sync_pos == 8))
            {
                GPIOD->ODR &= ~GPIO_ODR_ODR_0;            
            }
            else if ((sync_ref == 3) && (sync_pos == 4))
            {
                GPIOD->ODR &= ~GPIO_ODR_ODR_0;            
            }
        }

        if ((++tim_div100) == 100)
        {
            tim_div100 = 0;
        }

        if ((++tim_div1000) == 1000)
        {
            tim_div1000 = 0;
            //GPIOD->ODR ^= GPIO_ODR_ODR_12;
        }
    }
}
