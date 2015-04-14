/******************************************************************************/
/*!
\file		GCMajenTriangle.cpp
\author 	Bryan Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	2012/08/10
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/******************************************************************************/
#include "GCMajenTriangle.h"
#include "ComponentTypes.h"

#include "Simian\EngineComponents.h"
#include "Simian\EnginePhases.h"
#include "Simian\CTransform.h"
#include "Simian\Math.h"
#include "Simian\Scene.h"
#include "Simian\LogManager.h"
#include "EntityTypes.h"
#include "GCFlameEffect.h"
#include "GCPlayerLogic.h"
#include "CombatCommandSystem.h"
#include "CombatCommandDealDamage.h"
#include "GCParticleEmitterTriangle.h"

namespace Descension
{
    Simian::FactoryPlant<Simian::EntityComponent, GCMajenTriangle> GCMajenTriangle::factoryPlant_(
        &Simian::GameFactory::Instance().ComponentFactory(), GC_MAJENTRIANGLE);

    GCMajenTriangle::GCMajenTriangle()
        : castingTime_(0.0f),
          timer_(0.0f),
          ownerAttribs_(0),
          damage_(20)
    {
    }

    //--------------------------------------------------------------------------

    Simian::f32 GCMajenTriangle::CastingTime() const
    {
        return castingTime_;
    }

    void GCMajenTriangle::CastingTime( Simian::f32 val )
    {
        castingTime_ = val;
    }

    void GCMajenTriangle::OwnerAttribs(GCAttributes* ownerAttribs)
    {
        ownerAttribs_ = ownerAttribs;
    }

    void GCMajenTriangle::Damage(Simian::u32 damage)
    {
        damage_ = damage;
    }

    const Simian::Vector3* GCMajenTriangle::Points() const
    {
        return points_;
    }

    void GCMajenTriangle::Points( Simian::Vector3* val )
    {
        memcpy(points_, val, sizeof(Simian::Vector3) * 3);
    }

    //--------------------------------------------------------------------------

    void GCMajenTriangle::update_( Simian::DelegateParameter& param )
    {
        Simian::EntityUpdateParameter* p;
        param.As(p);

        timer_ += p->Dt;
        Simian::f32 intp = timer_/castingTime_;
        intp = Simian::Math::Clamp(intp, 0.0f, 1.0f);

        for (Simian::u32 i = 0; i < 3; ++i)
        {
            markers_[i]->Position(points_[i] + Simian::Vector3(0.0f, 0.15f, 0.0f));
            markers_[i]->Scale(Simian::Vector3(1.0f - intp, 1.0f - intp, 1.0f - intp));
        }

        if (intp >= 1.0f)
        {
            // create particles
            GCFlameEffect* flame;
            Simian::Entity* entity = ParentScene()->CreateEntity(E_FLAMEEFFECT);
            entity->ComponentByType(GC_FLAMEEFFECT, flame);
            flame->Vertices(points_);

            ParentScene()->AddEntity(entity);

            // destroy this
            if (ParentEntity()->ParentEntity())
                ParentEntity()->ParentEntity()->RemoveChild(ParentEntity());
            else
                ParentScene()->RemoveEntity(ParentEntity());

            // Deal damage only if player is within triangle
            Simian::Vector3 playerPos3 = GCPlayerLogic::Instance()->Transform().World().Position();
            Simian::Vector2 normalsArray[3] =
            {
                Simian::Vector2(points_[0].Z()-points_[1].Z(), points_[1].X()-points_[0].X()),
                Simian::Vector2(points_[1].Z()-points_[2].Z(), points_[2].X()-points_[1].X()),
                Simian::Vector2(points_[2].Z()-points_[0].Z(), points_[0].X()-points_[2].X())
            };
            for (Simian::u32 i = 0; i < 3; ++i)
            {
                Simian::Vector2 playerToPos((playerPos3-points_[i]).X(), (playerPos3-points_[i]).Z());
                if (playerToPos.Dot(normalsArray[i]) > 0.0f)
                    return;
            }
            GCAttributes* playerAtt = GCPlayerLogic::Instance()->Attributes();
            CombatCommandSystem::Instance().AddCommand(ownerAttribs_, playerAtt,
                        CombatCommandDealDamage::Create(damage_));
        }
    }

    //--------------------------------------------------------------------------

    void GCMajenTriangle::OnSharedLoad()
    {
        RegisterCallback(Simian::P_UPDATE, Simian::Delegate::CreateDelegate<GCMajenTriangle, &GCMajenTriangle::update_>(this));
    }

    void GCMajenTriangle::OnAwake()
    {
        Simian::Entity* entity = ParentEntity()->ChildByName("Marker1");
        entity->ComponentByType(Simian::C_TRANSFORM, markers_[0]);

        entity = ParentEntity()->ChildByName("Marker2");
        entity->ComponentByType(Simian::C_TRANSFORM, markers_[1]);

        entity = ParentEntity()->ChildByName("Marker3");
        entity->ComponentByType(Simian::C_TRANSFORM, markers_[2]);

        GCParticleEmitterTriangle* emitter;
        entity = ParentScene()->CreateEntity(E_EMBERSEFFECT);

        Simian::Entity* child = entity->ChildByName("Smoke");
        child->ComponentByType(GC_PARTICLEEMITTERTRIANGLE, emitter);
        emitter->Vertices(points_);

        child = entity->ChildByName("Floaty");
        child->ComponentByType(GC_PARTICLEEMITTERTRIANGLE, emitter);
        emitter->Vertices(points_);

        ParentScene()->AddEntity(entity);
    }
}
