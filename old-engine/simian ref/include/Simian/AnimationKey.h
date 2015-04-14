/************************************************************************/
/*!
\file		AnimationKey.h
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	August 10, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/************************************************************************/
#ifndef SIMIAN_ANIMATIONKEY_H_
#define SIMIAN_ANIMATIONKEY_H_

#include "SimianPlatform.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Matrix.h"

namespace Simian
{
    class SIMIAN_EXPORT AnimationKey
    {
    private:
        float time_;

        Simian::Vector3 position_;
        Simian::Vector4 rotation_;
        Simian::Vector3 scale_;
    public:
        bool operator< (const AnimationKey& other) const;
    public:
        AnimationKey(float time, const Simian::Matrix& transform);

        f32 Time() const;

        const Simian::Vector3& Position() const;
        const Simian::Vector4& Rotation() const;
        const Simian::Vector3& Scale() const;
    };
}

#endif
