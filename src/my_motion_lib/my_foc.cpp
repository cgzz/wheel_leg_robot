#include "Arduino.h"
#include "my_motion_config.h"

BLDCMotor motor_1 = BLDCMotor(7);
BLDCMotor motor_2 = BLDCMotor(7);

MagneticSensorI2C sensor_1 = MagneticSensorI2C(AS5600_I2C);
MagneticSensorI2C sensor_2 = MagneticSensorI2C(AS5600_I2C);

void my_motor_init()
{
    sensor_1.init(&I2C_1);
    sensor_2.init(&I2C_2);
    // 连接motor对象与编码器对象
    motor_1.linkSensor(&sensor_1);
    motor_2.linkSensor(&sensor_2);
    // 速度环PID参数
    motor_1.PID_velocity.P = 0.05;
    motor_1.PID_velocity.I = 1;
    motor_1.PID_velocity.D = 0;

    motor_2.PID_velocity.P = 0.05;
    motor_2.PID_velocity.I = 1;
    motor_2.PID_velocity.D = 0;

    motor_1.LPF_velocity.Tf = 0.02f;
    motor_2.LPF_velocity.Tf = 0.02f;
    motor_1.shaft_angle;
    // 驱动器设置
    motor_1.voltage_sensor_align = 6;
    motor_2.voltage_sensor_align = 6;
    driver_1.voltage_power_supply = 8;
    driver_2.voltage_power_supply = 8;

    motor_1.voltage_limit = 8.0f; // 与供电/驱动余量匹配
    motor_2.voltage_limit = 8.0f;

    driver_1.init();
    driver_2.init();

    // 连接motor对象与驱动器对象
    motor_1.linkDriver(&driver_1);
    motor_2.linkDriver(&driver_2);

    motor_1.torque_controller = voltage;
    motor_2.torque_controller = voltage;
    motor_1.controller = torque;
    motor_2.controller = torque;
    // motor_1.controller = velocity;
    // motor_2.controller = velocity;
    // monitor相关设置
    motor_1.useMonitoring(Serial);
    motor_2.useMonitoring(Serial);
    // 电机初始化
    motor_1.init();
    motor_1.initFOC();
    motor_2.init();
    motor_2.initFOC();
    Serial.println("电机初始化完成");
}

void my_motor_do()
{
    motor_1.target = robot.tor.motor_L;
    motor_2.target = robot.tor.motor_R;
    // 迭代计算FOC相电压
    motor_1.loopFOC();
    motor_2.loopFOC();
    // 设置轮部电机输出
    motor_1.move();
    motor_2.move();
}
