/************************************************************************/
/*!
\file		Joint.h
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	August 10, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/************************************************************************/
#ifndef SIMIAN_JOINT_H_
#define SIMIAN_JOINT_H_

#include "SimianPlatform.h"
#include "Matrix.h"
#include "Joint.h"
#include "AnimationKey.h"

#include <string>
#include <vector>

namespace Simian
{
    class Skeleton;
    class AnimationKey;

    class SIMIAN_EXPORT Joint
    {
    private:
        Skeleton* parentSkeleton_;
        std::string name_;
        Simian::Matrix bindPose_;
        Simian::Matrix invBindPose_;
        u32 id_;
        Joint* parent_;
        std::vector<Joint*> children_;
        std::vector<AnimationKey> keyFrames_;
    public:
        const std::string& Name() const;
        Simian::u32 Id() const;
        const Simian::Matrix& BindPose() const;
        const Simian::Matrix& InvBindPose() const;
        const std::vector<Joint*>& Children() const;
        const std::vector<AnimationKey>& KeyFrames() const;
        Skeleton* ParentSkeleton() const;
    public:
        Joint(std::string name, u32 id, const Simian::Matrix& bindPose);

        void AddChild(Joint* joint);

        friend class Skeleton;
        friend class Model;
    };
}

#endif
