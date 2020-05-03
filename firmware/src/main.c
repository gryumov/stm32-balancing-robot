#include "balance.h"
#include "motors.h"
#include "mpu6050.h"

int main(void)
{

    mpu6050_init();
    balance_init();
    motors_init();

    while (1)
    {
    }
}
