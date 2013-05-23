#include <stm32f4xx.h>
#include "ecu.h"
#include "calc.h"
#include "injection.h"
#include "ignition.h"
#include "switch.h"
#include "actuators.h"
#include "comm.h"

ecu_t ecu;

void TIM7_IRQHandler(void)
{
    if ((TIM7->SR & TIM_SR_UIF))
    {
        TIM7->SR &= ~TIM_SR_UIF;
        ecu.status.mtime++;
    }
}

int main(void)
{
    __disable_irq();

    RCC->APB1ENR |= RCC_APB1ENR_TIM7EN;
    RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
    RCC->AHB1ENR |= (RCC_AHB1ENR_GPIOBEN | RCC_AHB1ENR_GPIODEN | RCC_AHB1ENR_GPIOGEN | RCC_AHB1ENR_DMA2EN);

    /* Independed watchdog */
    IWDG->KR = 0x5555;
    IWDG->PR = 0;
    IWDG->RLR = 0xFF;
    IWDG->KR = 0xAAAA;
    IWDG->KR = 0xCCCC;

    /* Timer 7 */
    TIM7->PSC = 50;
    TIM7->ARR = 1000;
    TIM7->CNT = 0;
    TIM7->DIER |= TIM_DIER_UIE;
    TIM7->SR &= ~TIM_SR_UIF;
    TIM7->CR1 |= (TIM_CR1_URS | TIM_CR1_CEN);

    NVIC_SetPriority(TIM7_IRQn, 15);
    NVIC_EnableIRQ(TIM7_IRQn);

    switch_init();
    actuators_init();

    ecu.status.mtime = 0;
    ecu.config.fuel_pump_prime_time = 2000;

    __enable_irq();

    uint32_t mtime, mtime1, dtime1;
    uint8_t execount;

    mtime1 = 0;
    execount = 0;

    switch_update();

    for (;;)
    {
        IWDG->KR = 0xAAAA;

        main_relay();

        if ((ecu.status.flags2 & STATUS_FLAGS2_START_SW))
        {
            ecu.status.flags1 |= STATUS_FLAGS1_CRANK;
        }
        else
        {
            ecu.status.flags1 &= ~STATUS_FLAGS1_CRANK;   
        }

        /* События на каждый новый такт двигателя */
        if ((ecu.status.flags1 & STATUS_FLAGS1_STROKE))
        {
            ecu.status.flags1 &= ~STATUS_FLAGS1_STROKE;
            calc_rpm();
        }

        /* События с заданным интервалом */
        mtime = ecu.status.mtime;
        dtime1 = (mtime1 > mtime) ? (mtime1 - mtime) : (mtime - mtime1);

        // Каждые 10 мс
        if (dtime1 >= 10)
        {
            mtime1 = mtime;

            fuel_pump(dtime1);
            accel_enrich(dtime1);
            switch_update();
        }

        /* 
            События на каждую итерацию.
            Часть событий распределена по итерациям для более точного выполнения
            событий с заданным интервалом.
        */
        calc_load();

        //if (execount == 1)
        //{
            warmup_enrich();
            inj_afr();
            inj_trim();
            inj_timing();
            inj_calc_pw();
        //} 
        //else if (execount == 2)
        //{
            ign_dwell();
            ign_timing();
        //}
        //else if (execount == 3)
        //{
            cooling_fan();
            vvt();
        //}
        //else if (execount == 4)
        //{
            comm();
            //execount = 0;
        //}

        //execount++;
    }

    return 0;
}
