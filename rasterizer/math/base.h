#pragma once

#define _USE_MATH_DEFINES
#include <math.h> /* c-header as msvc seems to have an issue with cmath */
#include <cassert>

#define degrees(x) (x * 180.0 / M_PI)
#define radians(x) (x * M_PI / 180.0)
