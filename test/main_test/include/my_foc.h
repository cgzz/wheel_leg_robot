#include <SimpleFOC.h>

extern BLDCMotor motor_1;
extern BLDCMotor motor_2;

extern float target_L;
extern float target_R;

extern TwoWire I2C_2;
void my_motor_init();

void my_motor_mode(int controll_mode);

void my_motor_do();