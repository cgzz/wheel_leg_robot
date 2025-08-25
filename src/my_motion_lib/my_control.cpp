#include <SimpleFOC.h>
#include "my_motion_config.h"

pid_config ang_pid = {0, 0, 0, 8, 1000};
pid_config vel_pid = {0, 0, 0, 8, 1000};
pid_config pos_pid = {0, 0, 0, 1.5, 1000};
pid_config yaw_pid = {0, 0, 0, 2, 1000};
joy_state joy_now = {0, 0, 0, 0, 0.1, 0.1};
joy_state joy_last = {0, 0, 0, 0, 0.1, 0.1};
motion_state robot_now = {0, 0, 0, 0};
motion_state robot_err = {0, 0, 0, 0};
motion_state robot_tar = {0, 0, 0, 0};
robot_state my_robot = {
    .run = false,
    .chart_enable = false,
    .pitch_zero = 0,
    .imu = imu_raw,
    .fallen = {
        .is = false,
        .count = 0,
        .enable = false,
    },
    .test = {
        .is = false,
        .mode = 0,
        .value = 0,
        .coef = 1,
    },
};

float control_pos = 0.0f;
float control_spd = 0.0f;
float control_pitch = 0.0f;
float control_gyroY = 0.0f;

// PID 控制器 PID参数
PIDController PID_ANG{ang_pid.p, ang_pid.i, 0, ang_pid.ramp, ang_pid.limit};         // 力矩控制
PIDController PID_SPD{vel_pid.p, vel_pid.i, vel_pid.d, vel_pid.ramp, vel_pid.limit}; // 速度控制
PIDController PID_YAW{yaw_pid.p, yaw_pid.i, 0, yaw_pid.ramp, yaw_pid.limit};         // 偏航控制
PIDController PID_POS{pos_pid.p, pos_pid.i, pos_pid.d, pos_pid.ramp, pos_pid.limit}; // 位置控制

PIDController PID_ZERO{0.002, 0, 0, 100000, 4};

PIDController PID_TOR_BASE{1, 15, 0, 100000, 8};

// 低通滤波器(截止频率)
LowPassFilter W_SPD_FILTER{0.0180f};
LowPassFilter GYROY_FILTER{0.0035f};
LowPassFilter GYROZ_FILTER{0.0053f};
LowPassFilter lpf_zeropoint{0.1};
LowPassFilter lpf_joyy{0.2};

//
float last_spd0 = 0.0f;
float last_spd1 = 0.0f;
// 力矩变量定义
float tor_yaw = 0.0f;  // 转向力矩
float tor_base = 0.0f; // 基础力矩
float tor_vel = 0.0f;  // 速度力矩
// 偏航控制变量
float err_yaw = 0.0f; //[,] °
// 俯仰控制变量
// 轮速
float wel_spd1 = 0.0f;    //[,] rad/s
float wel_spd2 = 0.0f;    //[,] rad/s
float wel_spd_raw = 0.0f; //[,] rad/s
float wel_spd_avg = 0.0f; //[,] rad/s
// float pitch = 0.0f;  //已在my_mpu6050中定义 //[,] °
//
float last_yaw = 0.0f; // 上次偏航角
float yaw_angle_total = 0;


// 控制主函数
void test_mode()
{
    if (my_robot.test.is)
    {
        motor_1.controller = (MotionControlType)my_robot.test.mode;
        motor_2.controller = (MotionControlType)my_robot.test.mode;
        target_L = my_robot.test.coef * my_robot.test.value;
        target_R = my_robot.test.coef * my_robot.test.value;
    }
    if (!my_robot.test.is) // 停止测试模式
    {
        target_L = 0.0f;
        target_R = 0.0f;
        motor_1.controller = torque; // 恢复为力矩控制
        motor_2.controller = torque; // 恢复为力矩控制
    }
}
void pid_control_update()
{
}
// 俯仰角控制
void pitch_control()
{
    robot_now.ang = my_robot.imu.angley;
    // 获取轮子速度
    wel_spd1 = motor_1.shaftVelocity(); // rad/s
    wel_spd2 = motor_2.shaftVelocity(); // rad/s
    wel_spd_raw = 0.5f * (wel_spd1 + wel_spd2);
    // 滤波
    wel_spd_avg = W_SPD_FILTER(wel_spd_raw);
    // 求速度和位置
    robot_now.pos = WHEEL_R * wel_spd_avg; // m/s
    robot_now.pos += DT * robot_now.vel;
    // 位置环
    robot_err.pos = robot_tar.pos - robot_now.pos; // m
    robot_tar.vel = PID_POS(robot_err.pos);        // m/s
    // 速度环
    robot_err.vel = robot_tar.vel - robot_now.vel;
    robot_err.vel = my_math_limit(robot_err.vel, -SPD_ERR_LIM, +SPD_ERR_LIM);
    robot_err.vel = my_math_deadband(robot_err.vel, SPD_ERR_DEAD);
    robot_tar.ang = PID_SPD(robot_err.vel) + my_robot.pitch_zero;
    // 角度环
    robot_err.ang = robot_tar.ang - robot_now.ang;
    robot_err.ang = my_math_limit(robot_err.ang, -ANG_ERR_LIM, +ANG_ERR_LIM);
    tor_base = PID_ANG(robot_err.ang) - ang_pid.d * my_robot.imu.gyroy; // 基础力矩（左右相同）
    tor_base = my_math_limit(tor_base, -TOR_BASE_LIM, +TOR_BASE_LIM);   // 力矩限制
}
// 偏航角控制
void yaw_control()
{
    err_yaw = my_math_deadband(my_robot.imu.anglez0 - my_robot.imu.anglez, YAW_ERR_DEAD); // 死区
    // 偏航差动输出（PID + 角速度阻尼）
    tor_yaw = PID_YAW(err_yaw) - yaw_pid.d * my_robot.imu.gyroz; // >0 则左轮加力、右轮减力（视 YAW_TORQUE_SIGN）
    tor_yaw = my_math_limit(tor_yaw, -TOR_YAW_LIM, +TOR_YAW_LIM);
}
void vel_control()
{
    tor_vel = wel_spd_avg * 0;
}
void torque_add()
{
    target_L = -0.5f * my_math_limit(tor_base - tor_vel + tor_yaw, -TOR_SUM_LIM, +TOR_SUM_LIM);
    target_R = -0.5f * my_math_limit(tor_base - tor_vel - tor_yaw, -TOR_SUM_LIM, +TOR_SUM_LIM);
}

// 倒地检测
void fall_check()
{
    // 摔倒检测
    if (!fall_check)
        return;
    if (my_robot.imu.angley > FALL_MAX_PITCH || my_robot.imu.angley < FALL_MIN_PITCH)
        my_robot.fallen.count++;
    else
    {
        my_robot.fallen.count = 0;
        my_robot.fallen.is = false;
    }
    if (my_robot.fallen.count >= COUNT_FALL_MAX)
    {
        my_robot.fallen.is = true;
        target_L = 0, target_R = 0;
    }
}