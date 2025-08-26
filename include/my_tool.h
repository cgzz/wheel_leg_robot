#pragma once

float my_db(float value, float deadband);

float my_lim(float value, float min, float max);

float my_lim(float value, float lim);

static inline void rtrim_inplace(char *s);