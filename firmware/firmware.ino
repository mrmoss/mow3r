#include "motor.hpp"

motor_t lf(2,3);
motor_t lr(4,5);
motor_t rf(6,7);
motor_t rr(8,9);

void setup()
{
    rf.reverse();
    rr.reverse();
}

void loop()
{
}
