#pragma once

float my_math_deadband(float value, float deadband);

float my_math_limit(float value, float min, float max);

static inline void rtrim_inplace(char *s);