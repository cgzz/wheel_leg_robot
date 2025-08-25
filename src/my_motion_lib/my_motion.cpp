#include "my_motion_config.h"
#include "my_sms.h"
#include "my_mpu6050.h"
#include "my_motor.h"

void my_motion_init()
{
    mpu6050_init();  // 初始化6050 角度制(yaw0, pitch0, roll0)
    my_sms_init();   // 初始化舵机
    my_motor_init(); // 初始化电机 力矩模式
}

void robot_state_update()
{
}

void my_motion_update()
{
    mpu6050_update();
    robot_state_update();
    pitch_control(); // pitch 平衡控制
    yaw_control();   // yaw 保持控制
    vel_control();   // 速度阻尼控制
    torque_add();    // 力矩计算
    fall_check();    // 停机检测
    test_mode();
    my_motor_do();
}
