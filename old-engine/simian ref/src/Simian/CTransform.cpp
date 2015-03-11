/************************************************************************/
/*!
\file		CTransform.cpp
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	August 10, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/************************************************************************/
#include "Simian/CTransform.h"
#include "Simian/EnginePhases.h"
#include "Simian/DataFileIO.h"
#include "Simian/EngineComponents.h"
#include "Simian/LogManager.h"

namespace Simian
{
    FactoryPlant<EntityComponent, CTransform> CTransform::factoryPlant_(&GameFactory::Instance().ComponentFactory(), C_TRANSFORM);

    CTransform::CTransform()
        : parentTransform_(0),
          scale_(1.0f, 1.0f, 1.0f),
          axis_(Vector3::UnitY),
          computed_(false),
          dirty_(true)
    {
    }

    //--------------------------------------------------------------------------

    CTransform* CTransform::ParentTransform() const
    {
        return parentTransform_;
    }

    const Simian::Vector3& CTransform::Position() const
    {
        return position_;
    }

    void CTransform::Position( const Simian::Vector3& val )
    {
        position_ = val;
        dirty_ = true;
    }

    Simian::Vector3 CTransform::WorldPosition() const
    {
        return world_.Position();
    }

    void CTransform::WorldPosition( const Simian::Vector3& position )
    {
        if (parentTransform_) // transform into parent's space
            position_ = parentTransform_->World().Inversed() * position;
        else // no parent, position is global already
            position_ = position;
        dirty_ = true;
    }

    const Simian::Vector3 CTransform::Scale() const
    {
        return scale_;
    }

    void CTransform::Scale( const Simian::Vector3& val )
    {
        scale_ = val;
        dirty_ = true;
    }

    void CTransform::Rotation( const Simian::Vector3& axis, const Simian::Angle& angle )
    {
        axis_ = axis;
        if (axis_.LengthSquared() < 0.999f || axis_.LengthSquared() > 1.0001f)
            axis_.Normalize();
        angle_ = Simian::Radian(angle.Radians());
        dirty_ = true;
    }

    const Simian::Vector3& CTransform::Axis() const
    {
        return axis_;
    }

    const Simian::Radian& CTransform::Angle() const
    {
        return angle_;
    }

    const Simian::Matrix& CTransform::World() const
    {
        return world_;
    }

    void CTransform::World( const Simian::Matrix& val )
    {
        Simian::Vector4 rot;
        val.Decompose(position_, rot, scale_);

        // convert quaternion to axis angle
        angle_ = 2.0f * std::acos(rot.W());
        axis_ = Simian::Vector3(rot.X(), rot.Y(), rot.Z());
        axis_ /= std::sqrt(1.0f - rot.W() * rot.W());
    }

    Simian::Vector3 CTransform::Direction() const
    {
        f32 s = std::sin(angle_.Radians());
        f32 c = std::cos(angle_.Radians());
        f32 t = (1.0f - c) * axis_.Z();
        return Vector3(
            t * axis_.X() + axis_.Y() * s,
            t * axis_.Y() - axis_.X() * s,
            t * axis_.Z() + c);
    }

    //--------------------------------------------------------------------------

    void CTransform::dirtyChildren_()
    {
        for (u32 i = 0; i < childTransforms_.size(); ++i)
        {
            childTransforms_[i]->dirty_ = true;
            childTransforms_[i]->dirtyChildren_();
        }
    }

    void CTransform::computeWorld_(bool force)
    {
        if (force || (dirty_ && !computed_))
        {
            computed_ = !force;
            
            // find world: parent * local (srt)
            world_ = Simian::Matrix::Translation(position_) * 
                     Simian::Matrix::RotationAxisAngle(axis_, angle_) *
                     Simian::Matrix::Scale(scale_);

            if (parentTransform_)
            {
                parentTransform_->computeWorld_(force);
                world_ = parentTransform_->world_ * world_;
            }

            dirtyChildren_();
        }
    }

    void CTransform::updateTransform_( DelegateParameter& )
    {
        // get parent transform
        computeWorld_();
    }

    void CTransform::updatePostTransform_( DelegateParameter& )
    {
        dirty_ = false;
        computed_ = false;
    }

    //--------------------------------------------------------------------------

    void CTransform::OnSharedLoad()
    {
        RegisterCallback(P_TRANSFORM, Delegate::CreateDelegate<CTransform, &CTransform::updateTransform_>(this));
        RegisterCallback(P_POSTTRANSFORM, Delegate::CreateDelegate<CTransform, &CTransform::updatePostTransform_>(this));
    }

    void CTransform::OnAwake()
    {
        // check if i have a parent
        Entity* parentEntity = ParentEntity()->ParentEntity();
        if (parentEntity)
            parentEntity->ComponentByType(C_TRANSFORM, parentTransform_);
        computed_ = false;

        // find all children transforms
        for (u32 i = 0; i < ParentEntity()->Children().size(); ++i)
        {
            Simian::Entity* const child = ParentEntity()->Children()[i];
            Simian::CTransform* transform;
            child->ComponentByType(C_TRANSFORM, transform);
            if (transform) childTransforms_.push_back(transform);
        }

        computeWorld_();
    }

    void CTransform::OnReparent()
    {
        // remove from old parent
        if (parentTransform_)
        {
            std::vector<CTransform*>::iterator i = std::find(
                parentTransform_->childTransforms_.begin(),
                parentTransform_->childTransforms_.end(), this);
            if (i != parentTransform_->childTransforms_.end())
                parentTransform_->childTransforms_.erase(i);
        }

        // find the world position
        Simian::Vector3 worldPos = world_.Position();

        Entity* parentEntity = ParentEntity()->ParentEntity();
        if (parentEntity)
            parentEntity->ComponentByType(C_TRANSFORM, parentTransform_);
        else
            parentTransform_ = 0;
        computed_ = false;

        // compute the new local position and add to new parent
        if (parentTransform_)
        {
            parentTransform_->World().Inversed().Transform(worldPos);
            parentTransform_->childTransforms_.push_back(this);
        }

        // set the position correctly
        position_ = worldPos;
        dirty_ = true;

        computeWorld_();
    }

    void CTransform::RecomputeWorld()
    {
        // recompute the world matrix
        computeWorld_(true);
    }

    void CTransform::Serialize( FileObjectSubNode& data )
    {
        FileObjectSubNode* position = data.AddObject("Position");
        position->AddData("X", position_.X());
        position->AddData("Y", position_.Y());
        position->AddData("Z", position_.Z());

        FileObjectSubNode* scale = data.AddObject("Scale");
        scale->AddData("X", scale_.X());
        scale->AddData("Y", scale_.Y());
        scale->AddData("Z", scale_.Z());

        FileObjectSubNode* rotation = data.AddObject("Rotation");
        rotation->AddData("X", axis_.X());
        rotation->AddData("Y", axis_.Y());
        rotation->AddData("Z", axis_.Z());
        rotation->AddData("Angle", angle_.Degrees());
    }

    void CTransform::Deserialize( const FileObjectSubNode& data )
    {
        const FileObjectSubNode* positionNode = data.Object("Position");
        if (positionNode)
        {
            const FileDataSubNode* xNode = positionNode->Data("X");
            const FileDataSubNode* yNode = positionNode->Data("Y");
            const FileDataSubNode* zNode = positionNode->Data("Z");

            position_ = Simian::Vector3(
                xNode ? xNode->AsF32() : position_.X(),
                yNode ? yNode->AsF32() : position_.Y(),
                zNode ? zNode->AsF32() : position_.Z());
        }

        const FileObjectSubNode* scaleNode = data.Object("Scale");
        if (scaleNode)
        {
            const FileDataSubNode* xNode = scaleNode->Data("X");
            const FileDataSubNode* yNode = scaleNode->Data("Y");
            const FileDataSubNode* zNode = scaleNode->Data("Z");

            scale_ = Simian::Vector3(
                xNode ? xNode->AsF32() : scale_.X(),
                yNode ? yNode->AsF32() : scale_.Y(),
                zNode ? zNode->AsF32() : scale_.Z());
        }

        const FileObjectSubNode* rotationNode = data.Object("Rotation");
        if (rotationNode)
        {
            const FileDataSubNode* xNode = rotationNode->Data("X");
            const FileDataSubNode* yNode = rotationNode->Data("Y");
            const FileDataSubNode* zNode = rotationNode->Data("Z");
            const FileDataSubNode* angleNode = rotationNode->Data("Angle");

            axis_ = Simian::Vector3(
                xNode ? xNode->AsF32() : axis_.X(),
                yNode ? yNode->AsF32() : axis_.Y(),
                zNode ? zNode->AsF32() : axis_.Z());

            angle_ = angleNode ? Simian::Radian(Simian::Degree(angleNode->AsF32()).Radians()) : angle_;
        }
        dirty_ = true;
    }

    void CTransform::LookAt( const Vector3& target, const Vector3& upVec )
    {
        // create look at rotation matrix
        Vector3 forward = target - Position();
        forward.Normalize();
        Vector3 right = upVec.Cross(forward);
        Vector3 up = forward.Cross(right);

        // create quaternion from rotation matrix
        f32 w = std::sqrt(1.0f + right.X() + up.Y() + forward.Z()) / 2.0f;
        f32 w4 = 1.0f/(w * 4.0f);
        f32 x = (forward.Y() - up.Z()) * w4;
        f32 y = (right.Z() - forward.X()) * w4;
        f32 z = (up.X() - right.Y()) * w4;
        f32 d = 1.0f/std::sqrt(1.0f - w * w);

        // convert quaternion to axis angle
        angle_ = Simian::Radian(2.0f * std::acos(w));
        axis_ = Simian::Vector3(x * d, y * d, z * d);
    }

    void CTransform::LookAt( CTransform* transform, const Vector3& upVec )
    {
        LookAt(transform->Position(), upVec);
    }
}
