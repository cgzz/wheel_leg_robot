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
static TaskHandle_t control_TaskHandle = nullptr;   // 运动控制
static TaskHandle_t data_send_TaskHandle = nullptr; // 网页任务
static TaskHandle_t bat_check_TaskHandle = nullptr; // 电池检测任务

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
        uint32_t dt_ms = my_net_update();
        vTaskDelay(pdMS_TO_TICKS(dt_ms));
    }
}

void bat_check_Task(void *)
{
    for (;;)
    {
        bat_update();
        vTaskDelay(pdMS_TO_TICKS(BAT_CHECK_TIME));
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
    xTaskCreatePinnedToCore(robot_control_Task, "ctrl_2ms", 8192, nullptr, 15, &control_TaskHandle, 0); // 初始化运动任务
    xTaskCreatePinnedToCore(data_send_Task, "telem", 8192, nullptr, 5, &data_send_TaskHandle, 1);
    xTaskCreatePinnedToCore(bat_check_Task, "bat_check", 1024, nullptr, 4, &bat_check_TaskHandle, 1);
}

void loop()
{
}
