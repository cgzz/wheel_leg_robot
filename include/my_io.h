#pragma once

#include "Arduino.h"
#include <Wire.h>
#include <SimpleFOC.h>

// 编码器实例
extern TwoWire I2C_1;
extern TwoWire I2C_2; // 同时和6050复用
// PWM驱动器
extern BLDCDriver3PWM driver_1;
extern BLDCDriver3PWM driver_2;

void my_io_init();
