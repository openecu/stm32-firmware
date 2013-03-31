#ifndef _UART_H
#define _UART_H

#include <stdint.h>

#define UART_BAUD_RATE 115200

#define UART_RX_BUF_SIZE    128
#define UART_TX_BUF_SIZE    128

#define UART_TX_BUF_MASK    (UART_TX_BUF_SIZE - 1)
#define UART_RX_BUF_MASK    (UART_RX_BUF_SIZE - 1)

#define UART_NO_DATA    0x0100

void uart_init(void);

void uart_putc(uint8_t c);

uint16_t uart_getc(void);

#endif
