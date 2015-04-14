/************************************************************************/
/*!
\file		Joint.cpp
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	August 10, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/************************************************************************/
#include "Simian/Joint.h"
#include "Simian/Skeleton.h"

namespace Simian
{
    Joint::Joint( std::string name, u32 id, const Simian::Matrix& bindPose )
        : parentSkeleton_(0),
          name_(name),
          bindPose_(bindPose),
          id_(id),
          parent_(0)
    {
    }

    //--------------------------------------------------------------------------

    const std::string& Joint::Name() const
    {
        return name_;
    }

    Simian::u32 Joint::Id() const
    {
        return id_;
    }

    const Simian::Matrix& Joint::BindPose() const
    {
        return bindPose_;
    }

    const Simian::Matrix& Joint::InvBindPose() const
    {
        return invBindPose_;
    }

    const std::vector<Joint*>& Joint::Children() const
    {
        return children_;
    }

    const std::vector<AnimationKey>& Joint::KeyFrames() const
    {
        return keyFrames_;
    }

    Skeleton* Joint::ParentSkeleton() const
    {
        return parentSkeleton_;
    }

    //--------------------------------------------------------------------------

    void Joint::AddChild( Joint* joint )
    {
        joint->parent_ = this;
        children_.push_back(joint);
    }
}
