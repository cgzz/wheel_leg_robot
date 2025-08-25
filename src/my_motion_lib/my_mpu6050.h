#pragma once
#include <MPU6050_tockn.h>
// MPU6050实例
void mpu6050_init();
void mpu6050_setzero();
void mpu6050_update();

struct imu_data
{
    float anglex0;
    float angley0;
    float anglez0;
    float anglex;
    float angley;
    float anglez;
    float gyrox;
    float gyroy;
    float gyroz;
};

extern imu_data imu_raw;