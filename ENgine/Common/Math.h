
#pragma once

#define RADIAN  0.01745329f
#define PI      3.14159265f
#define TWO_PI  6.28318530f
#define HALF_PI 1.57079632f

#define rnd() ((float)rand()/RAND_MAX)
#define rnd_range(a, b) (a + (b - a) * (float)rand()/RAND_MAX)
#define MathMax(a,b)  (((a) > (b)) ? (a) : (b))
#define MathMin(a,b)  (((a) < (b)) ? (a) : (b))
#define MathClip(value,max, min)        \
    if (value > max)value = max;        \
    else                                \
       if (value < min)value  = min;
#define ROUND(value)  \
    if(value - static_cast<int>(value) > 0.5f)value = static_cast<int>(value) + 1;\
    else value = static_cast<int>(value);
#include "Math/Vector.h"
#include "Math/Vector2.h"
#include "Math/Vector4.h"
#include "Math/Quaternion.h"
#include "Math/Matrix.h"
