/*****************************************************************************/
/*!
\file		Math.h
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	March 14, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*****************************************************************************/

#ifndef SIMIAN_MATH_H_
#define SIMIAN_MATH_H_

#include "SimianPlatform.h"
#include "Vector2.h"
#include "Angle.h"

#undef PI

namespace Simian
{
    /** General math utility class. Contains helpful game optimized functions.*/
    class SIMIAN_EXPORT Math
    {
    public:
        /** The value of PI (fairly accurate approximation). */
        static const f32 PI;

        /** The value of TAU = 2 * PI (fairly accurate approximation). */
        static const f32 TAU;
        
        /** The value of EPSILON = smallest non-zero floating number */
        static const f32 EPSILON;
    public:
        /** Newton-Rhapson implementation of floating point inverse square root.
            @remarks
                This is a fairly accurate square root approximation. It 
                calculates the inverse square root of a value. Use 
                Math::FastSqrt to calculate the square root of a value.
            @see
                Math::FastSqrt
        */
        static f32 FastInvSqrt(f32 val);
        /** Newton-Rhapson implementation of floating point square root.
            @remarks
                This is a fairly accurate square root approximation. It 
                calculates the square root of a value. Use Math::FastInvSqrt 
                to calculate the inverse square root of a value.
            @see
                Math::FastInvSqrt
        */
        static f32 FastSqrt(f32 val);

        template <typename T>
        static T Clamp(T value, T min, T max)
        {
            return value > max ? max : value < min ? min : value;
        }

        template <typename T>
        static T Max( T a, T b )
        {
            return a > b ? a : b;
        }

        template <typename T>
        static T Min( T a, T b )
        {
            return a < b ? a : b;
        }

        static Vector2 NearestPointOnLine(const Vector2 &point, const Vector2 &lineBegin, const Vector2 &lineEnd);

        static s32 Random(s32 min, s32 max);

        static f32 Random(f32 min, f32 max);

        template <class T>
        static T Random(const T& min, const T& max)
        {
            return min + Random(0.0f, 1.0f) * (max - min);
        }

        template <class T>
        static T RandomPerElement(const T& min, const T& max, u32 elements)
        {
            T ret;
            for (u32 i = 0; i < elements; ++i)
                ret[i] = min[i] + Random(0.0f, 1.0f) * (max[i] - min[i]);
            return ret;
        }

        /** Wraps an angle between 0 - 360 degrees (or 0 - TAU radians). */
        static Simian::Radian WrapAngle(const Simian::Angle& a);

        /** Find the nearest offset for an angle. */
        static Simian::Radian NearestAngleDifference(const Simian::Angle& c, const Simian::Angle& t);
    };
}

#endif
