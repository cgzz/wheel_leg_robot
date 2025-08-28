
#include "my_control_config.h"
// STS舵机实例
SMS_STS sms_sts;

// 舵机初始化
// 舵机有效行程450
// 左侧舵机[2048+12+50,2048+12+450]
// 左侧舵机[2048-12-50,2048-12-450]
/// 舵机初始化
void my_sms_init()
{
    sms_sts.pSerial = &Serial2;
    my_sms_update(); // 舵机(ID1/ID2)以最高速度V=2400步/秒，加速度A=50(50*100步/秒^2)，运行至各自的Position位置
}

void my_sms_update()
{
    sms_sts.SyncWritePosEx(robot.sms.ID, 2, robot.sms.Position, robot.sms.Speed, robot.sms.ACC);
}
