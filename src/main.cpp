// 加载配置
#include "Arduino.h"
#include "freertos/FreeRTOS.h"
// 加载我的头文件
#include "my_config.h"
#include "my_io.h"
#include "my_bat.h"
#include "my_net.h"
#include "my_motion.h"
#include "my_tool.h"

// 任务配置
static TaskHandle_t control_TaskHandle = nullptr;
static TaskHandle_t data_send_TaskHandle = nullptr; // 遥测 FreeRTOS 任务

void robot_control_Task(void *)
{
    const TickType_t kPeriod = pdMS_TO_TICKS(DT * 1000); // 2ms 固定周期
    TickType_t lastWake = xTaskGetTickCount();
    for (;;)
    {
        my_motion_update();
        vTaskDelayUntil(&lastWake, kPeriod);
    }
}

void data_send_Task(void *)
{
    for (;;)
    {
        my_net_update();
        uint32_t dt_ms = my_web_data_update();
        vTaskDelay(pdMS_TO_TICKS(dt_ms));
    }
}

void setup()
{
    // 初始化IO
    my_io_init();
    // 初始化网络
    my_net_init();
    // 初始化运动
    my_motion_init();
    // 任务初始化
    xTaskCreatePinnedToCore(robot_control_Task, "ctrl_2ms", 8192, nullptr, 20, &control_TaskHandle, 0); // 初始化运动任务
    xTaskCreatePinnedToCore(data_send_Task, "telem", 8192, nullptr, 1, &data_send_TaskHandle, 1);
}

void loop()
{
}
