/******************************************************************************/
/*!
\file		GCRockDebris.h
\author 	Jonathan Butt, j.butt, 250000211
\par    	email: j.butt\@digipen.edu
\date   	2012/12/07
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/******************************************************************************/
#ifndef DESCENSION_GCROCKDEBRIS_H_
#define DESCENSION_GCROCKDEBRIS_H_

#include "Simian/EntityComponent.h"
#include "Simian/CTransform.h"

namespace Simian
{
    class Entity;
    class CSoundEmitter;
}

namespace Descension
{
    class GCAttributes;

    class GCRockDebris : public Simian::EntityComponent
    {
    private:
        Simian::Entity* shadow_;
        Simian::CTransform* rockTrans_;
        Simian::CTransform* shadowTrans_;
        Simian::CTransform playerTrans_;
        GCAttributes* attribs_;
        Simian::CSoundEmitter* sounds_;
        Simian::u32 damage_;
        Simian::f32 radius_;
        Simian::f32 radiusSqr_;
        Simian::f32 speed_;
        Simian::f32 startHeight_;
        Simian::f32 minXPos_, minZPos_, maxXPos_, maxZPos_;
        Simian::f32 randAngularVel_;
        Simian::Vector3 dShadowScale_;
        Simian::Vector3 randRotationAxis_;
        static Simian::FactoryPlant<Simian::EntityComponent, GCRockDebris> factoryPlant_;
    public:
        void Radius(Simian::f32 val);
        Simian::f32 Speed() const;
        void Speed(Simian::f32 val);
        Simian::f32 StartHeight() const;
        void StartHeight(Simian::f32 val);
    private:
        void update_( Simian::DelegateParameter& param );
        void dealDamage_();
    public:
        GCRockDebris();
        ~GCRockDebris();
        void OnSharedLoad();

        void OnInit();

        void DealDamage(Simian::s32 damage, GCAttributes* source = 0, bool flash = true);

        void Serialize(Simian::FileObjectSubNode& data);
        void Deserialize(const Simian::FileObjectSubNode& data);

        void SetRandomSpawnPos(Simian::f32 minX, Simian::f32 minZ, Simian::f32 maxX, Simian::f32 maxZ);
        void Damage(Simian::u32 damage);
    };
}
#endif
