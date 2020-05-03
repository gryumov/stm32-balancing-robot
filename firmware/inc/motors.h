#ifndef ROBOT_MOTORS_H
#define ROBOT_MOTORS_H

#include <stdint.h>

void motors_init(void);
void motors_stop(void);
void motors_forward(uint8_t left, uint8_t right);
void motors_reverse(uint8_t left, uint8_t right);
float encoders_read_position(void);
void motors_set_pwm(int pwm);

#endif /* ROBOT_MOTORS_H */
