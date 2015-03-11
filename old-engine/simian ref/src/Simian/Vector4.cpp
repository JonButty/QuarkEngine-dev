/*****************************************************************************/
/*!
\file		Vector4.cpp
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	March 14, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*****************************************************************************/

#include "Simian/Debug.h"
#include "Simian/Vector4.h"

namespace Simian
{
    const Vector4 Vector4::UnitX(1, 0, 0, 0);
    const Vector4 Vector4::UnitY(0, 1, 0, 0);
    const Vector4 Vector4::UnitZ(0, 0, 1, 0);
    const Vector4 Vector4::UnitW(0, 0, 0, 1);
    const Vector4 Vector4::Zero(0, 0, 0, 0);

    //--------------------------------------------------------------------------

    void Vector4::X(f32 x)
    {
        x_ = x;
    }

    f32 Vector4::X() const
    {
        return x_;
    }

    void Vector4::Y(f32 y)
    {
        y_ = y;
    }

    f32 Vector4::Y() const
    {
        return y_;
    }

    void Vector4::Z(f32 z)
    {
        z_ = z;
    }

    f32 Vector4::Z() const
    {
        return z_;
    }

    void Vector4::W(f32 w)
    {
        w_ = w;
    }

    f32 Vector4::W() const
    {
        return w_;
    }
    
    Vector4 Vector4::operator-() const
    {
        return Vector4(-x_, -y_, -z_, -w_);
    }

    f32& Vector4::operator[](s32 index)
    {
        return v[index];
    }

    const f32& Vector4::operator[](s32 index) const
    {
        return v[index];
    }

    Vector4 Vector4::Normalized() const
    {
        Vector4 vec = *this;
        vec.Normalize();
        return vec;
    }
}
