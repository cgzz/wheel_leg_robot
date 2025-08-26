#pragma once

#include "my_tool.h"
#include "my_motion.h"
#include "freertos/FreeRTOS.h"

// wifi定义
#define SSID "BIEGUOQU"
#define PASSWORD "qazwsxedcr"
// 图表显示

// bridge参数

// 函数定义
void my_net_init();

uint32_t my_net_update();

// void my_wsheart(); 心跳函数
