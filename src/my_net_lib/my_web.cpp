#include "my_net_config.h"
// ======================= 内部状态 =======================
// Web/WS 服务实例（仅本翻译单元可见）
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

ChartConfig chart_config[3] = {{CHART_NAME1, {CHART_NAME11, CHART_NAME12, CHART_NAME13}},
                               {CHART_NAME2, {CHART_NAME21, CHART_NAME22, CHART_NAME23}},
                               {CHART_NAME3, {CHART_NAME31, CHART_NAME32, CHART_NAME33}}};

SliderGroup slider_group[5] = {{SLIDER_NAME1, {SLIDER_NAME11, SLIDER_NAME12, SLIDER_NAME13}},
                               {SLIDER_NAME2, {SLIDER_NAME21, SLIDER_NAME22, SLIDER_NAME23}},
                               {SLIDER_NAME3, {SLIDER_NAME31, SLIDER_NAME32, SLIDER_NAME33}},
                               {SLIDER_NAME4, {SLIDER_NAME41, SLIDER_NAME42, SLIDER_NAME43}},
                               {SLIDER_NAME5, {SLIDER_NAME51, SLIDER_NAME52, SLIDER_NAME53}}};

// ======================= 事件处理 =======================
// 连接事件：仅在 WS_EVT_CONNECT 时发送一次 UI 配置；其后不再发送
void we_evt_connect(AsyncWebSocket *s, AsyncWebSocketClient *c, AwsEventType type, void *arg, uint8_t *data, size_t len)
{
    JsonDocument doc;
    doc["type"] = "ui_config";
    // charts
    JsonArray charts = doc["charts"].to<JsonArray>();

    for (int i = 0; i < 3; ++i)
    {
        JsonObject o = charts.add<JsonObject>();
        o["title"] = chart_config[i].title;
        JsonArray l = o["legends"].to<JsonArray>();
        for (int j = 0; j < 3; ++j)
            l.add(chart_config[i].legend[j]);
    }
    // sliders
    JsonArray sliders = doc["sliders"].to<JsonArray>();
    for (int i = 0; i < 5; ++i)
    {
        JsonObject g = sliders.add<JsonObject>();
        g["group"] = slider_group[i].group;
        JsonArray n = g["names"].to<JsonArray>();
        for (int j = 0; j < 3; ++j)
            n.add(slider_group[i].names[j]);
    }
    doc.shrinkToFit(); // 发送前收紧空间，减轻带宽
    // 先发 ui_config（首连一次，配置标题/图例/分组名称）
    wsSendTo(c, doc);
    // 再发一个简单的 info，便于前端状态显示
    JsonDocument ack;
    ack["type"] = "info";
    ack["text"] = "connected";
    wsSendTo(c, ack);
}

// 断联事件
void we_evt_disconnect(AsyncWebSocket *s, AsyncWebSocketClient *c, AwsEventType type, void *arg, uint8_t *data, size_t len)
{
    // 可按需记录日志或清理该 client 的状态
}

// 消息事件
void ws_evt_data(AsyncWebSocket *s, AsyncWebSocketClient *c, AwsEventType type, void *arg, uint8_t *data, size_t len)
{
    AwsFrameInfo *info = (AwsFrameInfo *)arg;
    JsonDocument doc;
    DeserializationError e = deserializeJson(doc, data, len);
    const char *typeStr = doc["type"] | "";
    // 仅处理完整文本帧
    if (!(info->final && info->index == 0 && info->len == len))
        return;
    if (info->opcode != WS_TEXT)
        return;
    if (e)
        return;
    if (!*typeStr)
        return;
    // 调试用
    Serial.println("--- Printing JsonDocument ---");
    serializeJsonPretty(doc, Serial);

    // ===========逻辑处理区域===========
    // 1) 设置遥测频率（上限锁 60Hz，避免队列堆积）
    if (!strcmp(typeStr, "telem_hz"))
        robot.data_ms = 1000 / my_lim(doc["ms"], REFRESH_RATE_MIN, REFRESH_RATE_MAX);

    // 2) 运行开关（只影响执行器；不影响遥测是否发送）
    else if (!strcmp(typeStr, "robot_run"))
        robot.run = doc["running"] | false; // 默认关闭

    // 3) 图表推送开关（关闭时后台仅发 3 路，显著减载）
    else if (!strcmp(typeStr, "charts_send"))
        robot.chart_enable = doc["on"] | false; // 默认关闭

    // 4) 测试模式：仅解析并回调（不保状态/不实现逻辑）
    else if (!strcmp(typeStr, "test_mode"))
        web_testmode(doc.as<JsonObject>());

    // 5) 摔倒检测开关
    else if (!strcmp(typeStr, "fall_check"))
        robot.fallen.enable = doc["enable"];

    // 6) 姿态零偏（预留）
    else if (!strcmp(typeStr, "imu_restart"))
        return; // TODO

    // 7) 摇杆
    else if (!strcmp(typeStr, "joy"))
        web_joystick(doc["x"] | 0.0f, doc["y"] | 0.0f, doc["a"] | 0.0f);

    // 8) 设置 PID
    else if (!strcmp(typeStr, "set_pid"))
        web_pid_set(doc["param"].as<JsonObject>());

    // 9) 读取 PID（回填给前端）
    else if (!strcmp(typeStr, "get_pid"))
        web_pid_get(c);
}

// ping/pong事件
void ws_evt_pong(AsyncWebSocket *s, AsyncWebSocketClient *c, AwsEventType type, void *arg, uint8_t *data, size_t len)
{
    // 可选：心跳统计
}

// 错误事件
void ws_evt_error(AsyncWebSocket *s, AsyncWebSocketClient *c, AwsEventType type, void *arg, uint8_t *data, size_t len)
{
    // 可选：错误日志
}

// ======================= WebSocket 事件绑定 =======================
static void onWsEvent(AsyncWebSocket *s, AsyncWebSocketClient *c, AwsEventType type, void *arg, uint8_t *data, size_t len)
{
    switch (type)
    {
    case WS_EVT_CONNECT:
        we_evt_connect(s, c, type, arg, data, len);
        break;
    case WS_EVT_DISCONNECT:
        we_evt_disconnect(s, c, type, arg, data, len);
        break;
    case WS_EVT_DATA:
        ws_evt_data(s, c, type, arg, data, len);
        break;
    case WS_EVT_PONG:
        ws_evt_pong(s, c, type, arg, data, len);
        break;
    case WS_EVT_ERROR:
        ws_evt_error(s, c, type, arg, data, len);
        break;
    default:
        break;
    }
}

// ======================= HTTP 处理器 =======================
static void handleApiState(AsyncWebServerRequest *req)
{
    JsonDocument d;
    String s;
    d["ms"] = robot.data_ms;
    d["running"] = robot.run;
    d["test_mode"] = robot.test.enable;
    d["chart_enable"] = robot.chart_enable;
    d["fallen_enable"] = robot.fallen.enable;
    serializeJson(d, s);
    req->send(200, "application/json; charset=utf-8", s);
}

static void handleRootRequest(AsyncWebServerRequest *req)
{
    if (!handleFileRead(req, "/"))
        req->send(404, "text/plain; charset=utf-8", "home4.html not found");
}

static void handleNotFound(AsyncWebServerRequest *req)
{
    if (!handleFileRead(req, req->url()))
        req->send(404, "text/plain; charset=utf-8", "404 Not Found");
}

// ======================= 路由 & 初始化入口 =======================
void my_web_asyn_init()
{
    if (!FSYS.begin(true)) // 1) 文件系统
        Serial.println("[WEB] LittleFS mount failed (formatted?)");

    ws.onEvent(onWsEvent); // 2) WebSocket
    server.addHandler(&ws);

    server.on("/api/state", HTTP_GET, handleApiState); // 3) 基础 API
    server.on("/", HTTP_GET, handleRootRequest);       // 4) 静态文件
    server.onNotFound(handleNotFound);
    server.begin(); // 5) 启动 HTTP
}
