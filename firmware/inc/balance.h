#ifndef ROBOT_BALANCE_H
#define ROBOT_BALANCE_H

/* Starts TIM4 and runs the control loop in its interrupt. */
void balance_init(void);
void TIM4_IRQHandler(void);

#endif /* ROBOT_BALANCE_H */
