# Firmware

[![Русский](https://img.shields.io/badge/%D0%A0%D1%83%D1%81%D1%81%D0%BA%D0%B8%D0%B9-6e7781?style=flat-square)](README.ru.md) [![English](https://img.shields.io/badge/English-0969da?style=flat-square)](README.md)

[← About the project](../README.md)

## Open the project

Project: [keil/robot.uvprojx](keil/robot.uvprojx), targeting the **STM32F103C8**.

Toolchain: Keil µVision, ARM Compiler **5.06 update 6**, **Keil.STM32F1xx_DFP 2.3.0**, and **ARM.CMSIS 5.5.1**. Install these packs in Keil, open the project, and run **Rebuild**.

## Source layout

| File | Purpose |
| --- | --- |
| [src/main.c](src/main.c) | Sensor, timer, and motor startup |
| [src/balance.c](src/balance.c) | Measurements, filtering, and control in the TIM4 interrupt handler |
| [src/mpu6050.c](src/mpu6050.c) | Accelerometer and gyroscope configuration |
| [src/i2c.c](src/i2c.c) | I²C communication through STM32 registers |
| [src/motors.c](src/motors.c) | PWM, motor direction, and encoders |
| [src/uart.c](src/uart.c) | Serial interface |
| [inc/robot_config.h](inc/robot_config.h) | Control gains, filter weights, and encoder conversion factor |
| [inc/mpu6050.h](inc/mpu6050.h) | Sensor registers and sensitivity factors |

## Balance control

At each step, the angle is estimated as:

```text
angle = 0.98 × (previous angle + angular velocity × dt)
      + 0.02 × accelerometer angle
```

The controller uses tilt, angular velocity, wheel displacement, and wheel velocity. Tune K1–K5 in [robot_config.h](inc/robot_config.h) for your motors and frame. The gains are `volatile`, so you can also adjust them in the debugger. The current values are from my robot.

## First run

Start with the wheels off the ground. Check the tilt reading and motor direction before tuning the gains.

## Third-party code

The register definitions in `inc/mpu6050.h` come from [Jeff Rowberg's I2Cdevlib](https://github.com/jrowberg/i2cdevlib/blob/master/Arduino/MPU6050/MPU6050.h). Their MIT license is included in the header.

ST and ARM support files in `keil/RTE` come from the [Keil.STM32F1xx_DFP 2.3.0 pack](https://www.keil.com/pack/Keil.STM32F1xx_DFP.2.3.0.pack), with their original notices preserved. Everything else is under the MIT license, see [LICENSE](../LICENSE).
