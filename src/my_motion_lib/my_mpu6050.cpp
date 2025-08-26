//
// Created by 22480 on 24-12-12.
//
#include "my_mpu6050.h"
#include "my_io.h"
#include "my_motion_config.h"
// MPU6050实例
MPU6050 mpu6050(I2C_2);

// 初始状态

void mpu6050_init()
{
    // mpu6050设置
    mpu6050.begin();
    mpu6050.calcGyroOffsets(true);
    Serial.println("MPU6050初始化完成");
    // 让IMU稳定一下再锁定初始航向（可按需要延长/多次均值）
    delay(200);
    mpu6050_setzero();
    Serial.println("MPU6050初始状态已更新");
}

void mpu6050_setzero()
{
    mpu6050_update();
    robot.imu.anglex0 = robot.imu.anglex;
    robot.imu.angley0 = robot.imu.angley;
    robot.imu.anglez0 = robot.imu.anglez;
}
// MPU6050更新,获取状态 角度制
void mpu6050_update()
{
    mpu6050.update();
    robot.imu.anglex = mpu6050.getAngleX();
    robot.imu.angley = mpu6050.getAngleY();
    robot.imu.anglez = mpu6050.getAngleZ();
    robot.imu.gyrox = mpu6050.getGyroX();
    robot.imu.gyroy = mpu6050.getGyroY();
    robot.imu.gyroz = mpu6050.getGyroZ();
}