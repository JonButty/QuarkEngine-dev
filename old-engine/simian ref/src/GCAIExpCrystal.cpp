/******************************************************************************/
/*!
\file		GCAIExpCrystal.cpp
\author 	Ngai Wen Sheng Jason, jason.ngai, 250002311
\par    	email: jason.ngai\@digipen.edu
\date   	2012/11/10
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/******************************************************************************/
#include "Simian/Debug.h"
#include "Simian/CModel.h"
#include "Simian/CTransform.h"
#include "Simian/AnimationController.h"
#include "Simian/CSoundEmitter.h"
#include "Simian/Scene.h"
#include "Simian/CParticleSystem.h"
#include "Simian/ParticleSystem.h"

#include "SaveManager.h"

#include "EntityTypes.h"
#include "ComponentTypes.h"
#include "Collision.h"
#include "EntityCollision.h"
#include "PathFinder.h"

#include "GCPhysicsController.h"
#include "GCAIExpCrystal.h"
#include "GCAttributes.h"
#include "GCEditor.h"
#include "GCPlayerLogic.h"

#include <iostream>
#include <sstream>

namespace Descension
{
    Simian::FactoryPlant<Simian::EntityComponent, GCAIExpCrystal> factoryPlant_(
        &Simian::GameFactory::Instance().ComponentFactory(), GC_AIEXPCRYSTAL);

    const Simian::u32 TOTAL_BREAK_SOUNDS = 4;

    GCAIExpCrystal::GCAIExpCrystal()
        :   GCAIBase(ES_IDLE, ES_TOTAL),
            animFrameRate_(48),
            gemID_(0),
            sounds_(0),
            totalDestroySounds_(5),
            prevHP_(0),
            numAnimation_(0),
            expReward_(0),
            healthPhase_(0)
    {
    }

    GCAIExpCrystal::~GCAIExpCrystal()
    {
    }

    //-Public-------------------------------------------------------------------

    void GCAIExpCrystal::Deserialize( const Simian::FileObjectSubNode& data )
    {
        GCAIBase::Deserialize(data);

        const Simian::FileDataSubNode* temp = data.Data("AnimationFrameRate");
        animFrameRate_ = temp? static_cast<Simian::u32>(temp->AsF32()) : animFrameRate_;

        data.Data("NumAnimation", numAnimation_, numAnimation_);
        data.Data("AnimationName", animationName_, animationName_);
        data.Data("ExpReward", expReward_, expReward_);
        data.Data("GemID", gemID_, gemID_);
    }

    void GCAIExpCrystal::Serialize( Simian::FileObjectSubNode& data)
    {
        GCAIBase::Serialize(data);
        data.AddData("AnimationFrameRate",animFrameRate_);
        
        data.AddData("NumAnimation", numAnimation_);
        data.AddData("AnimationName", animationName_);
        data.AddData("ExpReward", expReward_);
        data.AddData("GemID", gemID_);
    }

    void GCAIExpCrystal::OnAwake()
    {
        GCAIBase::OnAwake();

        aiType_ = AI_PASSIVE;

        fsm_[ES_IDLE].OnEnter = Simian::Delegate::CreateDelegate<GCAIExpCrystal, &GCAIExpCrystal::idleOnEnter_>(this);
        fsm_[ES_IDLE].OnUpdate = Simian::Delegate::CreateDelegate<GCAIExpCrystal, &GCAIExpCrystal::idleOnUpdate_>(this);
        fsm_[ES_DEATH].OnEnter = Simian::Delegate::CreateDelegate<GCAIExpCrystal, &GCAIExpCrystal::deathOnEnter_>(this);
        fsm_[ES_DEATH].OnUpdate = Simian::Delegate::CreateDelegate<GCAIExpCrystal, &GCAIExpCrystal::deathOnUpdate_>(this);
    }

    void GCAIExpCrystal::OnInit()
    {
        GCAIBase::OnInit();
        healthPhase_ = 0;
        ComponentByType(Simian::C_SOUNDEMITTER, sounds_);

#ifdef _DESCENDED
        if (GCEditor::EditorMode())
            return;
#endif

        if (SaveManager::Instance().CurrentData().Crystals[gemID_])
            fsm_.ChangeState(ES_DEATH);
    }

    void GCAIExpCrystal::OnDeinit()
    {
        GCAIBase::OnDeinit();
    }

    void GCAIExpCrystal::OnSharedLoad()
    {
        GCAIBase::OnSharedLoad();
    }

    //-Private------------------------------------------------------------------

    void GCAIExpCrystal::idleOnEnter_( Simian::DelegateParameter&)
    {
        model_->Controller().PlayAnimation("Idle", false);
        prevHP_ = attributes_->Health();
        healthPhase_ = 0;
    }

    void GCAIExpCrystal::idleOnUpdate_( Simian::DelegateParameter&)
    {
        PathFinder::Instance().SetNewPosition(transform_->World().Position(),physics_->Radius(),oldMin_,oldMax_);
        
        if (attributes_->Health() <= 0 || healthPhase_ >= numAnimation_)
        {
            fsm_.ChangeState(ES_DEATH);
            return;
        }
        
        Simian::u32 phase = static_cast<Simian::u32>((1.0f
                            - static_cast<Simian::f32>(attributes_->Health())
                            / attributes_->MaxHealth()) * (numAnimation_-1));
        if (attributes_->Health() < prevHP_)
        {
            GCPlayerLogic::Instance()->Attributes()->Experience(
                GCPlayerLogic::Instance()->Attributes()->Experience()+expReward_);
            prevHP_ = attributes_->Health();
            Simian::Entity* particles =
                ParentEntity()->ParentScene()->CreateEntity(E_REGENEXPPARTICLES);
            if ( particles )
            {
                ParentScene()->AddEntity(particles);
                Simian::CTransform* transferParticlestransform_;
                Simian::CParticleSystem* transferParticles_;
                particles->ComponentByType(Simian::C_TRANSFORM, transferParticlestransform_);
                particles->ComponentByType(Simian::C_PARTICLESYSTEM, transferParticles_);
                transferParticlestransform_->Position(
                    GCPlayerLogic::Instance()->Transform().World().Position());
                transferParticles_->ParticleSystem()->Enabled(true);
            }

            Simian::Entity* particles2 =
                ParentEntity()->ParentScene()->CreateEntity(E_REGENEXPPARTICLES);
            if ( particles2 )
            {
                ParentScene()->AddEntity(particles2);
                Simian::CTransform* transferParticlestransform_;
                Simian::CParticleSystem* transferParticles_;
                particles2->ComponentByType(Simian::C_TRANSFORM, transferParticlestransform_);
                particles2->ComponentByType(Simian::C_PARTICLESYSTEM, transferParticles_);
                transferParticlestransform_->Position(
                    physics_->Position());
                transferParticles_->ParticleSystem()->Enabled(true);
            }
        }

        if (phase > healthPhase_)
        {
            healthPhase_ = phase;
            std::stringstream sstr;
            sstr << animationName_ << healthPhase_;
            model_->Controller().PlayAnimation(sstr.str(), false, animFrameRate_);
            if (sounds_)
                sounds_->Play(Simian::Math::Random(1, 1+TOTAL_BREAK_SOUNDS));
        }
    }

    void GCAIExpCrystal::deathOnEnter_( Simian::DelegateParameter&  )
    {
        std::stringstream sstr;
        sstr << animationName_ << numAnimation_-1;
        model_->Controller().PlayAnimation(sstr.str(), false, animFrameRate_);
        physics_->Enabled(false);
        PathFinder::Instance().UnsetPosition(oldMin_,oldMax_);
        attributes_->Health(0);
        // Play destroy sound
        if (sounds_)
            sounds_->Play(0);

        //--Set it to be destroyed
        SaveManager::Instance().CurrentData().Crystals[gemID_] = 1;
    }

    void GCAIExpCrystal::deathOnUpdate_( Simian::DelegateParameter&  )
    {
    }

    void GCAIExpCrystal::overRideStateUpdate_( Simian::f32 dt )
    {
        dt;
    }
}
