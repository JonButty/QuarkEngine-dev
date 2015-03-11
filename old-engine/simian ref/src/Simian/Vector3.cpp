/*****************************************************************************/
/*!
\file		Vector3.cpp
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	March 14, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*****************************************************************************/

#include "Simian/Debug.h"
#include "Simian/Vector3.h"

namespace Simian
{
    const Vector3 Vector3::UnitX(1, 0, 0);
    const Vector3 Vector3::UnitY(0, 1, 0);
    const Vector3 Vector3::UnitZ(0, 0, 1);
    const Vector3 Vector3::Zero(0, 0, 0);

    //--------------------------------------------------------------------------

    void Vector3::X(f32 x)
    {
        x_ = x;
    }

    f32 Vector3::X() const
    {
        return x_;
    }

    void Vector3::Y(f32 y)
    {
        y_ = y;
    }

    f32 Vector3::Y() const
    {
        return y_;
    }

    void Vector3::Z(f32 z)
    {
        z_ = z;
    }

    f32 Vector3::Z() const
    {
        return z_;
    }

    f32& Vector3::operator[](s32 index)
    {
        return v[index];
    }

    const f32& Vector3::operator[](s32 index) const
    {
        return v[index];
    }
    
    Vector3 Vector3::operator-() const
    {
        return Vector3(-x_, -y_, -z_);
    }

    Vector3 Vector3::Normalized() const
    {
        Vector3 vec = *this;
        vec.Normalize();
        return vec;
    }
}
