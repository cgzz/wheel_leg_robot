#pragma once
// 库函数
#include "Arduino.h"
#include "SimpleFOC.h"
// 模块头文件
#include "my_motion.h"
// 模块内头文件
#include "my_mpu6050.h"
#include "my_motor.h"
#include "my_sms.h"
// 全局头文件
#include "my_io.h"
#include "my_config.h"
#include "my_tool.h"

struct pid_config
{
    float p, i, d;
    float k; // 斜率
    float l; // 最值限制
};
struct motor_target
{
    float pos;
    float vel;
    float tor;
};
struct motion_state
{
    float now;
    float tar;
    float err;
};
struct joy_state
{
    float x;
    float y;
    float a; // 方向角
    float r;
    float x_coef;
    float y_coef;
};
struct fallen_state
{
    bool is;     // 是否摔倒
    int count;   // 计数
    bool enable; // 检测是否启用
};
struct test_state
{
    bool is;      // 是否打开测试模式
    bool is_last; // 上一时刻是否是测试模式
    float mode;   // 0 力矩 1 速度 2角度
    float value;  // 测试值
    float coef;   // 测试值系数
};
struct motor_tor
{
    float base;
    float yaw;
    float vel;
    float motor_L;
    float motor_R;
};
struct wel_data
{
    float spd1;
    float spd2;
    float spd_avg;
    float pos1;
    float pos2;
};
struct robot_state
{
    bool run;
    bool chart_enable;
    float pitch_zero;

    motor_tor tor;

    imu_data imu;

    wel_data wel;

    joy_state joy_now;
    joy_state joy_last;

    fallen_state fallen;

    test_state test;

    motion_state ang;
    motion_state vel;
    motion_state pos;
    motion_state yaw;

    pid_config ang_pid;
    pid_config vel_pid;
    pid_config pos_pid;
    pid_config yaw_pid;
};

extern robot_state robot;

void test_mode();
void fall_check();
void pitch_control();
void robot_state_update();
void yaw_control();
void vel_control();
void torque_add();