#pragma once

#include "my_tool.h"
#include "my_control.h"
#include "freertos/FreeRTOS.h"
#include "my_bat.h"

// wifi定义
#define SSID "BIEGUOQU"
#define PASSWORD "qazwsxedcr"
// 函数定义
void my_wifi_init();       // 初始化网络
void my_web_asyn_init();   // 初始化网页
void my_web_data_update(); // 数据更新

// 图表显示模式
#define CHART_PID
// #define CHART_FOC

// 遥测推送数据============================================
// 推送数据
#define FALLEN robot.fallen.is
#define VOLTAGE battery
#define ANGLE_X robot.imu.angley
#define ANGLE_Y robot.imu.anglex
#define ANGLE_Z robot.imu.anglez

#ifdef CHART_PID
// 发送和接受数据定义
#define CHART_NAME1 "直立环"
#define CHART_NAME11 "now"
#define CHART_11 robot.ang.now
#define CHART_NAME12 "tor"
#define CHART_12 robot.ang.tor
#define CHART_NAME13 "err"
#define CHART_13 robot.ang.err

#define CHART_NAME2 "速度环"
#define CHART_NAME21 "now"
#define CHART_21 robot.spd.now
#define CHART_NAME22 "tor"
#define CHART_22 robot.spd.tor
#define CHART_NAME23 "err"
#define CHART_23 robot.spd.err

#define CHART_NAME3 "位置环"
#define CHART_NAME31 "now"
#define CHART_31 robot.pos.now
#define CHART_NAME32 "tor"
#define CHART_32 robot.pos.tor
#define CHART_NAME33 "err"
#define CHART_33 robot.pos.err

#endif

#ifdef CHART_FOC
#define CHART_NAME1 "L"
#define CHART_11 robot.tor.L
#define CHART_NAME11 "tor"
#define CHART_12 robot.wel.spd1
#define CHART_NAME12 "spd"
#define CHART_13 robot.wel.pos1
#define CHART_NAME13 "pos"

#define CHART_NAME2 "R"
#define CHART_21 robot.tor.R
#define CHART_NAME21 "tor"
#define CHART_22 robot.wel.spd2
#define CHART_NAME22 "spd"
#define CHART_23 robot.wel.pos2
#define CHART_NAME23 "pos"

#define CHART_NAME3 "0"
#define CHART_31 0
#define CHART_NAME31 "0"
#define CHART_32 0
#define CHART_NAME32 "0"
#define CHART_33 0
#define CHART_NAME33 "0"
#endif

// 滑块关联数据 =============================================
#define SLIDER_NAME1 "直立环"
#define SLIDER_NAME11 "P"
#define SLIDER_11 robot.ang_pid.p
#define SLIDER_NAME12 "I"
#define SLIDER_12 robot.ang_pid.i
#define SLIDER_NAME13 "D"
#define SLIDER_13 robot.ang_pid.d
#define SLIDER_NAME2 "速度环"
#define SLIDER_NAME21 "P"
#define SLIDER_21 robot.spd_pid.p
#define SLIDER_NAME22 "I"
#define SLIDER_22 robot.spd_pid.i
#define SLIDER_NAME23 "D"
#define SLIDER_23 robot.spd_pid.d
#define SLIDER_NAME3 "位置环"
#define SLIDER_NAME31 "P"
#define SLIDER_31 robot.pos_pid.p
#define SLIDER_NAME32 "I"
#define SLIDER_32 robot.pos_pid.i
#define SLIDER_NAME33 "D"
#define SLIDER_33 robot.pos_pid.d
#define SLIDER_NAME4 "偏航环"
#define SLIDER_NAME41 "P"
#define SLIDER_41 robot.yaw_pid.p
#define SLIDER_NAME42 "I"
#define SLIDER_42 robot.yaw_pid.i
#define SLIDER_NAME43 "D"
#define SLIDER_43 robot.yaw_pid.d
#define SLIDER_NAME5 "其他"
#define SLIDER_NAME51 "pitch_zero"
#define SLIDER_51 robot.pitch_zero
#define SLIDER_NAME52 "hight"
#define SLIDER_52 robot.height
#define SLIDER_NAME53 ""
