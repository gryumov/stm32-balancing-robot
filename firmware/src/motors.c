#include "motors.h"
#include "robot_config.h"
#include "stm32f10x.h"

static void encoder_tim1_init(void)
{
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_AFIOEN;
    RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;

    GPIOA->CRH &= ~(GPIO_CRH_CNF8_0 | GPIO_CRH_CNF9_0);
    GPIOA->CRH |= GPIO_CRH_CNF8_1 | GPIO_CRH_CNF9_1;

    GPIOA->BSRR = GPIO_BSRR_BR8;
    GPIOA->BSRR = GPIO_BSRR_BR9;

    TIM1->CCMR1 |= TIM_CCMR1_CC1S_0 | TIM_CCMR1_CC2S_0;
    TIM1->CCER &= ~(TIM_CCER_CC1P | TIM_CCER_CC2P);
    TIM1->SMCR |= TIM_SMCR_SMS_0 | TIM_SMCR_SMS_1;
    TIM1->ARR = 65535;
    TIM1->CR1 |= TIM_CR1_CEN;
    TIM1->CNT = 0;
}

static void encoder_tim3_init(void)
{
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_AFIOEN;
    RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
    GPIOA->CRL &= ~(GPIO_CRL_CNF6_0 | GPIO_CRL_CNF7_0);
    GPIOA->CRL |= GPIO_CRL_CNF6_1 | GPIO_CRL_CNF7_1;

    GPIOA->BSRR = GPIO_BSRR_BR6;
    GPIOA->BSRR = GPIO_BSRR_BR7;

    TIM3->CCMR1 |= TIM_CCMR1_CC1S_0 | TIM_CCMR1_CC2S_0;
    TIM3->CCER &= ~(TIM_CCER_CC1P | TIM_CCER_CC2P);
    TIM3->SMCR |= TIM_SMCR_SMS_0 | TIM_SMCR_SMS_1;
    TIM3->ARR = 65535;
    TIM3->CR1 |= TIM_CR1_CEN;
    TIM3->CNT = 0;
}

static void motor_pwm_init(void)
{

    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;

    GPIOA->CRL &= ~GPIO_CRL_CNF0; /* PA0 */
    GPIOA->CRL |= GPIO_CRL_CNF0_1;

    GPIOA->CRL &= ~GPIO_CRL_MODE0;
    GPIOA->CRL |= GPIO_CRL_MODE0;

    GPIOA->CRL &= ~GPIO_CRL_CNF1; /* PA1 */
    GPIOA->CRL |= GPIO_CRL_CNF1_1;

    GPIOA->CRL &= ~GPIO_CRL_MODE1;
    GPIOA->CRL |= GPIO_CRL_MODE1;

    TIM2->PSC = (SystemCoreClock / 24000000 - 1);
    TIM2->ARR = 255;

    TIM2->CCR1 = 0;
    TIM2->CCR2 = 0;

    TIM2->CCMR1 |= TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_2; // CH1
    TIM2->CCER |= TIM_CCER_CC1E;
    TIM2->CCER &= ~TIM_CCER_CC1P;

    TIM2->CCMR1 |= TIM_CCMR1_OC2M_1 | TIM_CCMR1_OC2M_2; // CH2
    TIM2->CCER |= TIM_CCER_CC2E;                        // on CH2
    TIM2->CCER &= ~TIM_CCER_CC2P;

    TIM2->CR1 &= ~TIM_CR1_DIR;
    TIM2->CR1 |= TIM_CR1_CEN;
}

static void motor_gpio_init(void)
{
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;

    GPIOA->CRL &= ~(GPIO_CRL_MODE2 | GPIO_CRL_CNF2);
    GPIOA->CRL |= GPIO_CRL_MODE2_1;

    GPIOA->CRL &= ~(GPIO_CRL_MODE3 | GPIO_CRL_CNF3);
    GPIOA->CRL |= GPIO_CRL_MODE3_1;

    GPIOA->CRL &= ~(GPIO_CRL_MODE4 | GPIO_CRL_CNF4);
    GPIOA->CRL |= GPIO_CRL_MODE4_1;

    GPIOA->CRL &= ~(GPIO_CRL_MODE5 | GPIO_CRL_CNF5);
    GPIOA->CRL |= GPIO_CRL_MODE5_1;

    GPIOA->CRH &= ~(GPIO_CRH_MODE10 | GPIO_CRH_CNF10);
    GPIOA->CRH |= GPIO_CRH_MODE10_1;

    GPIOA->BSRR = GPIO_BSRR_BS10;
}

void motors_forward(uint8_t left, uint8_t right)
{
    TIM2->CCR1 = right;
    TIM2->CCR2 = left;

    GPIOA->BSRR = (GPIO_BSRR_BS2);
    GPIOA->BSRR = (GPIO_BSRR_BS5);
    GPIOA->BSRR = (GPIO_BSRR_BR3);
    GPIOA->BSRR = (GPIO_BSRR_BR4);
}

void motors_reverse(uint8_t left, uint8_t right)
{
    TIM2->CCR1 = right;
    TIM2->CCR2 = left;

    GPIOA->BSRR = (GPIO_BSRR_BS3);
    GPIOA->BSRR = (GPIO_BSRR_BS4);
    GPIOA->BSRR = (GPIO_BSRR_BR2);
    GPIOA->BSRR = (GPIO_BSRR_BR5);
}

void motors_stop(void)
{
    GPIOA->BSRR = (GPIO_BSRR_BR3);
    GPIOA->BSRR = (GPIO_BSRR_BR4);
    GPIOA->BSRR = (GPIO_BSRR_BR2);
    GPIOA->BSRR = (GPIO_BSRR_BR5);
}

float encoders_read_position(void)
{
    int16_t volatile left_count;
    int16_t volatile right_count;

    float left_position;
    float right_position;
    float center_position;

    left_count = TIM3->CNT;
    right_count = TIM1->CNT;

    left_position = left_count * ENCODER_METERS_PER_COUNT;
    right_position = right_count * ENCODER_METERS_PER_COUNT;
    center_position = 0.5 * (left_position + right_position);

    return center_position;
}

void motors_set_pwm(int pwm)
{
    if (pwm > 255)
    {
        pwm = 255;
    }
    if (pwm < -255)
    {
        pwm = -255;
    }
    if (pwm > 0)
    {
        motors_forward(pwm, pwm);
    }
    else if (pwm < 0)
    {
        motors_reverse(-pwm, -pwm);
    }
    else
    {
        motors_stop();
    }
}

void motors_init(void)
{
    encoder_tim1_init();
    encoder_tim3_init();
    motor_pwm_init();
    motor_gpio_init();
}
