#ifndef COMM_H
#define COMM_H

#include "cmsis/stm32f4xx.h"

#define UART_BAUD_RATE 115200

#define UART_RX_BUF_SIZE    128
#define UART_TX_BUF_SIZE    128

#define UART_TX_BUF_MASK    (UART_TX_BUF_SIZE - 1)
#define UART_RX_BUF_MASK    (UART_RX_BUF_SIZE - 1)

#define UART_NO_DATA    0x0100

#define COMM_FLAGS1_READY   0x00000001

typedef struct
{
    uint32_t flags1;

} comm_state_t;

void comm_init(void);

void uart_putc(uint8_t c);

uint16_t uart_getc(void);

#endif
