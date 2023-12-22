#pragma once

#define _USE_MATH_DEFINES
#include <math.h> /* c-header as msvc seems to have an issue with cmath */
#include <cassert>
#include <cstdint>

inline constexpr float PI = 3.1415926535897932384626433832795;
inline constexpr float PI_two = 6.2831853071795864769252867665590;
inline constexpr float PI_half = 1.5707963267948966192313216916398;

#define degrees(x) (x * 180.0 / PI)
#define radians(x) (x * PI / 180.0)
