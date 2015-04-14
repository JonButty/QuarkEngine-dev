/************************************************************************/
/*!
\file		AnimationController.h
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	August 10, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/************************************************************************/
#ifndef SIMIAN_ANIMATIONCONTROLLER_H_
#define SIMIAN_ANIMATIONCONTROLLER_H_

#include "SimianPlatform.h"
#include "Matrix.h"

#include <vector>

namespace Simian
{
    class Joint;
    class AnimationController;
    class AnimationKey;
    class Model;
    struct AnimationFrames;

    class SIMIAN_EXPORT AnimationJoint
    {
    private:
        AnimationController* parentController_;
        AnimationJoint* parent_;
        Joint* joint_;
        Simian::Matrix world_;
        Simian::Matrix local_;
        std::vector<AnimationJoint*> children_;
    public:
        Simian::Joint* Joint() const;
        const Simian::Matrix& World() const;
    private:
        void updateAnimation_();
    public:
        AnimationJoint(AnimationController* parentController, AnimationJoint* parent, Simian::Joint* joint);

        void Update();

        friend class AnimationController;
    };

    class SIMIAN_EXPORT AnimationController
    {
    private:
        Model* parent_;
        AnimationJoint* root_;
        std::vector<Matrix> boneMatrices_;
        u32 frame_;
        f32 timeElapsed_;
        f32 timePerFrame_;
        bool loop_;
        bool playing_;
        const AnimationFrames* currentAnimation_;
    public:
        Model* Parent() const;
        const std::vector<Matrix>& BoneMatrices() const;
    private:
        AnimationJoint* buildAnimationJoint_(Joint* joint, AnimationJoint* parent);
        void destroyAnimationJoint_(AnimationJoint*& joint);
        AnimationJoint* animationJointByName_(const std::string& name, AnimationJoint* joint) const;
    public:
        AnimationController(Model* parent, Joint* root);
        ~AnimationController();

        void PlayAnimation(const std::string& animationName, bool loop = true, s32 fps = -1);
        void StopAnimation();

        AnimationJoint* JointByName(const std::string& name) const;

        void Update(f32 dt);

        friend class AnimationJoint;
    };
}

#endif
