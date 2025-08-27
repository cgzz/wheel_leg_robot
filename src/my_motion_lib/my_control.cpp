#include <SimpleFOC.h>
#include "my_motion_config.h"

// PID 控制器 PID参数
PIDController PID_ANG{robot.ang_pid.p, robot.ang_pid.i, 0, robot.ang_pid.k, robot.ang_pid.l};               // 力矩控制
PIDController PID_SPD{robot.spd_pid.p, robot.spd_pid.i, robot.spd_pid.d, robot.spd_pid.k, robot.spd_pid.l}; // 速度控制
PIDController PID_YAW{robot.yaw_pid.p, robot.yaw_pid.i, 0, robot.yaw_pid.k, robot.yaw_pid.l};               // 偏航控制
PIDController PID_POS{robot.pos_pid.p, robot.pos_pid.i, robot.pos_pid.d, robot.pos_pid.k, robot.pos_pid.l}; // 位置控制
// 低通滤波器(截止频率)
LowPassFilter W_SPD_FILTER{0.0180f};
// 状态更新

// new
PIDController pid_roll_angle{8, 0, 0, 100000, 450};
PIDController pid_lqr_u{1, 15, 0, 100000, 8};
LowPassFilter lpf_zeropoint{0.1};
LowPassFilter lpf_joyy{0.2};
LowPassFilter lpf_roll{0.3};

float YAW_angle_total = 0;

void pid_state_update()
{
    // 更新PID控制器状态
    PID_ANG.P = robot.ang_pid.p;
    PID_ANG.I = robot.ang_pid.i;
    // PID_ANG.D = robot.ang_pid.d; 直接使用陀螺仪

    PID_SPD.P = robot.spd_pid.p;
    PID_SPD.I = robot.spd_pid.i;
    PID_SPD.D = robot.spd_pid.d;

    PID_POS.P = robot.pos_pid.p;
    PID_POS.I = robot.pos_pid.i;
    PID_POS.D = robot.pos_pid.d;

    PID_YAW.P = robot.yaw_pid.p;
    PID_YAW.I = robot.yaw_pid.i;
    // PID_YAW.D = robot.yaw_pid.d; 直接使用陀螺仪
}

// 状态更新
void robot_state_update()
{
    // 上一时刻状态
    // 摇杆更新
    robot.joy_l.x = robot.joy.x;
    robot.joy_l.y = robot.joy.y;
    robot.joy_l.a = robot.joy.a;
    robot.joy_l.r = robot.joy.r;
    robot.joy_l.x_coef = robot.joy.x_coef;
    robot.joy_l.y_coef = robot.joy.y_coef;
    // 状态
    robot.ang.last = robot.ang.now;
    robot.spd.last = robot.spd.now;
    robot.pos.last = robot.pos.now;
    robot.yaw.last = robot.yaw.now;
    // 同步机器人状态
    robot.wel.spd1 = motor_1.shaftVelocity(); // rad/s
    robot.wel.spd2 = motor_2.shaftVelocity(); // rad/s
    robot.wel.pos1 = motor_1.shaftAngle();    // rad
    robot.wel.pos2 = motor_2.shaftAngle();    // rad
    // 更新当前状态
    robot.ang.now = robot.imu.angley;
    robot.spd.now = -0.5f * (robot.wel.spd1 + robot.wel.spd2); // rad/s
    robot.pos.now = -0.5f * (robot.wel.pos1 + robot.wel.pos2); // rad
    // robot.yaw.now = ;
}

// 位移零点重置
void robot_pos_control()
{
    if (robot.joy.y != 0) // 有前后方向运动指令时的处理
    {
        robot.pos.tar = robot.pos.now; // 位移零点重置
        pid_lqr_u.error_prev = 0;      // 输出积分清零
    }
    if ((robot.joy_l.x != 0 && robot.joy.x == 0) || (robot.joy_l.y != 0 && robot.joy.y == 0)) // 运动指令复零时的原地停车处理
        robot.joy_stop_control = true;
    if ((robot.joy_stop_control == true) && (abs(robot.spd.now) < 0.5))
    {
        robot.pos.tar = robot.pos.now; // 位移零点重置
        robot.joy_stop_control = false;
    }
    if (abs(robot.spd.now) > 15)       // 被快速推动时的原地停车处理
        robot.pos.tar = robot.pos.now; // 位移零点重置
}

void pitch_control()
{
    // 角度环
    robot.ang.tar = robot.pitch_zero;
    robot.ang.err = robot.ang.now - robot.ang.tar;
    robot.ang.tor = PID_ANG(robot.ang.err) + my_lim(robot.ang_pid.d * robot.imu.gyroy, robot.ang_pid.l);
    // 速度环
    robot.spd.err = robot.spd.now - robot.spd.tar; // robot.vel.tar = 0
    robot.spd.tor = PID_SPD(robot.spd.err - robot.joy.y_coef * lpf_joyy(robot.joy.y));
    // 位置环
    robot.pos.err = robot.pos.now - robot.pos.tar; // m
    robot.pos.tor = PID_POS(robot.pos.err);
    // 轮部离地检测
    if (abs(robot.spd.now - robot.spd.last) > 10 || abs(robot.spd.now) > 50) // 若轮部角速度、角加速度过大或处于跳跃后的恢复时期，认为出现轮部离地现象，需要特殊处理
    {
        robot.pos.tar = robot.pos.now; // 位移零点重置
        robot.tor.base = robot.ang.tor;
        pid_lqr_u.error_prev = 0; // 输出积分清零
    }
    else
        robot.tor.base = robot.ang.tor + robot.pos.tor + robot.spd.tor;
    // 触发条件：遥控器无信号输入、轮部位移控制正常介入、不处于跳跃后的恢复时期
    if (abs(robot.tor.base) < 5 && robot.joy.y == 0 && abs(robot.pos.tor) < 4)
    {
        robot.tor.base = pid_lqr_u(robot.tor.base);                          // 小转矩补偿
        robot.pitch_zero -= my_lim(0.002 * lpf_zeropoint(robot.pos.tor), 4); // 重心自适应
    }
    else
        pid_lqr_u.error_prev = 0; // 输出积分清零
}
// 偏航角控制
void yaw_control()
{
    YAW_angle_total += robot.imu.anglez - robot.imu_l.anglez + robot.joy.x * robot.joy.x_coef; // 遥控器输入叠加;
    robot.tor.yaw = PID_YAW(YAW_angle_total) + my_lim(robot.yaw_pid.d * robot.imu.gyroz, robot.yaw_pid.l);
}
// 速度阻尼
void vel_control()
{
    robot.tor.spd = robot.spd.now * 0;
}
// 力矩求和
void torque_add()
{
    robot.tor.L = -0.5f * my_lim(robot.tor.base + robot.tor.yaw, TOR_SUM_LIM);
    robot.tor.R = -0.5f * my_lim(robot.tor.base - robot.tor.yaw, TOR_SUM_LIM);
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
        robot.tor.L = 0;
        robot.tor.R = 0;
    }
}

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
    robot.sms.Position[0] = my_lim(robot.sms.Position[0], 2110, 2510);
    robot.sms.Position[1] = my_lim(robot.sms.Position[1], 1586, 1986);
    my_sms_update();
}

// 测试模式
void test_mode()
{
    if (robot.test.enable) // 测试模式
    {
        motor_1.controller = (MotionControlType)robot.test.foc_mode;
        motor_2.controller = (MotionControlType)robot.test.foc_mode;
        robot.tor.L = robot.test.motor1;
        robot.tor.R = robot.test.motor2;
        // TODO 舵机暂时只做了位置模式
        // robot.sms.Position[0] = 2048 + 12 - robot.test.servo1;
        // robot.sms.Position[1] = 2048 - 12 - robot.test.servo2;
        robot.test.active = true;
    }
    else if (!robot.test.enable && robot.test.active) // 停止测试模式
    {
        robot.tor.L = 0.0f;
        robot.tor.R = 0.0f;
        motor_1.controller = torque; // 恢复为力矩控制
        motor_2.controller = torque; // 恢复为力矩控制
        robot.sms.Position[0] = 2048 + 12;
        robot.sms.Position[1] = 2048 - 12;
        robot.test.active = false;
    }
}