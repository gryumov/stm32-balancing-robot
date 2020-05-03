#ifndef ROBOT_CONFIG_H
#define ROBOT_CONFIG_H

/* Robot settings. */
#define BALANCE_PERIOD_S ((float)0.01)
#define ACCEL_FILTER_WEIGHT ((float)0.02)
#define ENCODER_FILTER_WEIGHT ((float)0.05)

/* Controller gains: tilt, angular velocity, distance, velocity, overall scale. */
#define BALANCE_K1 25
#define BALANCE_K2 0.3
#define BALANCE_K3 0.5
#define BALANCE_K4 18
#define BALANCE_K5 90

/* Meters per encoder count: wheel radius 33 mm, 1185 counts per revolution. */
#define ENCODER_METERS_PER_COUNT (float)(0.000174886)

#endif /* ROBOT_CONFIG_H */
