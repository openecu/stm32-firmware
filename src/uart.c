#include <stm32f4xx.h>
#include "uart.h"

uint8_t rx_buf[UART_RX_BUF_SIZE];
uint8_t tx_buf[UART_TX_BUF_SIZE];
volatile uint16_t rx_head;
volatile uint16_t rx_tail;
volatile uint16_t tx_head;
volatile uint16_t tx_tail;

void uart_init(void)
{
    /* USART1 */
    // PB6 TX: Output, alternate function, push-pull
    GPIOB->MODER |= GPIO_MODER_MODER6_1;
    GPIOB->OTYPER &= ~GPIO_OTYPER_OT_6;
    GPIOB->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR6_0;
    GPIOB->AFR[0] |= (0x07 << 24); // USART1_TX

    // PB7 RX: Input, alternate function, floating
    GPIOB->MODER |= GPIO_MODER_MODER7_1;
    GPIOB->PUPDR &= ~GPIO_PUPDR_PUPDR7;
    GPIOB->AFR[0] |= (0x07 << 28); // USART1_RX

    USART1->BRR = 0x01B2; // 50.0 MHz / 0x01B2 = 115200
    USART1->CR1 |= (USART_CR1_RXNEIE | USART_CR1_TE | USART_CR1_RE);
    USART1->CR3 |= USART_CR3_DMAT;
    USART1->CR1 |= USART_CR1_UE;

    NVIC_EnableIRQ(USART1_IRQn);

    /* DMA */
    //DMA2_Stream7->NDTR = sizeof(ecu.status);
    DMA2_Stream7->PAR = (uint32_t)&(USART1->DR);
    //DMA2_Stream7->M0AR = (uint32_t)&(ecu.status);
    DMA2_Stream7->CR |= DMA_SxCR_CHSEL_2; // Channel 4
     // Memory increment, memory-to-peripheral
    DMA2_Stream7->CR |= (DMA_SxCR_MINC | DMA_SxCR_CIRC | DMA_SxCR_DIR_0);
    //DMA2_Stream7->CR |= DMA_SxCR_EN;
}

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
