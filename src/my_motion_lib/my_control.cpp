#include <SimpleFOC.h>
#include "my_motion_config.h"

// PID 控制器 PID参数
PIDController PID_ANG{robot.ang_pid.p, robot.ang_pid.i, 0, robot.ang_pid.k, robot.ang_pid.l};               // 力矩控制
PIDController PID_SPD{robot.vel_pid.p, robot.vel_pid.i, robot.vel_pid.d, robot.vel_pid.k, robot.vel_pid.l}; // 速度控制
PIDController PID_YAW{robot.yaw_pid.p, robot.yaw_pid.i, 0, robot.yaw_pid.k, robot.yaw_pid.l};               // 偏航控制
PIDController PID_POS{robot.pos_pid.p, robot.pos_pid.i, robot.pos_pid.d, robot.pos_pid.k, robot.pos_pid.l}; // 位置控制
// 低通滤波器(截止频率)
LowPassFilter W_SPD_FILTER{0.0180f};
// 状态更新
void robot_state_update()
{
    // 同步IMU和状态器
    robot.ang.now = robot.imu.angley;
    // 获取轮子速度
    robot.wel.spd1 = motor_1.shaftVelocity();                                   // rad/s
    robot.wel.spd2 = motor_2.shaftVelocity();                                   // rad/s
    robot.wel.spd_avg = W_SPD_FILTER(0.5f * (robot.wel.spd1 + robot.wel.spd2)); // 滤波
    robot.wel.pos1 = motor_1.shaftAngle();                                      // rad
    robot.wel.pos2 = motor_2.shaftAngle();                                      // rad
}
void pid_state_update()
{
    // 更新PID控制器状态
    PID_ANG.P = robot.ang_pid.p;
    PID_ANG.I = robot.ang_pid.i;
    PID_ANG.D = robot.ang_pid.d;
    PID_SPD.P = robot.vel_pid.p;
    PID_SPD.I = robot.vel_pid.i;
    PID_SPD.D = robot.vel_pid.d;
    PID_POS.P = robot.pos_pid.p;
    PID_POS.I = robot.pos_pid.i;
    PID_POS.D = robot.pos_pid.d;
    PID_YAW.P = robot.yaw_pid.p;
    PID_YAW.I = robot.yaw_pid.i;
    PID_YAW.D = robot.yaw_pid.d;
}

// 测试模式
void test_mode()
{
    if (robot.test.enable) // 测试模式
    {
        motor_1.controller = (MotionControlType)robot.test.foc_mode;
        motor_2.controller = (MotionControlType)robot.test.foc_mode;
        robot.tor.motor_L = robot.test.motor1;
        robot.tor.motor_R = robot.test.motor2;
        // TODO 舵机暂时只做了位置模式
        // robot.sms.Position[0] = 2048 + 12 - robot.test.servo1;
        // robot.sms.Position[1] = 2048 - 12 - robot.test.servo2;
        robot.test.active = true;
    }
    else if (!robot.test.enable && robot.test.active) // 停止测试模式
    {
        robot.tor.motor_L = 0.0f;
        robot.tor.motor_R = 0.0f;
        motor_1.controller = torque; // 恢复为力矩控制
        motor_2.controller = torque; // 恢复为力矩控制
        robot.sms.Position[0] = 2048 + 12;
        robot.sms.Position[1] = 2048 - 12;
        robot.test.active = false;
    }
}
// 俯仰角控制
void pitch_control()
{
    // 求速度和位置
    robot.pos.now = WHEEL_R * robot.wel.spd_avg; // m/s
    robot.pos.now += DT * robot.vel.now;
    // 位置环
    robot.pos.err = robot.pos.tar - robot.pos.now; // m
    robot.vel.tar = PID_POS(robot.pos.err);        // m/s
    // 速度环
    robot.vel.err = robot.vel.tar - robot.vel.now;
    robot.vel.err = my_math_limit(robot.vel.err, -SPD_ERR_LIM, +SPD_ERR_LIM);
    robot.vel.err = my_math_deadband(robot.vel.err, SPD_ERR_DEAD);
    robot.ang.tar = PID_SPD(robot.vel.err) + robot.pitch_zero;
    // 角度环
    robot.ang.err = robot.ang.tar - robot.ang.now;
    robot.ang.err = my_math_limit(robot.ang.err, -ANG_ERR_LIM, +ANG_ERR_LIM);
    robot.tor.base = PID_ANG(robot.ang.err) - robot.ang_pid.d * robot.imu.gyroy;  // 基础力矩（左右相同）
    robot.tor.base = my_math_limit(robot.tor.base, -TOR_BASE_LIM, +TOR_BASE_LIM); // 力矩限制
}
// 偏航角控制
void yaw_control()
{
    robot.yaw.err = my_math_deadband(robot.imu.anglez0 - robot.imu.anglez, YAW_ERR_DEAD); // 死区
    // 偏航差动输出（PID + 角速度阻尼）
    robot.tor.yaw = PID_YAW(robot.yaw.err) - robot.yaw_pid.d * robot.imu.gyroz; // >0 则左轮加力、右轮减力（视 YAW_TORQUE_SIGN）
    robot.tor.yaw = my_math_limit(robot.tor.yaw, -TOR_YAW_LIM, +TOR_YAW_LIM);
}
// 速度阻尼
void vel_control()
{
    robot.tor.vel = robot.wel.spd_avg * 0;
}
// 力矩求和
void torque_add()
{
    float tor_base = pid_lqr_u(robot.tor.base - robot.tor.vel);
    robot.tor.motor_L = -0.5f * my_math_limit(tor_base + robot.tor.yaw, -TOR_SUM_LIM, +TOR_SUM_LIM);
    robot.tor.motor_R = -0.5f * my_math_limit(tor_base - robot.tor.yaw, -TOR_SUM_LIM, +TOR_SUM_LIM);
}
// 倒地检测
void fall_check()
{
    // 摔倒检测
    if (!robot.fallen.enable) // 没有启用直接返回
        return;
    if (robot.imu.angley > FALL_MAX_PITCH || robot.imu.angley < FALL_MIN_PITCH)
        robot.fallen.count++;
    else
    {
        robot.fallen.count = 0;
        robot.fallen.is = false;
    }
    if (robot.fallen.count >= COUNT_FALL_MAX)
    {
        robot.fallen.is = true;
        robot.tor.motor_L = 0;
        robot.tor.motor_R = 0;
    }
}

PIDController pid_roll_angle{8, 0, 0, 100000, 450};
LowPassFilter lpf_roll{0.3};

// 腿部动作控制
void leg_update()
{
    robot.sms.ACC[0] = 8, robot.sms.ACC[1] = 8;
    robot.sms.Speed[0] = 200, robot.sms.Speed[1] = 200;
    // 机身高度自适应控制
    // leg_position_add += pid_roll_angle(roll_angle);
    robot.leg_position_add = pid_roll_angle(lpf_roll(robot.imu.anglex + 2.0)); // test
    robot.sms.Position[0] = 2048 + 12 + 8.4 * (robot.height - 32) - robot.leg_position_add;
    robot.sms.Position[1] = 2048 - 12 - 8.4 * (robot.height - 32) - robot.leg_position_add;
    // 限幅
    robot.sms.Position[0] = my_math_limit(robot.sms.Position[0], 2110, 2510);
    robot.sms.Position[1] = my_math_limit(robot.sms.Position[1], 1586, 1986);
    my_sms_update();
}