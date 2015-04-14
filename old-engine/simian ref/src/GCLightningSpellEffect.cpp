/*************************************************************************/
/*!
\file		GCLightningSpellEffect.cpp
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	2012/08/10
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*************************************************************************/
#include "GCLightningSpellEffect.h"
#include "ComponentTypes.h"
#include "GCLightningEffect.h"

#include "Simian\EnginePhases.h"
#include "Simian\EngineComponents.h"
#include "Simian\CTransform.h"
#include "Simian\Matrix.h"
#include "Simian\Vector3.h"
#include "Simian\Math.h"
#include "Simian\Interpolation.h"
#include "Simian\CModelColorOperation.h"
#include "Simian\Scene.h"
#include "Simian\CSoundEmitter.h"

namespace Descension
{
    const Simian::f32 Y_OFFSET = 5.0f;
    Simian::FactoryPlant<Simian::EntityComponent, GCLightningSpellEffect> GCLightningSpellEffect::factoryPlant_(
        &Simian::GameFactory::Instance().ComponentFactory(), GC_LIGHTNINGSPELLEFFECT);

    const Simian::u32 TOTAL_LIGHTNING_SOUNDS = 2;

    GCLightningSpellEffect::GCLightningSpellEffect()
        : fsm_(LSS_DROP, LSS_TOTAL),
          autoDestroy_(true),
          sounds_(0)
    {
    }

    //--------------------------------------------------------------------------

    void GCLightningSpellEffect::update_( Simian::DelegateParameter& param )
    {
        Simian::EntityUpdateParameter* p;
        param.As(p);
        fsm_.Update(p->Dt);
    }

    void GCLightningSpellEffect::dropEnter_( Simian::DelegateParameter& )
    {
        // Play random sound
        if (sounds_)
            sounds_->Play(Simian::Math::Random(0, TOTAL_LIGHTNING_SOUNDS));

        timer_ = 0.0f;
        flickerTimer_ = 0.0f;

        mainBeam_[0]->Origin(transform_->World().Position() + Simian::Vector3(0, Y_OFFSET, 0));
        mainBeam_[1]->Origin(transform_->World().Position() + Simian::Vector3(0, Y_OFFSET, 0));

        mainBeam_[0]->Target(transform_->World().Position() + Simian::Vector3(0, Y_OFFSET, 0));
        mainBeam_[1]->Target(transform_->World().Position() + Simian::Vector3(0, Y_OFFSET, 0));

        mainBeam_[0]->Visible(true);
        mainBeam_[1]->Visible(true);

        lightColor_->Color(Simian::Color(0.5f, 0.5f, 1.0f, 0.0f));
    }

    void GCLightningSpellEffect::dropUpdate_( Simian::DelegateParameter& param)
    {
        const Simian::f32 DROP_DURATION = 0.2f;
        const Simian::f32 WAIT_DURATION = DROP_DURATION + 0.1f;
        const Simian::f32 FLICKER_DURATION = 0.05f;

        Simian::FiniteStateUpdateParameter* p;
        param.As(p);
        timer_ += p->Dt;
        flickerTimer_ += p->Dt;

        if (flickerTimer_ > FLICKER_DURATION && timer_ > DROP_DURATION)
        {
            flickerTimer_ = 0.0f;
            lightColor_->Color(Simian::Color(0.5f, 0.5f, 1.0f, 
                lightColor_->Color().A() < 0.5f ?
                Simian::Math::Random(0.7f, 1.0f) :
                Simian::Math::Random(0.0f, 0.3f)));
        }

        if (timer_ < DROP_DURATION)
        {
            float intp = timer_/DROP_DURATION;
            intp = Simian::Math::Clamp(intp, 0.0f, 1.0f);

            Simian::Vector3 target = Simian::Interpolation::Interpolate(
                transform_->World().Position() + Simian::Vector3(0, Y_OFFSET, 0),
                transform_->World().Position(), intp, Simian::Interpolation::Linear);

            mainBeam_[0]->Target(target);
            mainBeam_[1]->Target(target);
        }
        else if (timer_ < WAIT_DURATION)
        {
            mainBeam_[0]->Target(transform_->World().Position());
            mainBeam_[1]->Target(transform_->World().Position());
        }
        else
        {
            fsm_.ChangeState(LSS_STRIKE);
        }
    }

    void GCLightningSpellEffect::strikeEnter_( Simian::DelegateParameter& )
    {
        timer_ = 0.0f;

        Simian::f32 angle = 0.0f;
        for (Simian::u32 i = 0; i < 5; ++i)
        {
            trails_[i].Start = transform_->World().Position();
            trails_[i].End = transform_->World().Position();

            trails_[i].Direction = Simian::Vector3(std::cos(angle), 0, std::sin(angle));
            angle += Simian::Math::TAU/5.0f;

            childBeams_[i]->Target(trails_[i].Start);
            childBeams_[i]->Origin(trails_[i].End);
            childBeams_[i]->Visible(true);
        }
    }

    void GCLightningSpellEffect::strikeUpdate_( Simian::DelegateParameter& param )
    {
        const Simian::f32 SPEED = 3.0f;
        const Simian::f32 TRAIL_LENGTH = 1.0f;
        const Simian::f32 MAX_RADIUS = 1.0f;
        const Simian::f32 DURATION = 2.0f;
        const Simian::f32 DROP_DURATION = 0.2f;
        const Simian::f32 FLICKER_DURATION = 0.05f;
        const Simian::f32 FLICKER_TOTAL = 0.5f;

        Simian::FiniteStateUpdateParameter* p;
        param.As(p);

        timer_ += p->Dt;
        flickerTimer_ += p->Dt;

        if (flickerTimer_ > FLICKER_DURATION && timer_ < FLICKER_TOTAL)
        {
            flickerTimer_ = 0.0f;
            lightColor_->Color(Simian::Color(0.5f, 0.5f, 1.0f, 
                lightColor_->Color().A() < 0.5f ?
                Simian::Math::Random(0.7f, 1.0f) :
                Simian::Math::Random(0.0f, 0.3f)));
        }
        else if (timer_ > FLICKER_TOTAL)
            lightColor_->Color(Simian::Color(0.5f, 0.5f, 1.0f, 0.0f));

        if (timer_ < DROP_DURATION)
        {
            float intp = timer_/DROP_DURATION;
            intp = Simian::Math::Clamp(intp, 0.0f, 1.0f);

            Simian::Vector3 origin = Simian::Interpolation::Interpolate(
                transform_->World().Position() + Simian::Vector3(0, Y_OFFSET, 0),
                transform_->World().Position(), intp, Simian::Interpolation::Linear);

            mainBeam_[0]->Origin(origin);
            mainBeam_[1]->Origin(origin);
        }
        else
        {
            mainBeam_[0]->Visible(false);
            mainBeam_[1]->Visible(false);
        }

        for (Simian::u32 i = 0; i < 5; ++i)
        {
            if ((trails_[i].Start - transform_->World().Position()).LengthSquared() < MAX_RADIUS * MAX_RADIUS)
                trails_[i].Start += trails_[i].Direction * SPEED * p->Dt;
            if (((trails_[i].End - trails_[i].Start).LengthSquared() > TRAIL_LENGTH * TRAIL_LENGTH ||
                (trails_[i].Start - transform_->World().Position()).LengthSquared() >= MAX_RADIUS * MAX_RADIUS) &&
                (trails_[i].End - transform_->World().Position()).LengthSquared() < MAX_RADIUS * MAX_RADIUS)
                trails_[i].End += trails_[i].Direction * SPEED * p->Dt;
            if ((trails_[i].End - transform_->World().Position()).LengthSquared() >= MAX_RADIUS * MAX_RADIUS)
                childBeams_[i]->Visible(false);

            childBeams_[i]->Target(trails_[i].Start);
            childBeams_[i]->Origin(trails_[i].End);
        }

        if (timer_ > DURATION)
        {
            // supposed to stop but repeat for testing
            if (autoDestroy_)
            {
                if (ParentEntity()->ParentEntity())
                    ParentEntity()->ParentEntity()->RemoveChild(ParentEntity());
                else
                    ParentScene()->RemoveEntity(ParentEntity());
            }
            else
                fsm_.ChangeState(LSS_DROP);
        }
    }

    //--------------------------------------------------------------------------

    void GCLightningSpellEffect::OnSharedLoad()
    {
        RegisterCallback(Simian::P_UPDATE, 
            Simian::Delegate::CreateDelegate<GCLightningSpellEffect, &GCLightningSpellEffect::update_>(this));
    }

    void GCLightningSpellEffect::OnAwake()
    {
        ComponentByType(Simian::C_TRANSFORM, transform_);
        ComponentByType(Simian::C_SOUNDEMITTER, sounds_);

        // get all the needed component handles
        ParentEntity()->ChildByName("MainBeam")->ComponentByType(GC_LIGHTNINGEFFECT, mainBeam_[0]);
        ParentEntity()->ChildByName("MainBeamSub")->ComponentByType(GC_LIGHTNINGEFFECT, mainBeam_[1]);

        // get the floor beams
        ParentEntity()->ChildByName("FloorBeam1")->ComponentByType(GC_LIGHTNINGEFFECT, childBeams_[0]);
        ParentEntity()->ChildByName("FloorBeam2")->ComponentByType(GC_LIGHTNINGEFFECT, childBeams_[1]);
        ParentEntity()->ChildByName("FloorBeam3")->ComponentByType(GC_LIGHTNINGEFFECT, childBeams_[2]);
        ParentEntity()->ChildByName("FloorBeam4")->ComponentByType(GC_LIGHTNINGEFFECT, childBeams_[3]);
        ParentEntity()->ChildByName("FloorBeam5")->ComponentByType(GC_LIGHTNINGEFFECT, childBeams_[4]);

        ComponentByType(Simian::C_MODELCOLOROPERATION, lightColor_);

        fsm_[LSS_DROP].OnEnter = Simian::Delegate::CreateDelegate<GCLightningSpellEffect, &GCLightningSpellEffect::dropEnter_>(this);
        fsm_[LSS_DROP].OnUpdate = Simian::Delegate::CreateDelegate<GCLightningSpellEffect, &GCLightningSpellEffect::dropUpdate_>(this);

        fsm_[LSS_STRIKE].OnEnter = Simian::Delegate::CreateDelegate<GCLightningSpellEffect, &GCLightningSpellEffect::strikeEnter_>(this);
        fsm_[LSS_STRIKE].OnUpdate = Simian::Delegate::CreateDelegate<GCLightningSpellEffect, &GCLightningSpellEffect::strikeUpdate_>(this);
    }

    void GCLightningSpellEffect::OnDeinit()
    {
    }

    void GCLightningSpellEffect::Serialize( Simian::FileObjectSubNode& data )
    {
        data.AddData("AutoDestroy", autoDestroy_);
    }

    void GCLightningSpellEffect::Deserialize( const Simian::FileObjectSubNode& data )
    {
        data.Data("AutoDestroy", autoDestroy_, autoDestroy_);
    }
}
