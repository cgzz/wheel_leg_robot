#include <WiFi.h>
#include <Arduino.h>
#include "my_tool.h"
#include <my_control.h>
#include <my_mpu6050.h>
#include "my_udp.h"
#include "my_config.h"
const uint16_t pc_port = 1347;

const uint16_t local_port = 1346;

const uint16_t send_hz = 50;

const char *pc_ip = "192.168.137.1";

WiFiUDP udp;
IPAddress remoteIP;
uint32_t lastSendMs = 0;
uint32_t sendIntervalMs = 1000 / send_hz;
uint32_t seq = 0; // 序号，方便接收端检测丢包（可选）

// 组装 CSV 的逻辑独立出来，便于复用/单元测试
static inline int formatTelemetryCSV(const Telemetry &t, char *buf, size_t sz)
{
    return snprintf(buf, sz, "%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f\n",
                    t.yaw, t.pitch, t.roll,
                    t.err_pos, t.err_spd, t.err_ang,
                    t.wel_spd1, t.wel_spd2,
                    t.tor_base, t.tor_yaw, t.tor_vel);
}

void my_udp_init()
{
    udp.begin(local_port);
    remoteIP.fromString(pc_ip);
}

void my_udp_send()
{
    uint32_t now = millis();
    if (now - lastSendMs >= sendIntervalMs)
    {
        lastSendMs = now;
        // 1) 用当前变量填充结构体（以后你新增字段，只改这里+结构体）
        Telemetry t{.yaw = now_yaw, .pitch = now_pitch, .roll = now_roll, .err_pos = err_pos, .err_spd = err_spd, .err_ang = err_ang, .wel_spd1 = wel_spd1, .wel_spd2 = wel_spd2, .tor_base = tor_base, .tor_yaw = tor_yaw, .tor_vel = tor_vel};
        // 2) 格式化为与 PC 端“完全一致”的 CSV 文本
        char buf[128]; // 96 够用，留点余量更安全
        int n = formatTelemetryCSV(t, buf, sizeof(buf));
        // 3) 发送
        udp.beginPacket(remoteIP, pc_port);
        udp.write((const uint8_t *)buf, n);
        udp.endPacket();
    }
}

// ====== 你的业务变量 ======

// bool key1 = false;
float send1 = 0;
float send2 = 0;
float send3 = 0;
// float joyX = 0; // 摇杆X轴,转向控制[-1,1]
// float joyY = 0; // 摇杆Y轴,前后控制[-1,1]

// 接收并解析一个键值包
void my_udp_receive()
{
    int n;
    char buf[256];
    if (udp.parsePacket() <= 0 || (n = udp.read((uint8_t *)buf, sizeof(buf) - 1)) <= 0) // 如果没有数据包 或 读取失败，就直接返回
        return;
    buf[n] = '\0';
    while (n > 0 && (buf[n - 1] == '\n' || buf[n - 1] == '\r')) // 去掉结尾的 \r 或 \n
        buf[--n] = '\0';
    if (n == 0) // 空行直接返回
        return;
    char *sep = strchr(buf, ':'); // 找冒号
    if (!sep)
        return;  // 找不到冒号
    *sep = '\0'; // 拆 key 和 val
    const char *key = buf;
    const char *val = sep + 1;
    if (strcmp(key, "robot_run") == 0)
        robot_run = (strtof(val, nullptr) != 0.0f);
    else if (strcmp(key, "key1") == 0)
        key1 = (strtof(val, nullptr) != 0.0f);
    else if (strcmp(key, "send1") == 0)
        send1 = strtof(val, nullptr);
    else if (strcmp(key, "send2") == 0)
        send2 = strtof(val, nullptr);
    else if (strcmp(key, "send3") == 0)
        send3 = strtof(val, nullptr);
    else if (strcmp(key, "joy") == 0)
    {
        char *comma = strchr(val, ','); // 拆逗号
        *comma = '\0';
        // joyX = strtof(val, nullptr);
        // joyY = strtof(comma + 1, nullptr);
    }
}
