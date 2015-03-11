/************************************************************************/
/*!
\file		AnimationKey.cpp
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	August 10, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/************************************************************************/
#include "Simian/AnimationKey.h"

#include <d3dx9.h>

namespace Simian
{
    AnimationKey::AnimationKey( float time, const Simian::Matrix& transform )
        : time_(time)
    {
        // decompose transform
        transform.Decompose(position_, rotation_, scale_);
    }

    //--------------------------------------------------------------------------

    f32 AnimationKey::Time() const
    {
        return time_;
    }

    const Simian::Vector3& AnimationKey::Position() const
    {
        return position_;
    }

    const Simian::Vector4& AnimationKey::Rotation() const
    {
        return rotation_;
    }

    const Simian::Vector3& AnimationKey::Scale() const
    {
        return scale_;
    }

    //--------------------------------------------------------------------------

    bool AnimationKey::operator<( const AnimationKey& other ) const
    {
        return time_ < other.time_;
    }
}
