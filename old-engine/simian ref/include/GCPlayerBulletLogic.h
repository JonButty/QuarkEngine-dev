/*************************************************************************/
/*!
\file		GCPlayerBulletLogic.h
\author 	Gavin Yeo Wei Ming, gavin.yeo, 290000111
\par    	email: gavin.yeo\@digipen.edu
\date   	2012/08/10
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*************************************************************************/
#ifndef DESCENSION_GCPLAYERBULLETLOGIC_H_
#define DESCENSION_GCPLAYERBULLETLOGIC_H_

#include "Simian/EntityComponent.h"

namespace Simian
{
    class CTransform;
}

namespace Descension
{
    class GCPhysicsController;

    class GCPlayerBulletLogic: public Simian::EntityComponent
    {
    private:
        Simian::CTransform* transform_;
        GCPhysicsController* physics_;
        GCAttributes* ownerAttribs_;

        Simian::f32 movementSpeed_;
        Simian::Vector3 movementVec_;
        Simian::f32 timer_;

        // factory plant
        static Simian::FactoryPlant<Simian::EntityComponent, GCPlayerBulletLogic> factoryPlant_;
    public:
        Simian::CTransform& Transform();
        void OwnerAttribs(GCAttributes* attribs);
    private:
        void update_(Simian::DelegateParameter& param);
    public:
        GCPlayerBulletLogic();

        void OnSharedLoad();
        void OnAwake();
        void OnInit();

        void Serialize(Simian::FileObjectSubNode& data);
        void Deserialize(const Simian::FileObjectSubNode& data);
    };
}

#endif
