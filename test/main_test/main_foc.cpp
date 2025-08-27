#include "Arduino.h"
#include "my_servo.h"
#include "my_foc.h"

void setup()
{
    Serial.begin(115200);
    my_motor_init();
    motor_1.controller = velocity;
    motor_2.controller = velocity;
    motor_1.target = 10;
    motor_2.target = 5;
}

void loop()
{
    my_motor_do();
}
