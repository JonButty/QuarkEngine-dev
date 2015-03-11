/************************************************************************/
/*!
\file		Skeleton.cpp
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	August 10, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/************************************************************************/
#include "Simian/Skeleton.h"
#include "Simian/Joint.h"

namespace Simian
{
    template <class T>
    static T extractBinary_(s8*& memPtr, u32 num = 1)
    {
        T ret = *((T*)memPtr);
        ((T*&)memPtr) += num;
        return ret;
    }

    template <class T, class U>
    static void extractChunk_(T* dest, U& src, u32 size)
    {
        memcpy(dest, src, sizeof(T) * size);
        src += sizeof(T) * size;
    }

    //--------------------------------------------------------------------------

    Skeleton::Skeleton( Joint* root, const std::vector<Joint*>& joints )
        : root_(root),
          joints_(joints)
    {
    }

    Skeleton::Skeleton(s8* memPtr)
        : root_(0)
    {
        u32 size = extractBinary_<u32>(memPtr);
        joints_.resize(size);
        root_ = loadJointData_(memPtr, 0);
        rootTransformInversed_ = root_->BindPose().Inversed();
    }

    //--------------------------------------------------------------------------

    Joint* Skeleton::Root() const
    {
        return root_;
    }

    Simian::u32 Skeleton::JointCount() const
    {
        return joints_.size();
    }

    const Matrix& Skeleton::RootTransformInversed() const
    {
        return rootTransformInversed_;
    }

    //--------------------------------------------------------------------------

    Joint* Skeleton::loadJointData_( s8*& memPtr, Joint* parent )
    {
        u32 length = extractBinary_<u32>(memPtr);
        std::string name = std::string(memPtr, memPtr + length);
        memPtr += length;
        Matrix bindPose = extractBinary_<Matrix>(memPtr);
        Matrix invBindPose = extractBinary_<Matrix>(memPtr);
        u32 id = extractBinary_<u32>(memPtr);
        Joint* joint = new Joint(name, id, bindPose);
        joints_[id] = joint;
        joint->parentSkeleton_ = this;
        joint->parent_ = parent;
        joint->invBindPose_ = invBindPose;
        length = extractBinary_<u32>(memPtr);
        if (length)
        {
            joint->keyFrames_.resize(length, AnimationKey(0, Matrix::Identity));
            extractChunk_(&joint->keyFrames_[0], memPtr, length);
        }
        length = extractBinary_<u32>(memPtr);
        for (u32 i = 0; i < length; ++i)
        {
            Joint* child = loadJointData_(memPtr, joint);
            joint->children_.push_back(child);
        }
        return joint;
    }

    void Skeleton::cacheJointData_( std::ofstream& file, Joint* joint )
    {
        // name
        u32 length = joint->name_.length();
        file.write((s8*)&length, sizeof(u32));
        file.write(joint->name_.c_str(), sizeof(s8) * length);
        file.write((s8*)&joint->BindPose(), sizeof(Matrix));
        file.write((s8*)&joint->InvBindPose(), sizeof(Matrix));
        u32 id = joint->Id();
        file.write((s8*)&id, sizeof(u32));
        length = joint->KeyFrames().size();
        file.write((s8*)&length, sizeof(u32));
        file.write((s8*)&joint->KeyFrames()[0], sizeof(AnimationKey) * length);
        length = joint->Children().size();
        file.write((s8*)&length, sizeof(u32));
        for (u32 i = 0; i < length; ++i)
            cacheJointData_(file, joint->Children()[i]);
    }

    //--------------------------------------------------------------------------

    bool Skeleton::Load()
    {
        rootTransformInversed_ = root_->BindPose().Inversed();
        for (u32 i = 0; i < joints_.size(); ++i)
            joints_[i]->parentSkeleton_ = this;
        return true;
    }

    void Skeleton::Unload()
    {
        for (u32 i = 0; i < joints_.size(); ++i)
            delete joints_[i];
        joints_.clear();
        root_ = 0;
    }

    void Skeleton::CacheBinaryData( std::ofstream& file )
    {
        u32 size = joints_.size();
        file.write((s8*)&size, sizeof(u32));
        cacheJointData_(file, root_);
    }

    Joint* Skeleton::FindJointByName( const std::string& name )
    {
        for (u32 i = 0; i < joints_.size(); ++i)
        {
            if (joints_[i]->Name() == name)
                return joints_[i];
        }
        return NULL;
    }
}
