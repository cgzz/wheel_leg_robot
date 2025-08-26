#include "Arduino.h"
#include "Wire.h"
#include "SimpleFOC.h"

// 编码器实例
TwoWire I2C_1 = TwoWire(0);
TwoWire I2C_2 = TwoWire(1);

// PWM驱动器
BLDCDriver3PWM driver_1 = BLDCDriver3PWM(32, 33, 25, 22);
BLDCDriver3PWM driver_2 = BLDCDriver3PWM(26, 27, 14, 12);

void my_io_init()
{
    Serial.begin(115200);
    Serial.setTimeout(5); // ms
    Serial.println("串口初始化完成");

    I2C_1.begin(19, 18, 400000UL);
    I2C_2.begin(23, 5, 400000UL);
    Serial.println("I2C初始化完成");
}
