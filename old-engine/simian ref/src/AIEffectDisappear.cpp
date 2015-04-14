/******************************************************************************/
/*!
\file		AIEffectDisappear.cpp
\author 	Jonathan Butt, j.butt, 250000211
\par    	email: j.butt\@digipen.edu
\date   	2012/11/01
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/******************************************************************************/
#include "AIEffectDisappear.h"

#include "GCAttributes.h"

#include "Simian/Entity.h"
#include "Simian/EntityComponent.h"
#include "Simian/Scene.h"
#include "Simian/CTransform.h"
#include "Simian/CModel.h"
#include "Simian/AnimationController.h"
#include "Simian/CParticleSystem.h"
#include "Simian/ParticleSystem.h"
#include "Simian/CSoundEmitter.h"

#include "ComponentTypes.h"
#include "EntityTypes.h"
#include "GCPlayerLogic.h"
#include "Simian/Math.h"
#include "Simian/LogManager.h"

namespace Descension
{
    Simian::FactoryPlant<AIEffectBase, AIEffectDisappear> AIEffectDisappear::factoryPlant_(
        AIEffectDisappear::createFactoryPlant_<AIEffectDisappear>(EFF_E_DISAPPEAR));

	static const Simian::f32 SMOKE_PUFF_HEIGHT = 0.3f;

    AIEffectDisappear::AIEffectDisappear()
        :   AIEffectBase(EFF_E_DISAPPEAR),
            trans_(0),
            smokeTrans_(0),
            smoke_(0),
            smokeParticleSys_(0),
            visible_(true),
            stateTrigger_(2),
            timer_(0),
            disappearDelay_(2),
            smokeLifeTime_(0),
            sounds_(0),
            poofSoundNum_(3),
            fsm_(DS_PREDISAPPEAR,DS_TOTAL)

    {
    }

    AIEffectDisappear::~AIEffectDisappear()
    {
    }


    bool AIEffectDisappear::Update( Simian::f32 dt)
    {
        if( !Enable() )
            return false;

        // Disappear
        if(visible_ && Target()->CurrentState() == stateTrigger_)
        {
            Target()->MovementSpeed(0);
            fsm_.Update(dt);
            return true;
        }
        else if( !GCPlayerLogic::Instance()->PlayerControlOverride() && !visible_ && Target()->CurrentState() != stateTrigger_)
        {
            fsm_.Update(dt);
            return true;
        }
        else                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                               
        {
            smokeParticleSys_->Enabled(false);
            Target()->MovementSpeed(1);
        }
        return false;
    }

    //-Private------------------------------------------------------------------

    void AIEffectDisappear::onAwake_()
    {

    }

    void AIEffectDisappear::onInit_( GCAttributes*  )
    {
        if( Target() )
        {
            Target()->ComponentByType(Simian::C_TRANSFORM, trans_);
            Target()->ComponentByType(Simian::C_MODEL, model_);
            Target()->ComponentByType(Simian::C_SOUNDEMITTER,sounds_);
        }

        if(!trans_  || !model_)
            Enable(false);

        fsm_[DS_PREDISAPPEAR].OnEnter   = Simian::Delegate::CreateDelegate<AIEffectDisappear, &AIEffectDisappear::preDisappearOnEnter_>(this);
        fsm_[DS_PREDISAPPEAR].OnUpdate  = Simian::Delegate::CreateDelegate<AIEffectDisappear, & AIEffectDisappear::preDisappearOnUpdate_>(this);
        fsm_[DS_DISAPPEAR].OnEnter    = Simian::Delegate::CreateDelegate<AIEffectDisappear, &AIEffectDisappear::disappearOnEnter_>(this);
        fsm_[DS_DISAPPEAR].OnUpdate   = Simian::Delegate::CreateDelegate<AIEffectDisappear, &AIEffectDisappear::disappearOnUpdate_>(this);
        fsm_[DS_APPEAR].OnEnter  = Simian::Delegate::CreateDelegate<AIEffectDisappear, &AIEffectDisappear::appearOnEnter_>(this);
        fsm_[DS_APPEAR].OnUpdate = Simian::Delegate::CreateDelegate<AIEffectDisappear, &AIEffectDisappear::appearOnUpdate_>(this);

        smoke_ = Target()->ParentEntity()->ParentScene()->CreateEntity(E_SMOKEPUFF);
        if(!smoke_)
        {
            Enable(false);
            return;
        }

        smoke_->ComponentByType(Simian::C_TRANSFORM,smokeTrans_);

        if(!smokeTrans_)
        {
            Enable(false);
            return;
        }

        Simian::CParticleSystem* cParticleSystem = 0;
        smoke_->ChildByName("Particles")->ComponentByType(Simian::C_PARTICLESYSTEM,cParticleSystem);

        if(!cParticleSystem)
        {
            Enable(false);
            return;
        }
        smokeParticleSys_ = cParticleSystem->ParticleSystem();
        smokeLifeTime_ = smokeParticleSys_->LifeTime();
        smokeParticleSys_->Enabled(false);
        Target()->ParentEntity()->ParentScene()->AddEntity(smoke_);
    }

    void AIEffectDisappear::onUnload_()
    {
        Target()->ParentEntity()->ParentScene()->RemoveEntity(smoke_);
    }

    void AIEffectDisappear::serialize_( Simian::FileObjectSubNode& node )
    {
        node.AddData("StateTrigger",stateTrigger_);
        node.AddData("DisappearDelay",disappearDelay_);
        node.AddData("PoofSoundNum",poofSoundNum_);
    }

    void AIEffectDisappear::deserialize_( const Simian::FileObjectSubNode& node )
    {
        const Simian::FileDataSubNode* data = node.Data("StateTrigger");
        stateTrigger_ = data? data->AsU32() : stateTrigger_;

        data = node.Data("DisappearDelay");
        disappearDelay_ = data? data->AsF32() : disappearDelay_;

        data = node.Data("PoofSoundNum");
        poofSoundNum_ = data? data->AsU32() : poofSoundNum_;
    }

    void AIEffectDisappear::preDisappearOnEnter_( Simian::DelegateParameter&  )
    {
        timer_ = 0.0f;
        model_->Controller().PlayAnimation("AttackDualWield 1 Down");
    }

    void AIEffectDisappear::preDisappearOnUpdate_( Simian::DelegateParameter& param )
    {
        timer_ += param.As<Simian::EntityUpdateParameter>().Dt;

        if(timer_ > disappearDelay_)
        {
            fsm_.ChangeState(DS_DISAPPEAR);
            addSmoke_();
        }
    }

    void AIEffectDisappear::disappearOnEnter_( Simian::DelegateParameter&  )
    {
        timer_ = 0.0f;
        //model_->Visible(false);
        trans_->Scale(Simian::Vector3::Zero);

        if (sounds_)
            sounds_->Play(poofSoundNum_);
    }

    void AIEffectDisappear::disappearOnUpdate_( Simian::DelegateParameter& param )
    {
        timer_ += param.As<Simian::EntityUpdateParameter>().Dt;

        if(timer_ > smokeLifeTime_)
        {
            visible_ = false;
            smokeParticleSys_->Enabled(false);
            fsm_.ChangeState(DS_APPEAR);
        }
    }

    void AIEffectDisappear::appearOnEnter_( Simian::DelegateParameter&  )
    {
        if (sounds_)
            sounds_->Play(poofSoundNum_+1);
    }

    void AIEffectDisappear::appearOnUpdate_( Simian::DelegateParameter&  )
    {
        addSmoke_();
        visible_ = true;
        trans_->Scale(Simian::Vector3(1,1,1));
        fsm_.ChangeState(DS_PREDISAPPEAR);
    }

    void AIEffectDisappear::addSmoke_()
    {
		smokeTrans_->Position(Simian::Vector3(trans_->Position().X(), trans_->Position().Y()+SMOKE_PUFF_HEIGHT, trans_->Position().Z()));
        smokeParticleSys_->Enabled(true);
    }
}
