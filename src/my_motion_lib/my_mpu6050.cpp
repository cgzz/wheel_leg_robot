//
// Created by 22480 on 24-12-12.
//
#include "my_mpu6050.h"
#include "my_io.h"
// MPU6050实例
MPU6050 mpu6050(I2C_2);
imu_data imu_raw = {
    .anglex0 = 0,
    .angley0 = 0,
    .anglez0 = 0,
    .anglex = 0,
    .angley = 0,
    .anglez = 0,
    .gyrox = 0,
    .gyroy = 0,
    .gyroz = 0,
};
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
    imu_raw.anglex0 = imu_raw.anglex;
    imu_raw.angley0 = imu_raw.angley;
    imu_raw.anglez0 = imu_raw.anglez;
}
// MPU6050更新,获取状态 角度制
void mpu6050_update()
{
    mpu6050.update();
    imu_raw.anglex = mpu6050.getAngleX();
    imu_raw.angley = mpu6050.getAngleY();
    imu_raw.anglez = mpu6050.getAngleZ();
    imu_raw.gyrox = mpu6050.getGyroX();
    imu_raw.gyroy = mpu6050.getGyroY();
    imu_raw.gyroz = mpu6050.getGyroZ();
}