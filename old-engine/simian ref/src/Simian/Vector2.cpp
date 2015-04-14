/*****************************************************************************/
/*!
\file		Vector2.cpp
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	March 14, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*****************************************************************************/

#include "Simian/Vector2.h"
#include "Simian/Debug.h"

namespace Simian
{
    const Vector2 Vector2::UnitX(1, 0);
    const Vector2 Vector2::UnitY(0, 1);
    const Vector2 Vector2::Zero(0, 0);

    //--------------------------------------------------------------------------

    f32 Vector2::X() const
    {
        return x_;
    }

    void Vector2::X(f32 x)
    {
        x_ = x;
    }

    f32 Vector2::Y() const
    {
        return y_;
    }

    void Vector2::Y(f32 y)
    {
        y_ = y;
    }

    Vector2 Vector2::operator-() const
    {
        return Vector2(-x_, -y_);
    }

    const f32& Vector2::operator[](s32 index) const
    {
        SAssert(index >= 0 && index < 2, "Vector subscript out of range.");
        return v_[index];
    }

    f32& Vector2::operator[](s32 index)
    {
        SAssert(index >= 0 && index < 2, "Vector subscript out of range.");
        return v_[index];
    }
}
