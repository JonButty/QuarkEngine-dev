/*************************************************************************/
/*!
\file		GCEnemySpawnSpot.h
\author 	Gavin Yeo Wei Ming, gavin.yeo, 290000111
\par    	email: gavin.yeo\@digipen.edu
\date   	2012/08/10
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*************************************************************************/
#ifndef DESCENSION_GCENEMYSPAWNSPOT_H_
#define DESCENSION_GCENEMYSPAWNSPOT_H_

#include "Simian/EntityComponent.h"
#include "Simian/Angle.h"

namespace Simian
{
    class CTransform;
}

namespace Descension
{
    class GCAttributes;

    class GCEnemySpawnSpot: public Simian::EntityComponent
    {
    private:
        Simian::CTransform* transform_;

        Simian::f32 radius_;
        Simian::Radian angularDifference_;

        // factory plant
        static Simian::FactoryPlant<Simian::EntityComponent, GCEnemySpawnSpot> factoryPlant_;
    public:
        Simian::CTransform& Transform();
    private:
#ifdef _DESCENDED
        void update_(Simian::DelegateParameter& param);
#endif
    public:
        GCEnemySpawnSpot();

#ifdef _DESCENDED
        void OnSharedLoad();
#endif
        void OnAwake();

        GCAttributes* Spawn(Simian::s32 entityId);

        void Serialize(Simian::FileObjectSubNode& data);
        void Deserialize(const Simian::FileObjectSubNode& data);
    };
}

#endif
