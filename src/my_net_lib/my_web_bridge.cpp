#include "my_net_config.h"

// 12+2 路遥测数据
void my_web_data_update()
{
    JsonDocument doc;

    // 组包 -> 广播
    doc["type"] = "telemetry";
    doc["fallen"] = FALLEN;
    doc["voltage"] = VOLTAGE;
    doc["pitch"] = ANGLE_X;
    doc["roll"] = ANGLE_Y;
    doc["yaw"] = ANGLE_Z;
    // 根据 charts_send 决定是否打包 n 路曲线数据
    if (robot.chart_enable)
    {
        JsonArray arr = doc["d"].to<JsonArray>();
        arr.add(CHART_11);
        arr.add(CHART_12);
        arr.add(CHART_13);
        arr.add(CHART_21);
        arr.add(CHART_22);
        arr.add(CHART_23);
        arr.add(CHART_31);
        arr.add(CHART_32);
        arr.add(CHART_33);
    }
    wsBroadcast(doc);
}
// PID 设置（顺序：角度P/I/D，速度P/I/D，位置P/I/D）
void web_pid_set(JsonObject param)
{
    SLIDER_11 = param["key01"].as<float>();
    SLIDER_12 = param["key02"].as<float>();
    SLIDER_13 = param["key03"].as<float>();
    SLIDER_21 = param["key04"].as<float>();
    SLIDER_22 = param["key05"].as<float>();
    SLIDER_23 = param["key06"].as<float>();
    SLIDER_31 = param["key07"].as<float>();
    SLIDER_32 = param["key08"].as<float>();
    SLIDER_33 = param["key09"].as<float>();
    SLIDER_41 = param["key10"].as<float>();
    SLIDER_42 = param["key11"].as<float>();
    SLIDER_43 = param["key12"].as<float>();
    SLIDER_51 = param["key13"].as<float>();
    SLIDER_52 = param["key14"].as<float>();
    pid_state_update();
}
// PID 读取
void web_pid_get(AsyncWebSocketClient *c)
{
    JsonDocument out;
    JsonObject pr = out["param"].to<JsonObject>();
    out["type"] = "pid";
    pr["key01"] = SLIDER_11;
    pr["key02"] = SLIDER_12;
    pr["key03"] = SLIDER_13;
    pr["key04"] = SLIDER_21;
    pr["key05"] = SLIDER_22;
    pr["key06"] = SLIDER_23;
    pr["key07"] = SLIDER_31;
    pr["key08"] = SLIDER_32;
    pr["key09"] = SLIDER_33;
    pr["key10"] = SLIDER_41;
    pr["key11"] = SLIDER_42;
    pr["key12"] = SLIDER_43;
    pr["key13"] = SLIDER_51;
    pr["key14"] = SLIDER_52;

    wsSendTo(c, out);
}
// 测试模式
void web_testmode(JsonObject param)
{
    robot.test.enable = param["enabled"].as<bool>();
    robot.test.foc_mode = param["foc_mode"].as<int>();
    robot.test.ser_mode = param["ser_mode"].as<int>();
    robot.test.motor1 = param["motor1"].as<float>();
    robot.test.motor2 = param["motor2"].as<float>();
    robot.test.servo1 = param["servo1"].as<float>();
    robot.test.servo2 = param["servo2"].as<float>();
}
// 摇杆
void web_joystick(float x, float y, float a)
{
    robot.joy.x = my_db(my_lim(x, -1.0f, 1.0f), 0.02f);
    robot.joy.y = my_db(my_lim(y, -1.0f, 1.0f), 0.02f);
    robot.joy.a = my_db(my_lim(a, -1.0f, 1.0f), 0.02f);
}
