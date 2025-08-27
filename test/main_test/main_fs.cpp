#include "LittleFS.h"

void setup()
{
    Serial.begin(115200);
    if (!LittleFS.begin())
    {
        Serial.println("LittleFS Mount Failed");
        return;
    }
    // --- 写入示例 ---
    File configFile = LittleFS.open("/config.txt", "w");
    if (!configFile)
    {
        Serial.println("Failed to open config file for writing");
        return;
    }
    configFile.println("ssid=MyWiFi");
    configFile.println("password=12345678");
    configFile.close(); // **不要忘记关闭！**
    Serial.println("Config file written.");
    // --- 读取示例 ---
    configFile = LittleFS.open("/config.txt", "r");
    if (!configFile)
    {
        Serial.println("Failed to open config file for reading");
        return;
    }
    Serial.println("Reading config file:");
    while (configFile.available())
    {
        String line = configFile.readStringUntil('\n');
        Serial.println(line);
    }
    configFile.close(); // **读取后也要关闭！**
}

void loop()
{
}