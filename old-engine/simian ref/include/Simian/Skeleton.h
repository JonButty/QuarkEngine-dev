/************************************************************************/
/*!
\file		Skeleton.h
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	August 10, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/************************************************************************/
#ifndef SIMIAN_SKELETON_H_
#define SIMIAN_SKELETON_H_

#include "SimianPlatform.h"
#include "Matrix.h"

#include <string>
#include <vector>
#include <fstream>

namespace Simian
{
    class Joint;

    class SIMIAN_EXPORT Skeleton
    {
    private:
        Joint* root_;
        std::vector<Joint*> joints_;
        Matrix rootTransformInversed_;
    public:
        Joint* Root() const;
        u32 JointCount() const;
        const Matrix& RootTransformInversed() const;
    private:
        Joint* loadJointData_(s8*& memPtr, Joint* parent);
        void cacheJointData_(std::ofstream& file, Joint* joint);
    public:
        Skeleton(Joint* root, const std::vector<Joint*>& joints);
        Skeleton(s8* memPtr);

        bool Load();
        void Unload();
        void CacheBinaryData(std::ofstream& file);

        Joint* FindJointByName(const std::string& name);
    };
}

#endif
