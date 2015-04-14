/*****************************************************************************/
/*!
\file		Vector3Native.cpp
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

#include "Simian/Vector3Native.h"
#include "Simian/Debug.h"

#include <cmath>

namespace Simian
{
    Vector3::Vector3(f32 x, f32 y, f32 z)
    : x_(x),
      y_(y),
      z_(z)
    {

    }

    //--------------------------------------------------------------------------

    Vector3 Vector3::operator+(const Vector3& v) const
    {
        return Vector3(
            x_ + v.x_,
            y_ + v.y_,
            z_ + v.z_);
    }

    Vector3 Vector3::operator-(const Vector3& v) const
    {
        return Vector3(
            x_ - v.x_,
            y_ - v.y_,
            z_ - v.z_);
    }

    Vector3 Vector3::operator*(const Vector3& v) const
    {
        return Vector3(
            x_ * v.x_,
            y_ * v.y_,
            z_ * v.z_);
    }

    Vector3 Vector3::operator/(const Vector3& v) const
    {
        return Vector3(
            x_/v.x_,
            y_/v.y_,
            z_/v.z_);
    }

    Vector3 Vector3::operator*(f32 c) const
    {
        return Vector3(
            x_ * c,
            y_ * c,
            z_ * c);
    }

    Vector3 Vector3::operator/(f32 c) const
    {
        return Vector3(
            x_/c,
            y_/c,
            z_/c);
    }

    Vector3& Vector3::operator+=(const Vector3& v)
    {
        x_ += v.x_;
        y_ += v.y_;
        z_ += v.z_;

        return *this;
    }

    Vector3& Vector3::operator-=(const Vector3& v)
    {
        x_ -= v.x_;
        y_ -= v.y_;
        z_ -= v.z_;

        return *this;
    }

    Vector3& Vector3::operator*=(const Vector3& v)
    {
        x_ *= v.x_;
        y_ *= v.y_;
        z_ *= v.z_;

        return *this;
    }

    Vector3& Vector3::operator/=(const Vector3& v)
    {
        x_ /= v.x_;
        y_ /= v.y_;
        z_ /= v.z_;

        return *this;
    }

    Vector3& Vector3::operator*=(f32 c)
    {
        x_ *= c;
        y_ *= c;
        z_ *= c;

        return *this;
    }

    Vector3& Vector3::operator/=(f32 c)
    {
        x_ /= c;
        y_ /= c;
        z_ /= c;

        return *this;
    }

    //--------------------------------------------------------------------------

    void Vector3::Normalize()
    {
        f32 length = Length();
        x_ /= length;
        y_ /= length;
        z_ /= length;
    }

    f32 Vector3::LengthSquared() const
    {
        return x_ * x_ + y_ * y_ + z_ * z_;
    }

    f32 Vector3::Length() const
    {
        return std::sqrtf(LengthSquared());
    }

    f32 Vector3::Dot(const Vector3& vec) const
    {
        return x_ * vec.x_ + y_ * vec.y_ + z_ * vec.z_;
    }

    Vector3 Vector3::Cross(const Vector3& vec) const
    {
        return Vector3(
            y_ * vec.z_ - z_ * vec.y_,
            z_ * vec.x_ - x_ * vec.z_,
            x_ * vec.y_ - y_ * vec.x_);
    }
}

Simian::Vector3 operator*(Simian::f32 c, const Simian::Vector3& vec)
{
    return Simian::Vector3(
        c * vec.X(),
        c * vec.Y(),
        c * vec.Z());
}

Simian::Vector3 operator/(Simian::f32 c, const Simian::Vector3& vec)
{
    return Simian::Vector3(
        c/vec.X(),
        c/vec.Y(),
        c/vec.Z());
}

#endif
