#include "Arduino.h"
#include "my_servo.h"

float height = 32;

s16 max(s16 a, s16 b)
{
    return (a > b) ? a : b;
}

s16 min(s16 a, s16 b)
{
    return (a < b) ? a : b;
}

void setup()
{
    Serial.begin(115200);
    my_sms_init();
}

void loop()
{
    for (int i = 32; i < 85; i++)
    {
        height = i;
        my_sms_set(8, 8, 200, 200,
                   min(max(2048 + 12 + 8.4 * (height - 32), 2110), 2510),
                   min(max(2048 - 12 - 8.4 * (height - 32), 1586), 1986));
        my_sms_do();
        Serial.println(height);
        delay(200);
    }
    for (int j = 32; j < 85; j++)
    {
        height = 32 + 85 - j;
        my_sms_set(8, 8, 200, 200,
                   min(max(2048 + 12 + 8.4 * (height - 32), 2110), 2510),
                   min(max(2048 - 12 - 8.4 * (height - 32), 1586), 1986));
        my_sms_do();
        Serial.println(height);
        delay(200);
    }
}
