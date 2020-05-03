#include "mpu6050.h"
#include "i2c.h"

void mpu6050_init(void)
{
    /* Start I2C interface */
    i2c_init();

    /* Config sensor */
    i2c_write_register(MPU6050_WRITE_ADDRESS, MPU6050_RA_PWR_MGMT_1, 0x80);

    for (int i = 0; i < 240000; i++)
    {
    };

    i2c_write_register(MPU6050_WRITE_ADDRESS, MPU6050_RA_PWR_MGMT_1, 0x01);
    i2c_write_register(MPU6050_WRITE_ADDRESS, MPU6050_RA_CONFIG, 0x01);
    i2c_write_register(MPU6050_WRITE_ADDRESS, MPU6050_RA_SMPLRT_DIV, 0x01);

    /* Accelerometer full scale ±2 g (AFS_SEL = 0, bits 4:3), matches MPU6050_ACCE_SENS_2 */
    uint8_t register_config;
    register_config = i2c_read_register(0xD0, 0xD1, MPU6050_RA_ACCEL_CONFIG);
    register_config &= 0xE7;
    i2c_write_register(MPU6050_WRITE_ADDRESS, MPU6050_RA_ACCEL_CONFIG, register_config);

    /* Gyroscope full scale ±250 °/s (FS_SEL = 0, bits 4:3), matches MPU6050_GYRO_SENS_250 */
    register_config = i2c_read_register(0xD0, 0xD1, MPU6050_RA_GYRO_CONFIG);
    register_config &= 0xE7;
    i2c_write_register(MPU6050_WRITE_ADDRESS, MPU6050_RA_GYRO_CONFIG, register_config);
}
