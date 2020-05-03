#include "balance.h"
#include "robot_config.h"
#include "mpu6050.h"
#include "motors.h"
#include "stm32f10x.h"

#include <math.h>

void balance_init(void)
{
    RCC->APB1ENR |= RCC_APB1ENR_TIM4EN;
    TIM4->PSC = (SystemCoreClock / 100000 - 1);
    TIM4->ARR = 1000;
    TIM4->DIER |= TIM_DIER_UIE;
    TIM4->CR1 |= TIM_CR1_CEN;
    NVIC_EnableIRQ(TIM4_IRQn);
}

float volatile accelerometer_angle;
float volatile filtered_angle;
float volatile filtered_velocity;
float volatile previous_encoder_position = 0;
float volatile motor_command;
float volatile K1 = BALANCE_K1;
float volatile K2 = BALANCE_K2;
float volatile K3 = BALANCE_K3;
float volatile K4 = BALANCE_K4;
float volatile K5 = BALANCE_K5;
float volatile distance = 0;

void TIM4_IRQHandler(void)
{
    TIM4->SR &= ~TIM_SR_UIF;

    double volatile gyro_accel[6];
    uint8_t sample_buffer[14];
    int16_t accel_x_raw;
    int16_t accel_y_raw;
    int16_t accel_z_raw;
    int16_t temperature_raw;
    int16_t gyro_x_raw;
    int16_t gyro_y_raw;
    int16_t gyro_z_raw;

    i2c_read_registers(0xD0, 0xD1, MPU6050_RA_ACCEL_XOUT_H, sample_buffer, 14);

    accel_x_raw = ((sample_buffer[0] << 8) | sample_buffer[1]);
    gyro_accel[0] = accel_x_raw / MPU6050_ACCE_SENS_2;

    accel_y_raw = ((sample_buffer[2] << 8) | sample_buffer[3]);
    gyro_accel[1] = accel_y_raw / MPU6050_ACCE_SENS_2;

    accel_z_raw = ((sample_buffer[4] << 8) | sample_buffer[5]);
    gyro_accel[2] = accel_z_raw / MPU6050_ACCE_SENS_2;

    temperature_raw = (((sample_buffer[6] << 8) | sample_buffer[7]));
    temperature_raw /= 340;
    temperature_raw += 36.53;

    gyro_x_raw = ((sample_buffer[8] << 8) | sample_buffer[9]);
    gyro_accel[3] = gyro_x_raw / MPU6050_GYRO_SENS_250;

    gyro_y_raw = ((sample_buffer[10] << 8) | sample_buffer[11]);
    gyro_accel[4] = gyro_y_raw / MPU6050_GYRO_SENS_250;

    gyro_z_raw = ((sample_buffer[12] << 8) | sample_buffer[13]);
    gyro_accel[5] = gyro_z_raw / MPU6050_GYRO_SENS_250;

    if (gyro_accel[AZ] != 0)
        accelerometer_angle = atan(gyro_accel[AY] / gyro_accel[AZ]);

    filtered_angle = (1 - ACCEL_FILTER_WEIGHT) *
                         (filtered_angle + DEGREES_TO_RADIANS * gyro_accel[GX] * BALANCE_PERIOD_S) +
                     ACCEL_FILTER_WEIGHT * accelerometer_angle;

    filtered_velocity = (1 - ENCODER_FILTER_WEIGHT) * (filtered_velocity) +
                        ENCODER_FILTER_WEIGHT *
                            (encoders_read_position() - previous_encoder_position) /
                            BALANCE_PERIOD_S;

    previous_encoder_position = encoders_read_position();

    motor_command = K5 * (K1 * (filtered_angle) + K2 * DEGREES_TO_RADIANS * gyro_accel[GX] +
                          K3 * (encoders_read_position() + distance) + K4 * filtered_velocity);

    motors_set_pwm(motor_command);
}
