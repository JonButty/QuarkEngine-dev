/************************************************************************/
/*!
\file		AnimationController.cpp
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	August 10, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/************************************************************************/
#include "Simian/AnimationController.h"
#include "Simian/Joint.h"
#include "Simian/Model.h"
#include "Simian/Math.h"
#include "Simian/Interpolation.h"
#include "Simian/Vector3.h"
#include "Simian/Vector4.h"

namespace Simian
{
    AnimationController::AnimationController( Model* parent, Joint* root )
        : parent_(parent), 
          root_(0),
          frame_(0),
          timeElapsed_(0),
          timePerFrame_(1.0f/24.0f),
          loop_(true),
          playing_(false),
          currentAnimation_(0)
    {
        root_ = buildAnimationJoint_(root, NULL);
        boneMatrices_.resize(parent->Skeleton()->JointCount());
    }
    
    AnimationController::~AnimationController()
    {
        destroyAnimationJoint_(root_);
    }

    //--------------------------------------------------------------------------

    Model* AnimationController::Parent() const
    {
        return parent_;
    }

    const std::vector<Matrix>& AnimationController::BoneMatrices() const
    {
        return boneMatrices_;
    }

    //--------------------------------------------------------------------------

    AnimationJoint* AnimationController::buildAnimationJoint_( Joint* joint, AnimationJoint* parent )
    {
        AnimationJoint* ajoint = new AnimationJoint(this, parent, joint);

        const std::vector<Joint*>& children = joint->Children();
        for (std::vector<Joint*>::const_iterator i = children.begin(); i != children.end(); ++i)
        {
            AnimationJoint* child = buildAnimationJoint_(*i, ajoint);
            ajoint->children_.push_back(child);
        }

        return ajoint;
    }

    void AnimationController::destroyAnimationJoint_( AnimationJoint*& joint )
    {
        for (u32 i = 0; i < joint->children_.size(); ++i)
            destroyAnimationJoint_(joint->children_[i]);
        joint->children_.clear();
        delete joint;
        joint = 0;
    }

    AnimationJoint* AnimationController::animationJointByName_( const std::string& name, AnimationJoint* joint ) const
    {
        // check if the joint is the joint we're looking for
        if (joint->joint_->Name() == name)
            return joint;

        // go through all children and see if we can find one
        for (Simian::u32 i = 0; i < joint->children_.size(); ++i)
        {
            AnimationJoint* ret = animationJointByName_(name, joint->children_[i]);
            if (ret)
                return ret;
        }

        // nothing found..
        return 0;
    }

    //--------------------------------------------------------------------------

    void AnimationController::PlayAnimation( const std::string& animationName, bool loop, s32 fps )
    {
        currentAnimation_ = parent_->AnimationFrame(animationName);
        if (currentAnimation_)
        {
            loop_ = loop;
            timePerFrame_ = 1.0f/static_cast<f32>(fps >= 0 ? fps : currentAnimation_->Fps);
            timeElapsed_ = 0.0f;
            frame_ = currentAnimation_->Start;
            playing_ = true;
        }
    }

    void AnimationController::StopAnimation()
    {
        currentAnimation_ = 0;
        playing_ = false;
    }

    AnimationJoint* AnimationController::JointByName(const std::string& name) const
    {
        return animationJointByName_(name, root_);
    }
    
    void AnimationController::Update( f32 dt )
    {
        // calculate frames
        if (playing_ && currentAnimation_)
        {
            timeElapsed_ += dt;

            while (timeElapsed_ > timePerFrame_)
            {
                timeElapsed_ -= timePerFrame_;
                ++frame_;
                if (frame_ >= currentAnimation_->End)
                {
                    if (loop_)
                        frame_ = currentAnimation_->Start;
                    else
                    {
                        frame_ = currentAnimation_->End - 1;
                        timeElapsed_ = timePerFrame_;
                        playing_ = false;
                        break;
                    }
                }
            }
        }

        // update animation joints
        root_->Update();
    }


    //--------------------------------------------------------------------------

    AnimationJoint::AnimationJoint( AnimationController* parentController, AnimationJoint* parent, Simian::Joint* joint )
        : parentController_(parentController),
          parent_(parent),
          joint_(joint)
    {
    }

    //--------------------------------------------------------------------------

    Simian::Joint* AnimationJoint::Joint() const
    {
        return joint_;
    }

    const Simian::Matrix& AnimationJoint::World() const
    {
        return world_;
    }

    //--------------------------------------------------------------------------

    void AnimationJoint::updateAnimation_()
    { 
        if (joint_->KeyFrames().size() < 2)
            return;

        // find the new world matrix (from animation keys)
        const AnimationKey& current = joint_->KeyFrames()[parentController_->frame_];
        const AnimationKey& next = joint_->KeyFrames()[parentController_->frame_ + 1];

        // get interpolation value
        f32 intp = parentController_->timeElapsed_/parentController_->timePerFrame_;
        intp = Math::Clamp(intp, 0.0f, 1.0f);

        // interpolate position, scale and rotation
        Vector3 position = Interpolation::Interpolate(current.Position(), next.Position(), intp, Interpolation::Linear);
        Vector3 scale = Interpolation::Interpolate(current.Scale(), next.Scale(), intp, Interpolation::Linear);
        Simian::Vector4 rotation(
            Simian::Vector4::Slerp(current.Rotation(), next.Rotation(), intp));

        // yay compose the local matrix (SRT)
        local_ = Simian::Matrix::Translation(position) *
                 Simian::Matrix::RotationQuaternion(rotation) *
                 Simian::Matrix::Scale(scale);
    }

    //--------------------------------------------------------------------------

    void AnimationJoint::Update()
    {
        updateAnimation_();

        // update world
        world_ = (parent_ ? parent_->world_ : 
            joint_->BindPose() * Simian::Matrix::Scale(1, 1, -1)) * local_;

        // find the bone matrix
        parentController_->boneMatrices_[joint_->Id()] = 
            joint_->ParentSkeleton()->RootTransformInversed() * 
            world_ * joint_->InvBindPose();

        // update children
        for (u32 i = 0; i < children_.size(); ++i)
            children_[i]->Update();
    }
}
