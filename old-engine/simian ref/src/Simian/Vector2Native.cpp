/*****************************************************************************/
/*!
\file		Vector2Native.cpp
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	March 14, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*****************************************************************************/

#include "Simian/SimianPlatform.h"
#if SIMIAN_SSE_SUPPORT == 0

#include "Simian/Vector2Native.h"
#include "Simian/Math.h"
#include "Simian/Debug.h"

#include <cmath>

namespace Simian
{
    Vector2::Vector2(f32 x, f32 y)
    : x_(x),
      y_(y)
    {
    }

    //--------------------------------------------------------------------------

    Vector2 Vector2::operator+(const Vector2& vec) const
    {
        return Vector2(x_ + vec.x_, y_ + vec.y_);
    }

    Vector2& Vector2::operator+=(const Vector2& vec)
    {
        x_ += vec.x_;
        y_ += vec.y_;
        return *this;
    }

    Vector2 Vector2::operator-(const Vector2& vec) const
    {
        return Vector2(x_ - vec.x_, y_ - vec.y_);
    }

    Vector2& Vector2::operator-=(const Vector2& vec)
    {
        x_ -= vec.x_;
        y_ -= vec.y_;
        return *this;
    }

    Vector2 Vector2::operator*(const Vector2& vec) const
    {
        return Vector2(x_ * vec.x_, y_ * vec.y_);
    }

    Vector2& Vector2::operator*=(const Vector2& vec)
    {
        x_ *= vec.x_;
        y_ *= vec.y_;
        return *this;
    }

    Vector2 Vector2::operator*(f32 val) const
    {
        return Vector2(x_ * val, y_ * val);
    }

    Vector2& Vector2::operator*=(f32 val)
    {
        x_ *= val;
        y_ *= val;
        return *this;
    }

    Vector2 Vector2::operator/(const Vector2& vec) const
    {
        return Vector2(x_/vec.x_, y_/vec.y_);
    }

    Vector2& Vector2::operator/=(const Vector2& vec)
    {
        x_ /= vec.x_;
        y_ /= vec.y_;
        return *this;
    }

    Vector2 Vector2::operator/(f32 val) const
    {
        return Vector2(x_/val, y_/val);
    }

    Vector2& Vector2::operator/=(f32 val)
    {
        x_ /= val;
        y_ /= val;
        return *this;
    }

    //--------------------------------------------------------------------------

    f32 Vector2::LengthSquared() const
    {
        return x_ * x_ + y_ * y_;
    }

    f32 Vector2::Length() const
    {
        return Math::FastSqrt(LengthSquared());
    }

    void Vector2::Normalize()
    {
        f32 length = Length();
        x_ /= length;
        y_ /= length;
    }

    Vector2 Vector2::Normalized() const
    {
        f32 length = Length();
        return Vector2(x_/length, y_/length);
    }

    f32 Vector2::Dot(const Vector2& vec) const
    {
        return x_ * vec.x_ + y_ * vec.y_;
    }
}

Simian::Vector2 operator*(Simian::f32 val, const Simian::Vector2& vec)
{
    return Simian::Vector2(val * vec.X(), val * vec.Y());
}

Simian::Vector2 operator/(Simian::f32 val, const Simian::Vector2& vec)
{
    return Simian::Vector2(val/vec.X(), val/vec.Y());
}

#endif
