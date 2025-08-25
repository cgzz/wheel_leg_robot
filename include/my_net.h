#pragma once

#include "my_tool.h"
#include "freertos/FreeRTOS.h"

// wifi定义
#define SSID "BIEGUOQU"
#define PASSWORD "qazwsxedcr"
// 图表显示

// bridge参数

// 函数定义
void my_net_init();

void my_net_update();

void my_web_asyn_init();

void my_wifi_init();

void my_net_update();

uint32_t my_web_data_update();

// void my_wsheart(); 心跳函数
