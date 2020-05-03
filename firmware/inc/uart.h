#ifndef ROBOT_UART_H
#define ROBOT_UART_H

/* UART driver, 19 Feb 2019. */
#include "stm32f10x.h"

#define FREQUENCY ((uint32_t)72000000)

void uart_init(USART_TypeDef *uart, uint32_t baudrate);
void uart_write_line(USART_TypeDef *uart, char *str);
void uart_receive(USART_TypeDef *uart, void (*callback)(void), char *buffer);

#endif /* ROBOT_UART_H */
