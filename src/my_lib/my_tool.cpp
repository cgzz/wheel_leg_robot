#include <math.h>
#include <Arduino.h>

float my_db(float value, float deadband)
{
    float new_value = value;
    if (fabsf(value) < deadband)
        float new_value = 0.0f;
    return new_value;
}

float my_lim(float value, float min, float max)
{
    float new_value = value;
    if (value < min)
        new_value = min;
    else if (value > max)
        new_value = max;
    return new_value;
}
float my_lim(float value, float lim)
{
    float new_value = value;
    if (value < -lim)
        new_value = -lim;
    else if (value > lim)
        new_value = lim;
    return new_value;
}

// ====== 字符串工具函数 ======

static inline void rtrim_inplace(char *s)
{
    char *e = s + strlen(s);
    while (e > s && (e[-1] == ' ' || e[-1] == '\t' || e[-1] == '\r' || e[-1] == '\n'))
        --e;
    *e = '\0';
}