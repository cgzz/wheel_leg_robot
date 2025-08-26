#include "my_net_config.h"

// 12+2 路遥测数据
void my_web_data_update()
{
    JsonDocument doc;

    // 组包 -> 广播
    doc["type"] = "telemetry";
    doc["fallen"] = bridge_data.send.fallen;
    doc["voltage"] = bridge_data.send.voltage;
    doc["pitch"] = bridge_data.send.msg[0];
    doc["roll"] = bridge_data.send.msg[1];
    doc["yaw"] = bridge_data.send.msg[2];
    // 根据 charts_send 决定是否打包 n 路曲线数据
    if (bridge_data.rece.chart_enable)
    {
        JsonArray arr = doc["d"].to<JsonArray>();
        for (int i = 3; i < 12; i++)
            arr.add(bridge_data.send.msg[i]);
    }
    wsBroadcast(doc);
}
// PID 设置（顺序：角度P/I/D，速度P/I/D，位置P/I/D）
void cb_pid_set(JsonObject param)
{
    if (!param.isNull())
        for (int i = 0; i < 13; i++)
            bridge_data.rece.slide[i] = param[bridge_data.keys[i]].as<float>();
    ; // 没有就用原始数据
}
// PID 读取
void cb_pid_get(AsyncWebSocketClient *c)
{
    JsonDocument out;
    JsonObject pr = out["param"].to<JsonObject>();
    out["type"] = "pid";
    for (int i = 0; i < 13; i++)
        pr[bridge_data.keys[i]] = bridge_data.send.slide[i];
    wsSendTo(c, out);
}
// 测试模式
void cb_testmode(bool enabled, int mode, float value)
{
    bridge_data.rece.test_enable = enabled;
    bridge_data.rece.test_value = value;
    bridge_data.rece.test_mode = mode;
}
// 摇杆
void cb_joystick(float x, float y, float a)
{
    bridge_data.rece.joyx = my_math_deadband(my_math_limit(x, -1.0f, 1.0f), 0.02f);
    bridge_data.rece.joyy = my_math_deadband(my_math_limit(y, -1.0f, 1.0f), 0.02f);
    bridge_data.rece.joytheta = my_math_deadband(my_math_limit(a, -1.0f, 1.0f), 0.02f);
}
