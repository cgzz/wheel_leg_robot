#include "my_net_config.h"
// 包含外部库
#include "my_motion.h"
#include "my_bat.h"
// 发送和接受数据定义
// PID滑块
#define SLIDER1 robot.ang_pid.p
#define SLIDER2 robot.ang_pid.i
#define SLIDER3 robot.ang_pid.d
#define SLIDER4 robot.vel_pid.p
#define SLIDER5 robot.vel_pid.i
#define SLIDER6 robot.vel_pid.d
#define SLIDER7 robot.pos_pid.p
#define SLIDER8 robot.pos_pid.i
#define SLIDER9 robot.pos_pid.d
#define SLIDER10 robot.yaw_pid.p
#define SLIDER11 robot.yaw_pid.i
#define SLIDER12 robot.yaw_pid.d
#define SLIDER13 robot.pitch_zero
// 发送数据
#define FALLEN robot.fallen.is
#define VOLTAGE battery
#define ANGLE_X robot.imu.angley
#define ANGLE_Y robot.imu.anglex
#define ANGLE_Z robot.imu.anglez
#define CHART_DATA11 robot.ang.now
#define CHART_DATA12 robot.ang.tar
#define CHART_DATA13 robot.ang.err
#define CHART_DATA21 robot.vel.now
#define CHART_DATA22 robot.vel.tar
#define CHART_DATA23 robot.vel.err
#define CHART_DATA31 robot.pos.now
#define CHART_DATA32 robot.pos.tar
#define CHART_DATA33 robot.pos.err
#define CHART_DATA41 robot.yaw.now
#define CHART_DATA42 robot.yaw.tar
#define CHART_DATA43 robot.yaw.err
#define CHART_DATA51 robot.pitch_zero
// 接受数据
#define ROBOT_RUN robot.run
#define ROBOT_CHART robot.chart_enable
#define FALLEN_ENABLE robot.fallen.enable
#define TEST_ENABLE robot.test.enable
#define TEST_MODE robot.test.mode
#define TEST_VALUE robot.test.value
#define JOY_X robot.joy_now.x
#define JOY_Y robot.joy_now.y
#define JOY_A robot.joy_now.a

ChartConfig chart[3] = {{"直立环", {"now", "tar", "err"}},
                        {"速度环", {"now", "tar", "err"}},
                        {"位置环", {"now", "tar", "err"}}};

SliderGroup slider[4] = {{"直立环", {"P", "I", "D"}},
                         {"速度环", {"P", "I", "D"}},
                         {"位置环", {"P", "I", "D"}},
                         {"偏航环", {"P", "I", "D"}}};

web_data bridge_data = {
    .keys = {"key01", "key02", "key03", "key04", "key05", "key06", "key07", "key08", "key09", "key10", "key11", "key12", "key13", "key14", "key15"},
    .telem_hz = 10,
    .dt_ms = 100,
    .send = {
        .chart_config = {{"", {"", "", ""}},
                         {"", {"", "", ""}},
                         {"", {"", "", ""}}},
        .slider_group = {{"", {"", "", ""}},
                         {"", {"", "", ""}},
                         {"", {"", "", ""}},
                         {"", {"", "", ""}}},
        .fallen = false,
        .voltage = 0,
        .msg = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        .slide = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    },
    .rece = {
        .run = false,
        .chart_enable = false,
        .fallen_enable = false,
        .test = {false, false, 0, 0, 0, 0, 0, 0},
        .joy = {0, 0, 0, 0, 0, 0},
        .slide = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    },
};

void my_net_init()
{
    my_wifi_init();     // 初始化网络
    my_web_asyn_init(); // 初始化网页
    // 初始化UI数据
    bridge_data.send.chart_config[0] = chart[0];
    bridge_data.send.chart_config[1] = chart[1];
    bridge_data.send.chart_config[2] = chart[2];
    bridge_data.send.slider_group[0] = slider[0];
    bridge_data.send.slider_group[1] = slider[1];
    bridge_data.send.slider_group[2] = slider[2];
    bridge_data.send.slider_group[3] = slider[3];
}

void send_data_update()
{
    // 发送数据更新
    bridge_data.send.fallen = FALLEN;
    bridge_data.send.voltage = VOLTAGE;
    bridge_data.send.msg[0] = ANGLE_X;       // pitch
    bridge_data.send.msg[1] = ANGLE_Y;       // roll
    bridge_data.send.msg[2] = ANGLE_Z;       // yaw
    bridge_data.send.msg[3] = CHART_DATA11;  // 直立环值
    bridge_data.send.msg[4] = CHART_DATA12;  // 直立环目标
    bridge_data.send.msg[5] = CHART_DATA13;  // 直立环误差
    bridge_data.send.msg[6] = CHART_DATA21;  // 速度环值
    bridge_data.send.msg[7] = CHART_DATA22;  // 速度环目标
    bridge_data.send.msg[8] = CHART_DATA23;  // 速度环误差
    bridge_data.send.msg[9] = CHART_DATA31;  // 位置环值
    bridge_data.send.msg[10] = CHART_DATA32; // 位置环目标
    bridge_data.send.msg[11] = CHART_DATA33; // 位置环误差
    // pid数据读取
    bridge_data.send.slide[0] = SLIDER1;
    bridge_data.send.slide[1] = SLIDER2;
    bridge_data.send.slide[2] = SLIDER3;
    bridge_data.send.slide[3] = SLIDER4;
    bridge_data.send.slide[4] = SLIDER5;
    bridge_data.send.slide[5] = SLIDER6;
    bridge_data.send.slide[6] = SLIDER7;
    bridge_data.send.slide[7] = SLIDER8;
    bridge_data.send.slide[8] = SLIDER9;
    bridge_data.send.slide[9] = SLIDER10;
    bridge_data.send.slide[10] = SLIDER11;
    bridge_data.send.slide[11] = SLIDER12;
    bridge_data.send.slide[12] = SLIDER13;
}

void rece_data_update()
{
    // 遥测频率更新
    bridge_data.dt_ms = 1000 / (bridge_data.telem_hz ? bridge_data.telem_hz : 1);
    // 开关检测
    ROBOT_RUN = bridge_data.rece.run;
    ROBOT_CHART = bridge_data.rece.chart_enable;
    FALLEN_ENABLE = bridge_data.rece.fallen_enable;
    TEST_ENABLE = bridge_data.rece.test_enable;
    // 测试模式读取
    TEST_MODE = bridge_data.rece.test_mode;
    TEST_VALUE = bridge_data.rece.test_value;
    // JOY获取
    JOY_X = bridge_data.rece.joyx;
    JOY_Y = bridge_data.rece.joyy;
    JOY_A = bridge_data.rece.joytheta;
    // slider获取
    SLIDER1 = bridge_data.rece.slide[0];
    SLIDER2 = bridge_data.rece.slide[1];
    SLIDER3 = bridge_data.rece.slide[2];
    SLIDER4 = bridge_data.rece.slide[3];
    SLIDER5 = bridge_data.rece.slide[4];
    SLIDER6 = bridge_data.rece.slide[5];
    SLIDER7 = bridge_data.rece.slide[6];
    SLIDER8 = bridge_data.rece.slide[7];
    SLIDER9 = bridge_data.rece.slide[8];
    SLIDER10 = bridge_data.rece.slide[9];
    SLIDER11 = bridge_data.rece.slide[10];
    SLIDER12 = bridge_data.rece.slide[11];
    SLIDER13 = bridge_data.rece.slide[12];
}
uint32_t my_net_update()
{
    send_data_update();
    my_web_data_update();
    rece_data_update();
    return bridge_data.dt_ms;
}