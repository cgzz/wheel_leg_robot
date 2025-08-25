#pragma once
#include "my_motion.h"
#include "my_mpu6050.h"
#include "SimpleFOC.h"
#include "my_sms.h"
#include "my_io.h"
#include "my_config.h"
#include "my_tool.h"

extern BLDCMotor motor_1;
extern BLDCMotor motor_2;

extern float target_L;
extern float target_R;

struct pid_config
{
    float p, i, d;
    float ramp;  // 斜率
    float limit; // 最值限制
};

struct motor_target
{
    float pos;
    float vel;
    float tor;
};

struct motion_state
{
    float tor;
    float ang;
    float vel;
    float pos;
};
struct joy_state
{
    float x;
    float y;
    float theta;
    float r;
    float x_coef;
    float y_coef;
};
struct fallen_state
{
    bool is;
    int count;
    bool enable;
};
struct test_state
{
    bool is;
    float mode;
    float value;
    float coef;
};

struct robot_state
{
    bool run;
    bool chart_enable;
    float pitch_zero;
    imu_data imu;
    joy_state joy_now;
    joy_state joy_last;
    fallen_state fallen;
    test_state test;
    motion_state robot_now;
    motion_state robot_err;
    motion_state robot_tar;
};

extern pid_config ang_pid;
extern pid_config vel_pid;
extern pid_config tor_pid;
extern pid_config yaw_pid;

extern robot_state my_robot;

// 力矩参数
extern float tor_base;
extern float tor_yaw;
extern float tor_vel;
// 轮速
extern float wel_spd1;
extern float wel_spd2;
extern float wel_spd_avg;

// 遥测频率
extern int telem_hz;

void my_motor_init();

void my_motor_mode(int controll_mode);

void my_motor_do();
