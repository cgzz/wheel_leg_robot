
#include "my_sms.h"

// STS舵机实例
SMS_STS sms_sts;
byte ID[2];
s16 Position[2];
u16 Speed[2];
byte ACC[2];
/// 舵机初始化
void my_sms_init()
{
    // 舵机初始化
    // 舵机有效行程450
    // 左侧舵机[2048+12+50,2048+12+450]
    // 左侧舵机[2048-12-50,2048-12-450]
    Serial2.begin(1000000); // 腿部sts舵机
    sms_sts.pSerial = &Serial2;
    ID[0] = 1;
    ID[1] = 2;
    my_sms_set(30, 30, 300, 300, 2148, 1948); // 设置舵机加速度、速度和位置
    my_sms_do();                              // 舵机(ID1/ID2)以最高速度V=2400步/秒，加速度A=50(50*100步/秒^2)，运行至各自的Position位置
}

void my_sms_do()
{
    sms_sts.SyncWritePosEx(ID, 2, Position, Speed, ACC);
}

void my_sms_set(byte acc0, byte acc1, u16 speed0, u16 speed1, s16 position0, s16 position1)
{
    ACC[0] = acc0;
    ACC[1] = acc1;
    Speed[0] = speed0;
    Speed[1] = speed1;
    Position[0] = position0;
    Position[1] = position1;
}