#include "my_motion_config.h"
#include "my_sms.h"
#include "my_mpu6050.h"
#include "my_foc.h"

robot_state robot = {
    // 状态指示位
    .run = false,          // 运行指示位
    .chart_enable = false, // 图表推送位
    // 修正位
    .height = 38,          // 身高
    .pitch_zero = 0,       // pitch零点
    .leg_position_add = 0, // 腿部位置修正值
    // 轮子/舵机/电机/IMU
    .tor = {0, 0, 0, 0, 0},                              // 电机力矩 base, yaw, vel, motor_L, motor_R
    .sms = {{1, 2}, {2148, 1948}, {300, 300}, {30, 30}}, // ID, Position, Speed, ACC
    .imu = {0, 0, 0, 0, 0, 0, 0, 0, 0},                  // anglex0, angley0, anglez0,anglex, angley, anglez, gyrox, gyroy, gyroz
    .wel = {0, 0, 0, 0, 0},                              // 轮子数据 wel1 , wel2, wel_avg, pos1, pos2
    // 摇杆控制
    .joy_now = {0, 0, 0, 0, 0.1, 0.1},  // x, y, a, r, x_coef, y_coef
    .joy_last = {0, 0, 0, 0, 0.1, 0.1}, // x, y, a, r, x_coef, y_coef
    // 摔倒检测
    .fallen = {false, 0, false}, // is, count, enable
    // 测试模式
    .test = {false, false, 0, 0, 0, 0, 0, 0}, // enable, active, foc_mode, servo_mode, motor1, motor2, servo1, servo2
    // pid状态检测
    .ang = {0, 0, 0}, // 直立环状态
    .vel = {0, 0, 0}, // 速度环状态
    .pos = {0, 0, 0}, // 位置环状态
    .yaw = {0, 0, 0}, // 偏航环状态
    // pid参数设定
    .ang_pid = {0, 0, 0, 8, 1000},   // 直立环参数
    .vel_pid = {0, 0, 0, 8, 1000},   // 速度环参数
    .pos_pid = {0, 0, 0, 1.5, 1000}, // 位置环参数
    .yaw_pid = {0, 0, 0, 2, 1000},   // 偏航环参数
};

void my_motion_init()
{
    // 初始化6050
    mpu6050_init();
    // 初始化舵机
    my_sms_init();
    // 初始化电机 力矩模式
    my_motor_init();
}

void my_motion_update()
{
    // 更新imu数据
    mpu6050_update();
    // 更新robot状态数据
    robot_state_update();
    // pitch平衡控制计算
    pitch_control();
    // yaw 保持控制
    yaw_control();
    // 速度阻尼控制
    vel_control();
    // 力矩计算
    torque_add();
    // 摔倒检测
    fall_check();
    // 测试模式
    test_mode();
    // 运行检查
    if (!robot.run)
        return;
    // 舵机执行
    leg_update();
    // 电机执行
    my_motor_do();
}
