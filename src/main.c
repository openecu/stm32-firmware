#include <stm32f4xx.h>
#include "ecu.h"
#include "calc.h"
#include "injection.h"
#include "ignition.h"
#include "switch.h"
#include "actuators.h"
#include "comm.h"

ecu_t ecu;

#define UART_RX_BUF_SIZE    128
#define UART_TX_BUF_SIZE    128
#define UART_TX_BUF_MASK    (UART_TX_BUF_SIZE - 1)
#define UART_RX_BUF_MASK    (UART_RX_BUF_SIZE - 1)

#define UART_NO_DATA    0x0100

uint8_t rx_buf[UART_RX_BUF_SIZE];
uint8_t tx_buf[UART_TX_BUF_SIZE];
volatile uint16_t rx_head;
volatile uint16_t rx_tail;
volatile uint16_t tx_head;
volatile uint16_t tx_tail;

void uart_putc(uint8_t c)
{
    uint16_t tmphead;

    tmphead = (tx_head + 1) & UART_TX_BUF_MASK;
    tx_buf[tmphead] = c;
    tx_head = tmphead;

    USART1->CR1 |= USART_CR1_TXEIE;
}

uint16_t uart_getc(void)
{
    uint16_t data, tmptail;

    if (rx_tail == rx_head)
    {
        data = UART_NO_DATA;
    } 
    else 
    {
        tmptail = (rx_tail + 1) & UART_RX_BUF_MASK;
        rx_tail = tmptail;
        data = rx_buf[tmptail];
    }

    return data;
}

void TIM7_IRQHandler(void)
{
    if ((TIM7->SR & TIM_SR_UIF))
    {
        TIM7->SR &= ~TIM_SR_UIF;
        ecu.status.mtime++;
    }
}

void USART1_IRQHandler(void)
{
    if ((USART1->SR & USART_SR_RXNE))
    {
        uint8_t data, tmphead;

        USART1->SR &= ~USART_SR_RXNE;

        data = USART1->DR;
        tmphead = (rx_head + 1) & UART_RX_BUF_MASK;

        if (rx_tail == tmphead)
        {
            // Buffer overflow
        }
        else
        {
            rx_head = tmphead;
            rx_buf[tmphead] = data;
        }
    }

    if ((USART1->SR & USART_SR_TXE))
    {
        uint16_t tmptail;

        USART1->SR &= ~USART_SR_TXE;

        if (tx_tail != tx_head)
        {
            tmptail = (tx_tail + 1) & UART_TX_BUF_MASK;
            tx_tail = tmptail;
            USART1->DR = tx_buf[tmptail];
        } 
        else
        {
            USART1->CR1 &= ~USART_CR1_TXEIE;
        }
    }
}

int main(void)
{
    __disable_irq();

    ecu.status.mtime = 0;

    RCC->APB1ENR |= RCC_APB1ENR_TIM7EN;
    RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
    RCC->AHB1ENR |= (RCC_AHB1ENR_GPIOBEN | RCC_AHB1ENR_GPIODEN | RCC_AHB1ENR_GPIOGEN | RCC_AHB1ENR_DMA2EN);

    /* Timer 7 */
    TIM7->PSC = 50;
    TIM7->ARR = 1000;
    TIM7->CNT = 0;
    TIM7->DIER |= TIM_DIER_UIE;
    TIM7->SR &= ~TIM_SR_UIF;
    TIM7->CR1 |= (TIM_CR1_URS | TIM_CR1_CEN);

    NVIC_SetPriority(TIM7_IRQn, 15);
    NVIC_EnableIRQ(TIM7_IRQn);

    /* USART1 */
    // PB6 TX: Output, alternate function, push-pull
    GPIOB->MODER |= GPIO_MODER_MODER6_1;
    GPIOB->OTYPER &= ~GPIO_OTYPER_OT_6;
    GPIOB->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR6_0;
    GPIOB->AFR[0] |= (0x07 << 24);

    // PB7 RX: Input, alternate function, floating
    GPIOB->MODER |= GPIO_MODER_MODER7_1;
    GPIOB->PUPDR &= ~GPIO_PUPDR_PUPDR7;
    GPIOB->AFR[0] |= (0x07 << 28);

    USART1->BRR = 0x0364; // 57600
    USART1->CR1 |= (/*USART_CR1_TXEIE | */USART_CR1_RXNEIE | USART_CR1_TE | USART_CR1_RE);
    USART1->CR3 |= USART_CR3_DMAT;
    USART1->CR1 |= USART_CR1_UE;

    NVIC_EnableIRQ(USART1_IRQn);

    /* DMA */
    DMA2_Stream7->NDTR = sizeof(ecu.status);
    DMA2_Stream7->PAR = (uint32_t)&(USART1->DR);
    DMA2_Stream7->M0AR = (uint32_t)&(ecu.status);
    DMA2_Stream7->CR |= DMA_SxCR_CHSEL_2; // Channel 4
     // Memory increment, memory-to-peripheral
    DMA2_Stream7->CR |= (DMA_SxCR_MINC | DMA_SxCR_CIRC | DMA_SxCR_DIR_0);
    DMA2_Stream7->CR |= DMA_SxCR_EN;

    /* IO */
    switch_init();
    actuators_init();

    ecu.config.fuel_pump_prime_time = 2000;

    __enable_irq();

    uint32_t mtime, mtime1, dtime1;
    uint8_t execount;

    mtime1 = 0;
    execount = 0;

    for (;;)
    {
        switch_update();
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
        }

        /* 
            События на каждую итерацию.
            Часть событий распределена по итерациям для более точного выполнения
            событий с заданным интервалом.
        */
        calc_load();

        if (execount == 1)
        {
            warmup_enrich();
            inj_afr();
            inj_trim();
            inj_timing();
            inj_calc_pw();
        } 
        else if (execount == 2)
        {
            ign_dwell();
            ign_timing();
        }
        else if (execount == 3)
        {
            cooling_fan();
            vvt();
        }
        else if (execount == 4)
        {
            comm();
            execount = 0;
        }

        execount++;
    }

    return 0;
}
