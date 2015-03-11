#ifndef MATHDEF_H
#define MATHDEF_H

#include <float.h>
#include <limits.h>
#include <stdlib.h>

namespace Math
{
    static const float PI = 3.1415926535f;
    static const float TAU = 6.283185307179586476925286766559f;
    static const float EPSILON = FLT_MIN;
    static const float FLOAT_MAX = FLT_MAX;
    static const int INTEGER_MIN = INT_MIN;
    static const int INTEGER_MAX = INT_MAX;;
    static const unsigned int UINTEGER_MAX = UINT_MAX;
    static const char CHARACTER_MAX = CHAR_MAX;
    static const char CHARACTER_MIN = CHAR_MIN;
    static const unsigned char UCHARACTER_MAX = UCHAR_MAX;

    static float RandomFloat()
    {
        return static_cast<float>(rand())/static_cast<float>(RAND_MAX);
    }

    template <typename T>
    static T Clamp(T value, T min, T max)
    {
        return value > max ? max : value < min ? min : value;
    }

    template <typename T>
    static T Min(T a, T b)
    {
        return (a < b) ? a : b;
    }

    template <typename T>
    static T Min(T a, T b, T c)
    {
        T minTemp = Min(a,b);
        return (minTemp < c) ? minTemp : c;
    }

    template <typename T>
    static T Max(T a, T b)
    {
        return (a > b) ? a : b;
    }

    template <typename T>
    static T Max(T a, T b, T c)
    {
        T maxTemp = Max(a,b);
        return (maxTemp > c) ? maxTemp : c;
    }
}

#endif