/*************************************************************************/
/*!
\file		GCFlamingSnakeEffect.cpp
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	2012/08/10
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*************************************************************************/
#include "GCFlamingSnakeEffect.h"
#include "ComponentTypes.h"
#include "CombatCommandDealDamage.h"
#include "GCPlayerLogic.h"
#include "GCPhysicsController.h"

#include "Simian\EngineComponents.h"
#include "Simian\EnginePhases.h"
#include "Simian\CParticleSystem.h"
#include "Simian\ParticleSystem.h"
#include "Simian\Scene.h"
#include "Simian\Math.h"
#include "Simian\CTransform.h"
#include "Simian\DebugRenderer.h"

namespace Descension
{
    Simian::FactoryPlant<Simian::EntityComponent, GCFlamingSnakeEffect> GCFlamingSnakeEffect::factoryPlant_(
        &Simian::GameFactory::Instance().ComponentFactory(), GC_FLAMINGSNAKEEFFECT);

    const static Simian::f32 FIRESNAKE_RADIUS = 0.2f;

    GCFlamingSnakeEffect::GCFlamingSnakeEffect()
        : deviationAmount_(0),
          deviation_(0.0f),
          transform_(0),
          flames_(0),
          duration_(3.0f),
          timer_(0),
          speed_(10.0f),
          autoDestroy_(true),
          dealDamage_(false),
          damage_(0)
    {

    }

    //--------------------------------------------------------------------------

    const Simian::Vector3& GCFlamingSnakeEffect::Direction() const
    {
        return direction_;
    }

    void GCFlamingSnakeEffect::Direction( const Simian::Vector3& val )
    {
        direction_ = val;
        direction_.Normalize();
        deviationVec_ = direction_.Cross(Simian::Vector3::UnitY);
        deviationVec_.Normalize();
    }

    Simian::f32 GCFlamingSnakeEffect::DeviationAmount() const
    {
        return deviationAmount_;
    }

    void GCFlamingSnakeEffect::DeviationAmount( Simian::f32 val )
    {
        deviationAmount_ = val;
    }

    Simian::f32 GCFlamingSnakeEffect::Duration() const
    {
        return duration_;
    }

    void GCFlamingSnakeEffect::Duration( Simian::f32 val )
    {
        duration_ = val;
    }

    bool GCFlamingSnakeEffect::AutoDestroy() const
    {
        return autoDestroy_;
    }

    void GCFlamingSnakeEffect::AutoDestroy( bool val )
    {
        autoDestroy_ = val;
    }

    bool GCFlamingSnakeEffect::DealDamage() const
    {
        return dealDamage_;
    }

    void GCFlamingSnakeEffect::DealDamage( bool val )
    {
        dealDamage_ = val;
    }

    Simian::s32 GCFlamingSnakeEffect::Damage() const
    {
        return damage_;
    }

    void GCFlamingSnakeEffect::Damage( Simian::s32 val )
    {
        damage_ = val;
    }

    //--------------------------------------------------------------------------

    void GCFlamingSnakeEffect::update_( Simian::DelegateParameter& param )
    {
        Simian::EntityUpdateParameter* p;
        param.As(p);

        // increment timer
        timer_ += p->Dt;

        // randomize deviation
        deviation_ += Simian::Math::Random(-deviationAmount_, deviationAmount_);

        // find direction
        Simian::Vector3 directionVec = deviationVec_ * deviation_ * p->Dt;
        directionVec += direction_;
        directionVec.Normalize();

        if (timer_ < duration_)
            transform_->Position(transform_->Position() + directionVec * speed_ * p->Dt);

        if (dealDamage_ && timer_ < duration_)
        {
            DebugRendererDrawCircle(transform_->WorldPosition(),  FIRESNAKE_RADIUS, Simian::Color(1.0f, 0.0f, 0.0f, 1.0f));

            Simian::f32 fireSnakeRange = FIRESNAKE_RADIUS + GCPlayerLogic::Instance()->Physics()->Radius();
            Simian::Vector3 distVec = GCPlayerLogic::Instance()->Transform().WorldPosition() - transform_->WorldPosition();
            if (distVec.LengthSquared() < fireSnakeRange * fireSnakeRange)
            {
                // deal damage
                timer_ = duration_ + p->Dt;
                CombatCommandSystem::Instance().AddCommand(NULL, GCPlayerLogic::Instance()->Attributes(), 
                    CombatCommandDealDamage::Create(damage_));
            }
        }

        if (timer_ > duration_)
            flames_->ParticleSystem()->Enabled(false);

        if (timer_ > duration_ + flames_->ParticleSystem()->LifeTime() + flames_->ParticleSystem()->RandomLifeTime())
        {
            // auto remove!
            if (autoDestroy_)
            {
                if (ParentEntity()->ParentEntity())
                    ParentEntity()->ParentEntity()->RemoveChild(ParentEntity());
                else
                    ParentScene()->RemoveEntity(ParentEntity());
            }
            else
                timer_ = 0;
        }
    }

    //--------------------------------------------------------------------------

    void GCFlamingSnakeEffect::OnSharedLoad()
    {
        RegisterCallback(Simian::P_UPDATE, 
            Simian::Delegate::CreateDelegate<GCFlamingSnakeEffect, &GCFlamingSnakeEffect::update_>(this));
    }

    void GCFlamingSnakeEffect::OnAwake()
    {
        ComponentByType(Simian::C_TRANSFORM, transform_);
        ComponentByType(Simian::C_PARTICLESYSTEM, flames_);

        flames_->ParticleSystem()->Enabled(true);

        // find orthogonal
        direction_.Normalize();
        deviationVec_ = direction_.Cross(Simian::Vector3::UnitY);
        deviationVec_.Normalize();
    }

    void GCFlamingSnakeEffect::OnDeinit()
    {

    }

    void GCFlamingSnakeEffect::Serialize( Simian::FileObjectSubNode& data )
    {
        data.AddData("DeviationAmount", deviationAmount_);
        data.AddData("Duration", duration_);
        data.AddData("Speed", speed_);
        data.AddData("AutoDestroy", autoDestroy_);
        data.AddData("Damage", damage_);
        data.AddData("DealDamage", dealDamage_);

        Simian::FileObjectSubNode* direction = data.AddObject("Direction");
        direction->AddData("X", direction_.X());
        direction->AddData("Y", direction_.Y());
        direction->AddData("Z", direction_.Z());
    }

    void GCFlamingSnakeEffect::Deserialize( const Simian::FileObjectSubNode& data )
    {
        data.Data("DeviationAmount", deviationAmount_, deviationAmount_);
        data.Data("Duration", duration_, duration_);
        data.Data("Speed", speed_, speed_);
        data.Data("AutoDestroy", autoDestroy_, autoDestroy_);
        data.Data("Damage", damage_, damage_);
        data.Data("DealDamage", dealDamage_, dealDamage_);

        const Simian::FileObjectSubNode* direction = data.Object("Direction");
        if (direction)
        {
            Simian::f32 x, y, z;
            direction->Data("X", x, direction_.X());
            direction->Data("Y", y, direction_.Y());
            direction->Data("Z", z, direction_.Z());
            direction_ = Simian::Vector3(x, y, z);
        }
    }
}
