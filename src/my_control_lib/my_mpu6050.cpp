//
// Created by 22480 on 24-12-12.
//
#include "my_io.h"
#include "my_control_config.h"
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
    robot.imu_zero.anglex = robot.imu.anglex;
    robot.imu_zero.angley = robot.imu.angley;
    robot.imu_zero.anglez = robot.imu.anglez;
    robot.imu_zero.gyrox = robot.imu.gyrox;
    robot.imu_zero.gyroy = robot.imu.gyroy;
    robot.imu_zero.gyroz = robot.imu.gyroz;
}
// MPU6050更新,获取状态 角度制
void mpu6050_update()
{
    robot.imu_l.anglex = robot.imu.anglex;
    robot.imu_l.angley = robot.imu.angley;
    robot.imu_l.anglez = robot.imu.anglez;
    robot.imu_l.gyrox = robot.imu.gyrox;
    robot.imu_l.gyroy = robot.imu.gyroy;
    robot.imu_l.gyroz = robot.imu.gyroz;
    mpu6050.update();
    robot.imu.anglex = mpu6050.getAngleX();
    robot.imu.angley = mpu6050.getAngleY();
    robot.imu.anglez = mpu6050.getAngleZ();
    robot.imu.gyrox = mpu6050.getGyroX();
    robot.imu.gyroy = mpu6050.getGyroY();
    robot.imu.gyroz = mpu6050.getGyroZ();
}