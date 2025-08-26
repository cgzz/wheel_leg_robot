#include "my_motion_config.h"
#include "my_sms.h"
#include "my_mpu6050.h"
#include "my_motor.h"

robot_state robot = {
    .run = false,                       // 运行指示位
    .chart_enable = false,              // 图表推送位
    .pitch_zero = 0,                    // pitch零点
    .imu = imu_raw,                     // IMU数据
    .wel = {0, 0, 0, 0, 0},             // 轮子数据
    .joy_now = {0, 0, 0, 0, 0.1, 0.1},  // 遥控数据now
    .joy_last = {0, 0, 0, 0, 0.1, 0.1}, // 遥控数据last
    .fallen = {
        .is = false,     // 是否摔倒
        .count = 0,      // 摔倒检测计数
        .enable = false, // 摔倒检测器开关
    },
    .test = {
        .is = false,      // 是否处于测试模式
        .is_last = false, // 上一时刻是否处于测试模式
        .mode = 0,        // 电机模式
        .value = 0,       // 测试值
        .coef = 1,        // 测试值系数
    },
    .ang = {0, 0, 0}, // 直立环状态
    .vel = {0, 0, 0}, // 速度环状态
    .pos = {0, 0, 0}, // 位置环状态
    .yaw = {0, 0, 0}, // 偏航环状态

    .ang_pid = {0, 0, 0, 8, 1000},   // 直立环参数
    .vel_pid = {0, 0, 0, 8, 1000},   // 速度环参数
    .pos_pid = {0, 0, 0, 1.5, 1000}, // 位置环参数
    .yaw_pid = {0, 0, 0, 2, 1000},   // 偏航环参数
};

robot_state get_robot()
{
    return robot;
}
void my_motion_init()
{
    // 初始化6050 角度制(yaw0, pitch0, roll0)
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
    // 电机执行
    my_motor_do();
}
