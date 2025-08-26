#include "my_motion_config.h"
#include "my_sms.h"
#include "my_mpu6050.h"
#include "my_foc.h"

robot_state robot = {
    // 状态指示位
    .run = false,              // 运行指示位
    .chart_enable = false,     // 图表推送位
    .joy_stop_control = false, // 原地停车标志
    .wel_up = false,           // 轮部离地标志
    // 修正位
    .height = 38,          // 身高
    .pitch_zero = 0,       // pitch零点
    .leg_position_add = 0, // 腿部位置修正值
    // 轮子/舵机/电机
    .wel = {0, 0, 0, 0},                                 // 轮子数据 wel1 , wel2, pos1, pos2
    .tor = {0, 0, 0, 0, 0},                              // 电机力矩 base, yaw, spd, motor_L, motor_R
    .sms = {{1, 2}, {2148, 1948}, {300, 300}, {30, 30}}, // ID, Position, Speed, ACC
    // IMU数据 anglex, angley, anglez, gyrox, gyroy, gyroz
    .imu_zero = {0, 0, 0, 0, 0, 0},
    .imu_l = {0, 0, 0, 0, 0, 0},
    .imu = {0, 0, 0, 0, 0, 0},
    // 摇杆控制 x, y, a, r, x_coef, y_coef
    .joy = {0, 0, 0, 0, 0.015, 0.015},
    .joy_l = {0, 0, 0, 0, 0.015, 0.015},
    // 摔倒检测 is, count, enable
    .fallen = {false, 0, false},
    // 测试模式 enable, active, foc_mode, servo_mode, motor1, motor2, servo1, servo2
    .test = {false, false, 0, 0, 0, 0, 0, 0},
    // pid状态检测 now,last,target,error,torque
    .ang = {0, 0, 0, 0, 0}, // 直立环状态
    .spd = {0, 0, 0, 0, 0}, // 速度环状态
    .pos = {0, 0, 0, 0, 0}, // 位置环状态
    .yaw = {0, 0, 0, 0, 0}, // 偏航环状态
    // pid参数设定
    .ang_pid = {1, 0, 0.06, 100000, 8}, // 直立环参数
    .spd_pid = {0.7, 0, 0, 100000, 8},  // 速度环参数
    .pos_pid = {0.5, 0, 0, 100000, 8},  // 位置环参数
    .yaw_pid = {1, 0, 0.04, 100000, 2}, // 偏航环参数
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
    // 位移零点计算
    robot_pos_control();
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
    {
        robot.tor.L = 0.0f;
        robot.tor.R = 0.0f;
        // robot.sms.Position[0] = 2048 + 12;
        // robot.sms.Position[1] = 2048 - 12;
    }
    // 舵机执行
    leg_update();
    // 电机执行
    my_motor_do();
}
