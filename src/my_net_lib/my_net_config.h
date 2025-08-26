#pragma once

#include <Arduino.h>
#include <ArduinoJson.h>
#include <ESPAsyncWebServer.h>
#include <FS.h>
#include <LittleFS.h>
#include "my_net.h"

// 变量暴露
// 文件系统
#define FSYS LittleFS
// 以下是参数定义
// web刷新限制
#define REFRESH_RATE_DEF 10
#define REFRESH_RATE_MAX 60
#define REFRESH_RATE_MIN 1

struct ChartConfig
{
    const char *title;
    const char *legend[3];
};

struct SliderGroup
{
    const char *group;
    const char *names[3];
};

struct web_send
{
    ChartConfig chart_config[3];
    SliderGroup slider_group[5];
    float fallen;
    float voltage;
    float msg[12];
    float slide[15];
};

struct web_rece
{
    bool run;
    bool chart_enable;
    bool fallen_enable;

    test_state test;

    joy_state joy;

    float slide[15];
};

struct web_data
{
    const char *keys[15];
    int telem_hz;
    int dt_ms;
    web_send send;
    web_rece rece;
};

extern web_data bridge_data;

// 异步服务器对象
extern AsyncWebServer server;
extern AsyncWebSocket ws;

// UI 配置打包
void my_web_data_update();
void cb_pid_set(JsonObject param);
void cb_pid_get(AsyncWebSocketClient *c);
void cb_testmode(JsonObject param);
void cb_joystick(float x, float y, float a);
// fs函数
static String contentType(const String &path);
// webtool函数
void wsSendTo(AsyncWebSocketClient *c, const JsonDocument &doc);
void my_wsheart();
bool wsCanBroadcast();
void wsBroadcast(const JsonDocument &doc);
bool handleFileRead(AsyncWebServerRequest *req, String path);
// tool函数
float my_math_deadband(float value, float deadband);
float my_math_limit(float value, float min, float max);
void rtrim_inplace(char *s);

// wifi
void my_wifi_init();

// web
void my_web_asyn_init();