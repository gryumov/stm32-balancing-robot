#include "i2c.h"
#include "stm32f10x.h"

uint32_t volatile system_counter = 0;

void SysTick_Handler(void)
{
    system_counter++;
    if (system_counter > 100)
    {
        NVIC_SystemReset();
        system_counter = 0;
    }
}

static void systick_init(uint32_t times)
{
    SysTick->LOAD = times;
    SysTick->VAL = times;
    SysTick->CTRL =
        (SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_TICKINT_Msk | SysTick_CTRL_ENABLE_Msk);
}

void i2c_init(void)
{

    systick_init(720000);

    // Enable GPIO and I2C clocks

    RCC->APB2ENR |= RCC_APB2ENR_IOPBEN; // GPIOB clock
    RCC->APB1ENR |= RCC_APB1ENR_I2C1EN; // I2C1 clock
    RCC->APB2ENR |= RCC_APB2ENR_AFIOEN; // AFIO clock

    // PB6 - SCL

    GPIOB->CRL &= ~GPIO_CRL_MODE6;  // Clear field
    GPIOB->CRL |= GPIO_CRL_MODE6_1; // Output mode, speed 2 MHz (10)

    GPIOB->CRL &= ~GPIO_CRL_CNF6; // Clear field
    GPIOB->CRL |= GPIO_CRL_CNF6;  // Alternate function output Open-drain (11)

    // PB7 - SDA
    GPIOB->CRL &= ~GPIO_CRL_MODE7;  // Clear field
    GPIOB->CRL |= GPIO_CRL_MODE7_1; // Output mode, speed 2 MHz (10)

    GPIOB->CRL &= ~GPIO_CRL_CNF7; // Clear field
    GPIOB->CRL |= GPIO_CRL_CNF7;  // Alternate function output Open-drain (11)

    // Configure the I2C peripheral

        I2C1->CR2 = 0x18;         // Peripheral clock frequency, MHz
    I2C1->CCR = 0x1E;         // Clock control
    I2C1->CCR &= ~I2C_CCR_FS; // Standard mode
    I2C1->TRISE = 0x19;       // Rise time
    I2C1->CR1 |= I2C_CR1_ACK; // Enable ACK
    I2C1->OAR1 = 0x10;        // Own address
    I2C1->CR1 |= I2C_CR1_PE;  // Enable peripheral
}

void i2c_read_registers(uint8_t write_address, uint8_t read_address, uint8_t register_address,
                        uint8_t *buffer, uint8_t size)
{
    uint8_t i = 0x00;
    I2C1->CR1 |= I2C_CR1_ACK;
    I2C1->CR1 |= I2C_CR1_START;
    uint32_t timeout = 0;
    while (!(I2C1->SR1 & I2C_SR1_SB))
    {
        if (timeout == 7200000)
        {
            NVIC_SystemReset();
        }
        timeout++;
    };
    (void)I2C1->SR1;
    // Send device address
    I2C1->DR = write_address;

    while (!(I2C1->SR1 & I2C_SR1_ADDR))
    {
        if (timeout == 7200000)
        {
            NVIC_SystemReset();
        }
        timeout++;
    };
    ;
    (void)I2C1->SR1;
    (void)I2C1->SR2;
    // Send register address
    I2C1->DR = register_address;

    while (!(I2C1->SR1 & I2C_SR1_TXE))
    {
        if (timeout == 7200000)
        {
            NVIC_SystemReset();
        }
        timeout++;
    };
    ;
    I2C1->CR1 |= I2C_CR1_START;
    while (!(I2C1->SR1 & I2C_SR1_SB))
        ;
    (void)I2C1->SR1;
    // Send device read address
    I2C1->DR = read_address;

    while (!(I2C1->SR1 & I2C_SR1_ADDR))
    {
        if (timeout == 7200000)
        {
            NVIC_SystemReset();
        }
        timeout++;
    };
    ;
    (void)I2C1->SR1;
    (void)I2C1->SR2;

    if (size > 0x01)
    {
        size--;
        while (size--)
        {
            while (!(I2C1->SR1 & I2C_SR1_RXNE))
                ;
            buffer[i++] = I2C1->DR;
        }
    }
    while (!(I2C1->SR1 & I2C_SR1_BTF))
    {
        if (timeout == 7200000)
        {
            NVIC_SystemReset();
        }
        timeout++;
    };
    I2C1->CR1 &= ~I2C_CR1_ACK;
    buffer[i++] = I2C1->DR;
    I2C1->CR1 |= I2C_CR1_STOP;

    system_counter = 0;
    timeout = 0;
}

/* Write one byte to a device register. */

void i2c_write_register(uint8_t device_address, uint8_t register_address, uint8_t value)
{

    I2C1->CR1 |= I2C_CR1_START;
    while (!(I2C1->SR1 & I2C_SR1_SB))
        ;
    (void)(I2C1->SR1); // Read status register

    // Send device address
    I2C1->DR = device_address;
    while ((!(I2C1->SR1 & (I2C_SR1_AF | I2C_SR1_ADDR))))
    {
    };
    (void)(I2C1->SR1); // Read status register
    (void)(I2C1->SR2);
    (void)(I2C1->SR2); // SR2 must be read here to clear ADDR

    // Send register address
    I2C1->DR = register_address;
    while (!(I2C1->SR1 & I2C_SR1_BTF))
        ; /* Byte Transfer Finished */

    // Send data byte
    I2C1->DR = value;
    while (!(I2C1->SR1 & I2C_SR1_BTF))
        ; /* Byte Transfer Finished */
}

void i2c_write_byte(uint8_t device_address, uint8_t value)
{

    I2C1->CR1 |= I2C_CR1_START;
    while (!(I2C1->SR1 & I2C_SR1_SB))
        ;
    (void)(I2C1->SR1); // Read status register

    // Send device address
    I2C1->DR = device_address;
    while ((!(I2C1->SR1 & (I2C_SR1_AF | I2C_SR1_ADDR))))
    {
    };
    (void)(I2C1->SR1); // Read status register
    (void)(I2C1->SR2);
    (void)(I2C1->SR2); // SR2 must be read here to clear ADDR

    // Send data byte
    I2C1->DR = value;
    while (!(I2C1->SR1 & I2C_SR1_BTF))
        ; /* Byte Transfer Finished */
}

uint8_t i2c_read_register(uint8_t write_address, uint8_t read_address, uint8_t register_address)
{
    uint8_t data;

    I2C1->CR1 |= I2C_CR1_ACK;
    I2C1->CR1 |= I2C_CR1_START;

    while (!(I2C1->SR1 & I2C_SR1_SB))
        ;
    (void)I2C1->SR1;
    // Send device address
    I2C1->DR = write_address;

    while (!(I2C1->SR1 & I2C_SR1_ADDR))
        ;
    (void)I2C1->SR1;
    (void)I2C1->SR2;
    // Send register address
    I2C1->DR = register_address;

    while (!(I2C1->SR1 & I2C_SR1_TXE))
        ;
    I2C1->CR1 |= I2C_CR1_START;
    while (!(I2C1->SR1 & I2C_SR1_SB))
        ;
    (void)I2C1->SR1;
    // Send device read address
    I2C1->DR = read_address;

    while (!(I2C1->SR1 & I2C_SR1_ADDR))
        ;
    (void)I2C1->SR1;
    (void)I2C1->SR2;
    I2C1->CR1 |= I2C_CR1_STOP;
    I2C1->CR1 &= ~I2C_CR1_ACK;
    while (!(I2C1->SR1 & I2C_SR1_RXNE))
        ;
    data = I2C1->DR;
    return data;
}
