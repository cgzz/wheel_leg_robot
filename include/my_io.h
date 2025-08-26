#pragma once

#include "Arduino.h"
#include <Wire.h>
#include <SimpleFOC.h>

#define BAT_PIN 35 // select the input pin for the ADC
#define LED_PIN 14 // select the output pin for the LED

// 编码器实例
extern TwoWire I2C_1;
extern TwoWire I2C_2; // 同时和6050复用
// PWM驱动器
extern BLDCDriver3PWM driver_1;
extern BLDCDriver3PWM driver_2;

void my_io_init();
