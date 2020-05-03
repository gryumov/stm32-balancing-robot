#include "uart.h"

#include <string.h>

#define USART1_REMAP 1 // REMAP ON TX PA9->PB6, PX PA10->PB7

void uart_init(USART_TypeDef *uart, uint32_t baudrate)
{

    uint32_t volatile baud_divider =
        ((FREQUENCY + baudrate / 2) / baudrate); // USART baud rate divider

    RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;

    if (uart == USART1)
    {
        RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
#if USART1_REMAP
        RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;
        GPIOB->CRL &= ~(GPIO_CRL_MODE6 | GPIO_CRL_CNF6);
        GPIOB->CRL |= GPIO_CRL_CNF6_1; /* PB6 TX*/
        GPIOB->CRL |= GPIO_CRL_MODE6_1;
        GPIOB->CRL &= ~(GPIO_CRL_MODE7 | GPIO_CRL_CNF7);
        GPIOB->CRL |= GPIO_CRL_CNF7_0; /* PB7 RX */
        AFIO->MAPR |= AFIO_MAPR_USART1_REMAP;
#else
        RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
        GPIOA->CRH &= ~(GPIO_CRH_MODE9 | GPIO_CRH_CNF9);
        GPIOA->CRH |= GPIO_CRH_CNF9_1; /* PA9 TX*/
        GPIOA->CRH |= GPIO_CRH_MODE9_1;
        GPIOA->CRH &= ~(GPIO_CRH_MODE10 | GPIO_CRH_CNF10);
        GPIOA->CRH |= GPIO_CRH_CNF10_0; /* PA10 RX */
#endif
    }

    if (uart == USART2)
    {
        RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
        RCC->APB1ENR |= RCC_APB1ENR_USART2EN;
        GPIOA->CRL &= ~(GPIO_CRL_MODE2 | GPIO_CRL_CNF2);
        GPIOA->CRL |= GPIO_CRL_CNF2_1; /* PA2 TX*/
        GPIOA->CRL |= GPIO_CRL_MODE2_1;
        GPIOA->CRL &= ~(GPIO_CRL_MODE3 | GPIO_CRL_CNF3);
        GPIOA->CRL |= GPIO_CRL_CNF3_0; /* PA3 RX */
    }

    if (uart == USART3)
    {
        RCC->APB1ENR |= RCC_APB1ENR_USART3EN;
        RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;
        GPIOB->CRH &= ~(GPIO_CRH_MODE10 | GPIO_CRH_CNF10);
        GPIOB->CRH |= GPIO_CRH_CNF10_1; /* PB10 TX*/
        GPIOB->CRH |= GPIO_CRH_MODE10_1;

        GPIOB->CRH &= ~(GPIO_CRH_MODE11 | GPIO_CRH_CNF11);
        GPIOB->CRH |= GPIO_CRH_CNF11_0; /* PB11 RX */
    }

    uart->BRR = baud_divider;
    uart->CR2 = 0;
    uart->CR1 = 0;
    uart->CR1 |= USART_CR1_TE;
    uart->CR1 |= USART_CR1_RE;
    uart->CR1 |= USART_CR1_UE;
}

void uart_write_line(USART_TypeDef *uart, char *str)
{
    uint16_t i = 0;

    for (i = 0; i < strlen(str); i++)
    {
        while (!(uart->SR & USART_SR_TC))
        {
        };
        uart->DR = str[i];
    }

    while (!(uart->SR & USART_SR_TC))
        ;
    uart->DR = '\n';
}

static void uart_default_callback(void) {}

static void (*uart1_callback)(void) = uart_default_callback;
static void (*uart2_callback)(void) = uart_default_callback;
static void (*uart3_callback)(void) = uart_default_callback;

char *receive_buffer;

void uart_receive(USART_TypeDef *uart, void (*callback)(void), char *buffer)
{
    uart->CR1 |= USART_CR1_RXNEIE;
    if (uart == USART1)
    {
        NVIC_EnableIRQ(USART1_IRQn);
        receive_buffer = buffer;
        uart1_callback = callback;
    }

    if (uart == USART2)
    {
        NVIC_EnableIRQ(USART2_IRQn);
        receive_buffer = buffer;
        uart2_callback = callback;
    }

    if (uart == USART3)
    {
        NVIC_EnableIRQ(USART3_IRQn);
        receive_buffer = buffer;
        uart3_callback = callback;
    }
}

void USART1_IRQHandler(void)
{
    char tmp;
    if ((USART1->SR & USART_SR_RXNE) != 0)
    {
        tmp = USART1->DR;
        if (tmp == 0x0D)
        {
            uart1_callback();
            return;
        }
        receive_buffer[strlen(receive_buffer)] = tmp;
    }
}

void USART2_IRQHandler(void)
{
    char tmp;
    if ((USART2->SR & USART_SR_RXNE) != 0)
    {
        tmp = USART2->DR;
        if (tmp == 0x0D)
        {
            uart2_callback();
            return;
        }
        receive_buffer[strlen(receive_buffer)] = tmp;
    }
}

void USART3_IRQHandler(void)
{
    char tmp;
    if ((USART3->SR & USART_SR_RXNE) != 0)
    {
        tmp = USART3->DR;
        if (tmp == 0x0D)
        {
            uart3_callback();
            return;
        }
        receive_buffer[strlen(receive_buffer)] = tmp;
    }
}
