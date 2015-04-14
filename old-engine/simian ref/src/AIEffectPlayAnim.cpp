/******************************************************************************/
/*!
\file		AIEffectPlayAnim.cpp
\author 	Jonathan Butt, j.butt, 250000211
\par    	email: j.butt\@digipen.edu
\date   	2012/11/01
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/******************************************************************************/
#include "Simian/AnimationController.h"
#include "Simian/CModel.h"
#include "Simian/Math.h"
#include "Simian/EngineComponents.h"
#include "Simian/EntityComponent.h"
#include "Simian/CSoundEmitter.h"

#include "AIEffectPlayAnim.h"
#include "GCDescensionCamera.h"

#include "GCAttributes.h"
#include "Simian/LogManager.h"
#include "GCPlayerLogic.h"

namespace Descension
{
    static const Simian::f32 TRY_AGAIN_TIME = 5.0f;
    static const Simian::f32 SOUND_TIME = 0.5f;

    Simian::FactoryPlant<AIEffectBase, AIEffectPlayAnim> AIEffectPlayAnim::factoryPlant_(
        AIEffectBase::createFactoryPlant_<AIEffectPlayAnim>(EFF_E_PLAYANIM));

    AIEffectPlayAnim::AIEffectPlayAnim()
        :   AIEffectBase(EFF_E_PLAYANIM),
            animationModel_(0),
            soundEmitter_(0),
            chanceHit_(0),
            fps_(0),
            soundPlayed_(false),
            stateTrigger_(0),
            playChance_(0),
            playTime_(0),
            timer_(TRY_AGAIN_TIME),
            tryAgainTimer_(0),
            animationName_(""),
            soundName_(""),
            soundNum_(13),
            fsm_(PS_IDLE,PS_TOTAL)
    {
    }

    AIEffectPlayAnim::~AIEffectPlayAnim()
    {
    }

    bool AIEffectPlayAnim::Update( Simian::f32 dt )
    {
        if(!Enable()|| GCPlayerLogic::Instance()->Attributes()->Health() < Simian::Math::EPSILON)
            return false;

        if(Target()->CurrentState()== stateTrigger_)
        {
            if(!chanceHit_ && tryAgainTimer_ >= TRY_AGAIN_TIME)
                chanceHit_ = (playChance_ >= Simian::Math::Random(0,100));

            if(chanceHit_)
            {
                tryAgainTimer_ = 0.0f;
                Target()->MovementSpeed(movementSpeed_);
                fsm_.Update(dt);
                return true;
            }
            tryAgainTimer_ += dt;
        }
        return false;
    }

    //--------------------------------------------------------------------------

    void AIEffectPlayAnim::onAwake_()
    {

    }

    void AIEffectPlayAnim::onInit_( GCAttributes*  )
    {
        Target()->ParentEntity()->ComponentByType(Simian::C_MODEL,animationModel_);
        Target()->ParentEntity()->ComponentByType(Simian::C_SOUNDEMITTER,soundEmitter_);

        if(!animationModel_ && !soundEmitter_)
            Enable(false);

        fsm_[PS_IDLE].OnEnter = Simian::Delegate::CreateDelegate<AIEffectPlayAnim, &AIEffectPlayAnim::idleOnEnter_>(this);
        fsm_[PS_IDLE].OnUpdate = Simian::Delegate::CreateDelegate<AIEffectPlayAnim, &AIEffectPlayAnim::idleOnUpdate_>(this);
        fsm_[PS_PLAY].OnEnter = Simian::Delegate::CreateDelegate<AIEffectPlayAnim, &AIEffectPlayAnim::playOnEnter_>(this);
        fsm_[PS_PLAY].OnUpdate = Simian::Delegate::CreateDelegate<AIEffectPlayAnim, &AIEffectPlayAnim::playOnUpdate_>(this);
    }

    void AIEffectPlayAnim::onUnload_()
    {

    }

    void AIEffectPlayAnim::serialize_( Simian::FileObjectSubNode& node )
    {
        node.AddData("AnimationName",animationName_);
        node.AddData("SoundName",soundName_);
        node.AddData("PlayTime",playTime_);
        node.AddData("PlayChance",playChance_);
        node.AddData("MovementSpeed",movementSpeed_);
        node.AddData("FPS",fps_);
        node.AddData("StateTrigger",stateTrigger_);
    }

    void AIEffectPlayAnim::deserialize_( const Simian::FileObjectSubNode& node )
    {
        const Simian::FileDataSubNode* data = node.Data("AnimationName");
        animationName_ = data && data->AsString() != "NULL" ? data->AsString() : animationName_;

        data = node.Data("SoundName");
        soundName_ = data && data->AsString() != "NULL" ? data->AsString() : soundName_;

        data = node.Data("MovementSpeed");
        movementSpeed_ = data ? data->AsF32() : movementSpeed_;

        data = node.Data("FPS");
        fps_ = data? data->AsU32() : fps_;

        data = node.Data("PlayTime");
        playTime_ = data? data->AsF32() : playTime_;

        data = node.Data("StateTrigger");
        stateTrigger_ = data? data->AsU32() : stateTrigger_;

        data = node.Data("PlayChance");
        playChance_ = data? data->AsF32() : playChance_;
    }

    void AIEffectPlayAnim::idleOnEnter_( Simian::DelegateParameter&  )
    {
        timer_ = 0.0f;
    }

    void AIEffectPlayAnim::idleOnUpdate_( Simian::DelegateParameter&  )
    {
        if(animationModel_)
            animationModel_->Controller().PlayAnimation(animationName_,false, fps_);
        if(soundEmitter_)
            soundEmitter_->Play(soundName_);
        fsm_.ChangeState(PS_PLAY);
    }

    void AIEffectPlayAnim::playOnEnter_( Simian::DelegateParameter&  )
    {
        soundPlayed_ = false;
    }

    void AIEffectPlayAnim::playOnUpdate_( Simian::DelegateParameter& param )
    {
        timer_ += param.As<Simian::EntityUpdateParameter>().Dt;

        if(timer_ > SOUND_TIME && !soundPlayed_)
        {
            soundPlayed_ = true;
            if(soundEmitter_)
                soundEmitter_->Play(soundNum_);
            // shake camera
            GCDescensionCamera::Instance()->Shake(0.2f, 0.2f);
        }

        if(timer_ > playTime_)
        {
            chanceHit_ = false;
            fsm_.ChangeState(PS_IDLE);
            animationModel_->Controller().PlayAnimation("Run");
        }
    }
}


