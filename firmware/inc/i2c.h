#ifndef ROBOT_I2C_H
#define ROBOT_I2C_H

/* Device addresses include the R/W bit: 0xD0 (write) and 0xD1 (read) for the MPU-6050. */
#include <stdint.h>
void i2c_init(void);
void i2c_write_register(uint8_t device_address, uint8_t register_address, uint8_t value);
void i2c_read_registers(uint8_t write_address, uint8_t read_address, uint8_t register_address,
                        uint8_t *buffer, uint8_t size);
uint8_t i2c_read_register(uint8_t write_address, uint8_t read_address, uint8_t register_address);
void i2c_write_byte(uint8_t device_address, uint8_t value);

#endif /* ROBOT_I2C_H */
