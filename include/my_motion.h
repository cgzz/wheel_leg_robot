#pragma once
#include "Arduino.h"
#include "SMS_STS.h"
// 定义机器前倾为正pitch 轮子旋转向前为正wel 向前移动为正pos
// 角度环参数
#define ANG_ERR_LIM 10.0f // 角度环误差限幅（避免过激）
#define ANG_ERR_DEAD 0.1f // 角度环死区（去抖）
#define ANG_TAR_LIM 10.0f
#define ANG_TAR_DEAD 0.1f
// 速度环参数
#define SPD_ERR_LIM 1.0f // 速度环限幅（避免过激）
#define SPD_ERR_DEAD 0.1f
// 偏航控制参数
// #define YAW_ERR_LIM 0.5f // 误差限幅（避免过激）
#define YAW_ERR_DEAD 0.1f // 误差死区（去抖）
// 力矩限制
#define TOR_BASE_LIM 8.0f
#define TOR_YAW_LIM 2.0f
#define TOR_SUM_LIM 10.0f
// 摔倒检测参数
#define COUNT_FALL_MAX 5 // 连续5次采样超限才算倒地
#define FALL_MAX_PITCH +60.0f
#define FALL_MIN_PITCH -40.0f

struct servo_data
{
    byte ID[2];
    s16 Position[2];
    u16 Speed[2];
    byte ACC[2];
};

struct imu_data
{
    float anglex;
    float angley;
    float anglez;
    float gyrox;
    float gyroy;
    float gyroz;
};

struct pid_config
{
    float p, i, d;
    float k; // 斜率
    float l; // 最值限制
};
struct motor_target
{
    float pos;
    float spd;
    float tor;
};
struct motion_state
{
    float now;
    float last;
    float tar;
    float err;
    float tor;
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
    bool enable;  // 是否打开测试模式
    bool active;  // 上一时刻是否是测试模式
    int foc_mode; // 0 力矩 1 速度 2角度
    int ser_mode; // 0 力矩 1 速度 2角度
    float motor1;
    float motor2;
    float servo1;
    float servo2;
};
struct motor_tor
{
    float base;
    float yaw;
    float spd;
    float L;
    float R;
};
struct wel_data
{
    float spd1;
    float spd2;
    float pos1;
    float pos2;
};
struct robot_state
{
    bool run;
    bool chart_enable;
    bool joy_stop_control;
    bool wel_up;

    int height;
    float pitch_zero;
    float leg_position_add;

    wel_data wel;
    motor_tor tor;
    servo_data sms;

    imu_data imu_zero;
    imu_data imu_l;
    imu_data imu;

    joy_state joy;
    joy_state joy_l;

    fallen_state fallen;

    test_state test;

    motion_state ang;
    motion_state spd;
    motion_state pos;
    motion_state yaw;

    pid_config ang_pid;
    pid_config spd_pid;
    pid_config pos_pid;
    pid_config yaw_pid;
};

extern robot_state robot;
void my_motion_init();
void my_motion_update();

void pid_state_update();