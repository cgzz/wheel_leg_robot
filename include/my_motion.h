#pragma once
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
// 结构体定义

void my_motion_init();
void my_motion_update();

void test_mode();
void pid_control_update();
void fall_check();
void my_control();
void pitch_control();
void yaw_control();
void vel_control();
void torque_add();