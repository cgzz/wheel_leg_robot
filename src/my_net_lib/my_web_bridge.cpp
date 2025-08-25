#include "my_net_config.h"

web_data bridge_data = {
    .keys = {"angP", "angI", "angD", "spdP", "spdI", "spdD", "posP", "posI", "posD", "yawP", "yawI", "yawD", "zero"},
    .send = {
        .chart_config = {{"角速度", {"", "Y", ""}},
                         {"角度跟踪", {"误差角度", "当前角度", ""}},
                         {"电机输出", {"目标", "实际速度", ""}}},
        .slider_group = {{"摇摆模式", {"晃动强度系数", "", ""}},
                         {"", {"", "", ""}},
                         {"", {"", "", ""}},
                         {"", {"", "", ""}}},
        .fallen = false,
        .msg = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        .slide = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    },
    .rece = {
        .telem_hz = 10,
        .run = false,
        .chart_enable = false,
        .fallen_enable = false,
        .slide = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        .test_enable = false,
        .test_mode = 0,
        .test_value = 0,
        .joyx = 0,
        .joyy = 0,
        .joytheta = 0,
    },
};

// ========== UI 配置 JSON 打包（仅首连时发送一次） ==========
JsonDocument cb_ui_config()
{
    JsonDocument doc;
    doc["type"] = "ui_config";
    // charts
    JsonArray charts = doc["charts"].to<JsonArray>();

    for (const auto &c : bridge_data.send.chart_config)
    {
        JsonObject o = charts.add<JsonObject>();
        o["title"] = c.title;
        JsonArray l = o["legends"].to<JsonArray>();
        for (int i = 0; i < 3; ++i)
            l.add(c.legend[i]);
    }
    // sliders
    JsonArray sliders = doc["sliders"].to<JsonArray>();
    for (const auto &sg : bridge_data.send.slider_group)
    {
        JsonObject g = sliders.add<JsonObject>();
        g["group"] = sg.group;
        JsonArray n = g["names"].to<JsonArray>();
        for (int i = 0; i < 3; ++i)
            n.add(sg.names[i]);
    }
    doc.shrinkToFit(); // 发送前收紧空间，减轻带宽
    return doc;
}
// 12+1 路遥测数据（自定义映射：电流/角速度/位置/温度...）
uint32_t my_web_data_update()
{
    JsonDocument doc;
    uint32_t hz = bridge_data.rece.telem_hz ? bridge_data.rece.telem_hz : 1;
    uint32_t dt_ms = 1000 / hz;
    // 组包 -> 广播
    doc["type"] = "telemetry";
    doc["fallen"] = bridge_data.send.fallen;
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
    return dt_ms; // 返回下次采样间隔（ms）
}
// PID 设置（顺序：角度P/I/D，速度P/I/D，位置P/I/D）
void cb_pid_set(JsonObject param)
{
    if (!param.isNull())
        for (int i = 0; i < 13; i++)
            bridge_data.rece.slide[i] = param[bridge_data.keys[i]] | bridge_data.rece.slide[i]; // 没有就用原始数据
    // TODO PID 应用到控制器
}
// PID 读取
JsonDocument cb_pid_get()
{
    JsonDocument out;
    JsonObject pr = out["param"].to<JsonObject>();
    out["type"] = "pid";
    for (int i = 0; i < 13; i++)
        pr[bridge_data.keys[i]] = bridge_data.send.slide[i];
    return out;
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
