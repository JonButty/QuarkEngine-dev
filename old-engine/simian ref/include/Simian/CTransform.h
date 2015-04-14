/************************************************************************/
/*!
\file		CTransform.h
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	August 10, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/************************************************************************/
#ifndef SIMIAN_CTRANSFORM_H_
#define SIMIAN_CTRANSFORM_H_

#include "SimianPlatform.h"
#include "EntityComponent.h"

#include "Vector3.h"
#include "Matrix.h"
#include "Angle.h"

namespace Simian
{
    class SIMIAN_EXPORT CTransform: public EntityComponent
    {
    private:
        static FactoryPlant<EntityComponent, CTransform> factoryPlant_;
        CTransform* parentTransform_;
        std::vector<CTransform*> childTransforms_;

        Simian::Vector3 position_;
        Simian::Vector3 scale_;
        Simian::Vector3 axis_;
        Simian::Radian angle_;
        Simian::Matrix world_;

        bool computed_;
        bool dirty_;
    public:
        CTransform* ParentTransform() const;
        const Simian::Vector3& Position() const;
        void Position(const Simian::Vector3& val);
        Simian::Vector3 WorldPosition() const;
        void WorldPosition(const Simian::Vector3& position);
        const Simian::Vector3 Scale() const;
        void Scale(const Simian::Vector3& val);
        void Rotation(const Simian::Vector3& axis, const Simian::Angle& angle);
        const Simian::Vector3& Axis() const;
        const Simian::Radian& Angle() const;
        const Simian::Matrix& World() const;
        void World(const Simian::Matrix& val);
        Simian::Vector3 Direction() const;
    private:
        void dirtyChildren_();
        void computeWorld_(bool force = false);
        void updateTransform_(DelegateParameter& param);
        void updatePostTransform_(DelegateParameter& param);
    public:
        CTransform();

        void OnSharedLoad();
        void OnAwake();
        void OnReparent();

        void RecomputeWorld();

        void Serialize(FileObjectSubNode& data);
        void Deserialize(const FileObjectSubNode& data);

        void LookAt(const Vector3& target, const Vector3& up = Vector3::UnitY);
        void LookAt(CTransform* transform, const Vector3& up = Vector3::UnitY);
    };
}

#endif
