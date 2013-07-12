#include "cmsis/stm32f4xx.h"
#include "comm.h"
#include "config.h"
#include "status.h"

uint8_t rx_buf[UART_RX_BUF_SIZE];
uint8_t tx_buf[UART_TX_BUF_SIZE];

volatile uint16_t rx_head;
volatile uint16_t rx_tail;
volatile uint16_t tx_head;
volatile uint16_t tx_tail;

void comm_init(void)
{
    /* USART1 */
    // PA2 TX: Output, alternate function, push-pull
    GPIOA->MODER |= GPIO_MODER_MODER2_1;
    GPIOA->OTYPER &= ~GPIO_OTYPER_OT_2;
    GPIOA->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR2_0;
    GPIOA->AFR[0] |= (0x07 << 8); // USART2_TX

    // PA3 RX: Input, alternate function, floating
    GPIOA->MODER |= GPIO_MODER_MODER3_1;
    GPIOA->PUPDR &= ~GPIO_PUPDR_PUPDR3;
    GPIOA->AFR[0] |= (0x07 << 12); // USART2_RX

    USART2->BRR = 0x00D9; // 50.0 MHz / 0x00D9 = 115200
    //USART2->CR3 |= USART_CR3_DMAT;
    USART2->CR1 |= (USART_CR1_RXNEIE | USART_CR1_TE | USART_CR1_RE);
    USART2->CR1 |= USART_CR1_UE;

    NVIC_SetPriority(USART2_IRQn, 15);
    NVIC_EnableIRQ(USART2_IRQn);

    /* DMA */
    DMA1_Stream6->NDTR = sizeof(status.sync);
    DMA1_Stream6->M0AR = (uint32_t)&(status.sync);
    DMA1_Stream6->PAR = (uint32_t)&(USART2->DR);
    // channel 4, memory increment, memory-to-peripheral, transfer complete interrupt
    DMA1_Stream6->CR |= DMA_SxCR_CHSEL_2 | DMA_SxCR_MINC | DMA_SxCR_DIR_0 | DMA_SxCR_TCIE;
    //DMA1_Stream6->CR |= DMA_SxCR_EN;

    NVIC_SetPriority(DMA1_Stream6_IRQn, 15);
    NVIC_EnableIRQ(DMA1_Stream6_IRQn);
}

void DMA1_Stream6_IRQHandler(void)
{
    if ((DMA1->HISR & DMA_HISR_TCIF6))
    {
        DMA1->HIFCR |= DMA_HIFCR_CTCIF6;

        SETBIT(status.comm.flags1, COMM_FLAGS1_READY);
    }
}

void uart_putc(uint8_t c)
{
    uint16_t tmphead;

    tmphead = (tx_head + 1) & UART_TX_BUF_MASK;
    tx_buf[tmphead] = c;
    tx_head = tmphead;

    USART2->CR1 |= USART_CR1_TXEIE;
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

void USART2_IRQHandler(void)
{
    if ((USART2->SR & USART_SR_RXNE))
    {
        uint8_t data, tmphead;

        USART2->SR = ~USART_SR_RXNE;

        data = USART2->DR;
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

    if ((USART2->SR & USART_SR_TXE))
    {
        uint16_t tmptail;

        USART2->SR = ~USART_SR_TXE;

        if (tx_tail != tx_head)
        {
            tmptail = (tx_tail + 1) & UART_TX_BUF_MASK;
            tx_tail = tmptail;
            USART2->DR = tx_buf[tmptail];
        } 
        else
        {
            USART2->CR1 &= ~USART_CR1_TXEIE;
        }
    }
}
