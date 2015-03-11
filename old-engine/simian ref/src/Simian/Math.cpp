/*****************************************************************************/
/*!
\file		Math.cpp
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	March 14, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*****************************************************************************/

#include "Simian/Math.h"
#include <cstdlib>
#include <time.h>

#include <cmath>

namespace Simian
{
    const f32 Math::PI = 3.1415926535f;
    const f32 Math::TAU = 6.283185307179586476925286766559f;
    const f32 Math::EPSILON = 0.0001f;
    
    f32 Math::FastInvSqrt(f32 val)
    {
        f32 half = val * 0.5f;
        s32 cast = *reinterpret_cast<s32*>(&val);
        cast = 0x5f3759df - (cast >> 1);
        val = *(f32*)&cast;
        return val * (1.5f - half * val * val);
    }

    f32 Math::FastSqrt(f32 val)
    {
        return 1.0f / FastInvSqrt(val);
    }

    Vector2 Math::NearestPointOnLine(const Vector2& point, const Vector2 &lineBegin, const Vector2 &lineEnd)
    {
        // Seperation Axis Theorem: project all 3 points onto
        //   l(x): origin + lineDir*t
        Simian::Vector2 lineDir = lineEnd - lineBegin;
        lineDir.Normalize();    // TODO: Test if need to normalize
        f32 beginT = lineBegin.Dot(lineDir);
        f32 endT   = lineEnd.Dot(lineDir);
        f32 pointT = point.Dot(lineDir);

        // Point is outside Segment, end side
        if (pointT > endT)
            return lineEnd;

        // Point is outside Segment, begin side
        else if (pointT < beginT)
            return lineBegin;

        // Point is within Segment (between beginT and endT)

        // Find Normal of line
        Simian::Vector2 normal = lineBegin - lineEnd;
        f32 tmp = normal.X();
        normal.X(normal.Y());
        normal.Y(-tmp);
        normal.Normalize();

        // Find nearest point
        Simian::Vector2 toPoint = point - lineBegin;
        return point - normal * normal.Dot(toPoint);
    }

    s32 Math::Random( s32 min, s32 max )
    {
        return min + rand()%(max - min);
    }

    f32 Math::Random(f32 min, f32 max)
    {
        f32 r = static_cast<f32>(rand())/static_cast<f32>(RAND_MAX);
        return min + (max - min) * r;
    }

    Simian::Radian Math::WrapAngle(const Simian::Angle& a )
    {
        f32 angle = a.Radians();
        while (angle > Simian::Math::TAU)
            angle -= Simian::Math::TAU;
        while (angle < 0.0f)
            angle += Simian::Math::TAU;
        return Simian::Radian(angle);
    }

    Simian::Radian Math::NearestAngleDifference(const Simian::Angle& c, const Simian::Angle& t )
    {
        f32 targetAngle = t.Radians();
        f32 currentAngle = c.Radians();
        f32 diff = targetAngle - currentAngle;
        if (std::abs(diff) > Simian::Math::PI)
        {
            currentAngle += ((targetAngle > currentAngle) ? Simian::Math::TAU : -Simian::Math::TAU);
            diff = targetAngle - currentAngle;
        }
        return Simian::Radian(diff);
    }
}
