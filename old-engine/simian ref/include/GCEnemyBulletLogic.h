/*************************************************************************/
/*!
\file		GCEnemyBulletLogic.h
\author 	Gavin Yeo Wei Ming, gavin.yeo, 290000111
\par    	email: gavin.yeo\@digipen.edu
\date   	2012/08/10
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*************************************************************************/
#ifndef DESCENSION_GCENEMYBULLETLOGIC_H_
#define DESCENSION_GCENEMYBULLETLOGIC_H_

#include "Simian/EntityComponent.h"

namespace Simian
{
    class CTransform;
}

namespace Descension
{
    class GCPhysicsController;

    class GCEnemyBulletLogic: public Simian::EntityComponent
    {
    private:
        Simian::CTransform* transform_;
        GCPhysicsController* physics_;

        Simian::f32 movementSpeed_;
        Simian::Vector3 movementVec_;
        
        Simian::f32 timer_;
        GCAttributes* ownerAttribs_;

        // factory plant
        static Simian::FactoryPlant<Simian::EntityComponent, GCEnemyBulletLogic> factoryPlant_;
    public:
        Simian::CTransform& Transform();
        void OwnerAttribs(GCAttributes* attribs);

        Simian::Vector3 MovementVec() const;
        void MovementVec(Simian::Vector3 val);

    private:
        void update_(Simian::DelegateParameter& param);
    public:
        GCEnemyBulletLogic();

        void OnSharedLoad();
        void OnAwake();
        void OnInit();

        void Serialize(Simian::FileObjectSubNode& data);
        void Deserialize(const Simian::FileObjectSubNode& data);
    };
}

#endif
