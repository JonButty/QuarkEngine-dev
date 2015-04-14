/*****************************************************************************/
/*!
\file		Vector4Native.cpp
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

#include "Simian/Vector4Native.h"
#include "Simian/Debug.h"

#include <cmath>

namespace Simian
{
    Vector4::Vector4(f32 x, f32 y, f32 z, f32 w)
    : x_(x),
      y_(y),
      z_(z),
      w_(w)
    {
    }

    Vector4::Vector4( const Vector3& v, f32 w )
    : x_(v.X()),
      y_(v.Y()),
      z_(v.Z()),
      w_(w)
    {
    }

    //--------------------------------------------------------------------------

    Vector4 Vector4::operator+(const Vector4& v) const
    {
        return Vector4(
            x_ + v.x_,
            y_ + v.y_,
            z_ + v.z_,
            w_ + v.w_);
    }

    Vector4 Vector4::operator-(const Vector4& v) const
    {
        return Vector4(
            x_ - v.x_,
            y_ - v.y_,
            z_ - v.z_,
            w_ - v.w_);
    }

    Vector4 Vector4::operator*(const Vector4& v) const
    {
        return Vector4(
            x_ * v.x_,
            y_ * v.y_,
            z_ * v.z_,
            w_ * v.w_);
    }

    Vector4 Vector4::operator/(const Vector4& v) const
    {
        return Vector4(
            x_/v.x_,
            y_/v.y_,
            z_/v.z_,
            w_/v.w_);
    }

    Vector4 Vector4::operator*(f32 c) const
    {
        return Vector4(
            x_ * c,
            y_ * c,
            z_ * c,
            w_ * c);
    }

    Vector4 Vector4::operator/(f32 c) const
    {
        return Vector4(
            x_/c,
            y_/c,
            z_/c,
            w_/c);
    }

    Vector4& Vector4::operator+=(const Vector4& v)
    {
        x_ += v.x_;
        y_ += v.y_;
        z_ += v.z_;
        w_ += v.w_;

        return *this;
    }

    Vector4& Vector4::operator-=(const Vector4& v)
    {
        x_ -= v.x_;
        y_ -= v.y_;
        z_ -= v.z_;
        w_ -= v.w_;

        return *this;
    }

    Vector4& Vector4::operator*=(const Vector4& v)
    {
        x_ *= v.x_;
        y_ *= v.y_;
        z_ *= v.z_;
        w_ *= v.w_;

        return *this;
    }

    Vector4& Vector4::operator/=(const Vector4& v)
    {
        x_ /= v.x_;
        y_ /= v.y_;
        z_ /= v.z_;
        w_ /= v.w_;

        return *this;
    }

    Vector4& Vector4::operator*=(f32 c)
    {
        x_ *= c;
        y_ *= c;
        z_ *= c;
        w_ *= c;

        return *this;
    }

    Vector4& Vector4::operator/=(f32 c)
    {
        x_ /= c;
        y_ /= c;
        z_ /= c;
        w_ /= c;

        return *this;
    }

    //--------------------------------------------------------------------------

    void Vector4::Normalize()
    {
        f32 length = Length();
        x_ /= length;
        y_ /= length;
        z_ /= length;
        w_ /= length;
    }

    f32 Vector4::LengthSquared() const
    {
        return x_ * x_ + y_ * y_ + z_ * z_ + w_ * w_;
    }

    f32 Vector4::Length() const
    {
        return std::sqrtf(LengthSquared());
    }

    f32 Vector4::Dot(const Vector4& vec) const
    {
        return x_ * vec.x_ + y_ * vec.y_ + z_ * vec.z_ + w_ * vec.w_;
    }

    Vector4 Vector4::Cross(const Vector4& vec) const
    {
        return Vector4(
            y_ * vec.z_ - z_ * vec.y_,
            z_ * vec.x_ - x_ * vec.z_,
            x_ * vec.y_ - y_ * vec.x_);
    }

    Vector4 Vector4::Slerp( const Vector4& initial, const Vector4& final, float intp )
    {
        f32 fCos = initial.Dot(final);
        Simian::Vector4 target;

        // Do we need to invert rotation?
        if (fCos < 0.0f)
        {
            fCos = -fCos;
            target = -final;
        }
        else
            target = final;

        if (std::abs(fCos) < 0.999f)
        {
            // Standard case (slerp)
            f32 fSin = std::sqrt(1 - fCos * fCos);
            f32 fAngle = std::atan2(fSin, fCos);
            f32 fInvSin = 1.0f / fSin;
            f32 fCoeff0 = std::sin((1.0f - intp) * fAngle) * fInvSin;
            f32 fCoeff1 = std::sin(intp * fAngle) * fInvSin;
            return fCoeff0 * initial + fCoeff1 * target;
        }
        else
            return ((1.0f - intp) * initial + intp * target).Normalized();
    }
}

Simian::Vector4 operator*(Simian::f32 c, const Simian::Vector4& vec)
{
    return Simian::Vector4(
        c * vec.X(),
        c * vec.Y(),
        c * vec.Z(),
        c * vec.W());
}

Simian::Vector4 operator/(Simian::f32 c, const Simian::Vector4& vec)
{
    return Simian::Vector4(
        c/vec.X(),
        c/vec.Y(),
        c/vec.Z(),
        c/vec.W());
}

#endif
