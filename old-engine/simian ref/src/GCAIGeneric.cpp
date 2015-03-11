/******************************************************************************/
/*!
\file		GCAIGeneric.cpp
\author 	Jonathan Butt, j.butt, 250000211
\par    	email: j.butt\@digipen.edu
\date   	2012/08/10
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/******************************************************************************/
#include "Simian/Debug.h"
#include "Simian/EnginePhases.h"
#include "Simian/CModel.h"
#include "Simian/CModelColorOperation.h"
#include "Simian/CTransform.h"
#include "Simian/AnimationController.h"
#include "Simian/Interpolation.h"
#include "Simian/Math.h"
#include "Simian/Scene.h"
#include "Simian/DebugRenderer.h"
#include "Simian/Delegate.h"
#include "Simian/CSoundEmitter.h"
#include "Simian/CParticleSystem.h"
#include "Simian/ParticleSystem.h"

#include "EntityTypes.h"
#include "ComponentTypes.h"
#include "Collision.h"
#include "EntityCollision.h"
#include "TileWallCollision.h"
#include "CombatCommandSystem.h"
#include "CombatCommandDealDamage.h"

#include "GCTileMap.h"
#include "GCPhysicsController.h"
#include "GCPlayerLogic.h"
#include "GCAIGeneric.h"
#include "Pathfinder.h"
#include "GCAttributes.h"
#include "GCEditor.h"
#include "GCEnemyBulletLogic.h"

#include <vector>
#include <iostream>
#include "Simian/LogManager.h"

namespace Descension
{
	Simian::FactoryPlant<Simian::EntityComponent, GCAIGeneric> factoryPlant_(
		&Simian::GameFactory::Instance().ComponentFactory(), GC_AIGENERIC);

	static const Simian::f32 IDLE_BUFFER_TIME = 1.0f;
    static const Simian::f32 SEEK_BUFFER_TIME = 0.25f;

    // Sounds
    static const Simian::u32 TOTAL_ALERT_SOUNDS = 3;
    static const Simian::u32 TOTAL_DIE_SOUNDS = 2;
    static const Simian::u32 TOTAL_MELEE_GET_HIT_SOUNDS = 2;

	GCAIGeneric::GCAIGeneric()
		:   GCAIBase(ES_IDLE, ES_TOTAL),
			tMap_(0),
			playerLogic_(0),
			playerTransform_(0),
			playerPhysics_(0),
            sounds_(0),
            spawnParticles_(0),
            playerAtt_(0),
			isRanged_(false),
			hasAttacked_(false),
			seekIdle_(false),
			shadowShown_(false),
            effectsOnSummon_(false),
			attackAnimationsTotal_(1),
		    attackAnimationPrefix_("Attack"),
            shootAnimationPrefix_("Shoot"),
			animationFrameRate_(48),
            bulletType_(BT_BASIC),
            idleBufferTimer_(0.0f),
            seekRad_(0.0f), 
            lostRad_(0.0f),  
            attackRad_(0.0f), 
            skillRad_(0.0f), 
            attackDist_(0.2f),
            seekDist_(1.0f),
            lostDist_(1.0f),
            skillDist_(1.0f),
            seekIdleDist_(1.0f),
			seekRadiusSqr_(0.0f),
			attackRadiusSqr_(0.0f),
            skillRadiusSqr_(0.0f),
            lostRadiusSqr_(1.0f),
            attackTimer_(0.0f),
            seekBufferTimer_(0.0f),
            steerTimer_(0.0f),
			attackTimeTotal_(1.5f),
			attackHitDelay_(1.0f),
			bulletStartHeight_(1.0f),
            runSoundTimer_(0.0f),
            runSoundTimerMax_(0.52f),
            maxSteerTime_(0.2f),
			oldMin_(-1.f,-1.f),
			oldMax_(-1.f,-1.f),
			vecToPlayer_(Simian::Vector3::Zero),
			nextTile_(-1.0f,0.0f,-1.0f),
			prevDir_(Simian::Vector3::Zero),
			startDir_(Simian::Vector3::Zero),
			finalDir_(Simian::Vector3::Zero),
            overrideFsm_(OS_SEEK,OS_TOTAL),
            currentAttackAnimation_("")
	{ 
	}

	//-Public-------------------------------------------------------------------

	bool GCAIGeneric::HasAttacked() const
	{
		return hasAttacked_;
	}

	Simian::f32 GCAIGeneric::SeekRadiusSqr() const
	{
		return seekRadiusSqr_;
	}

	Simian::f32 GCAIGeneric::AttackRadiusSqr() const
	{
		return attackRadiusSqr_;
	}

	Simian::f32 GCAIGeneric::LostRadiusSqr() const
	{
		return lostRadiusSqr_;
	}

    Simian::CSoundEmitter* GCAIGeneric::Sounds() const
    {
        return sounds_;
    }

	//-Public-------------------------------------------------------------------

	void GCAIGeneric::Deserialize( const Simian::FileObjectSubNode& data )
	{
		GCAIBase::Deserialize(data);

		///////////////
		// Variables //
		///////////////

		const Simian::FileDataSubNode* temp = data.Data("SeekDist");
		seekDist_ = temp? temp->AsF32() : seekDist_;

        temp = data.Data("SkillDist");
        skillDist_ = temp? temp->AsF32() : skillDist_;

		temp = data.Data("AttackDist");
		attackDist_ = temp? temp->AsF32() : attackDist_;

		temp = data.Data("AttackTimeTotal");
		attackTimeTotal_ = temp? temp->AsF32() : attackTimeTotal_;

		temp = data.Data("AttackHitDelay");
		attackHitDelay_ = temp? temp->AsF32() : attackHitDelay_;

		temp = data.Data("AttackAnimationsTotal");
		attackAnimationsTotal_ = temp? temp->AsU32() : attackAnimationsTotal_;

		temp = data.Data("AttackAnimationPrefix");
		attackAnimationPrefix_ = temp? temp->AsString() : attackAnimationPrefix_;

        temp = data.Data("ShootAnimationPrefix");
		shootAnimationPrefix_ = temp? temp->AsString() : shootAnimationPrefix_;

		temp = data.Data("BulletStartHeight");
		bulletStartHeight_ = temp? temp->AsF32() : bulletStartHeight_;
        
		temp = data.Data("AnimationFrameRate");
		animationFrameRate_ = temp? temp->AsU32() : animationFrameRate_;

        temp = data.Data("BulletType");
		bulletType_ = temp? temp->AsU32() : bulletType_;

		temp = data.Data("LostDist");
		lostDist_ = temp? temp->AsF32() : lostDist_;
        
		temp = data.Data("IsRanged");
		isRanged_ = temp? temp->AsBool() : isRanged_;

        temp = data.Data("RunSoundTimerMax");
        runSoundTimerMax_ = temp? temp->AsF32() : runSoundTimerMax_;

        temp = data.Data("MaxSteerTime");
        maxSteerTime_ = temp? temp->AsF32() : maxSteerTime_;

        data.Data("EffectsOnSummon", effectsOnSummon_, effectsOnSummon_);
	}

	void GCAIGeneric::Serialize( Simian::FileObjectSubNode& data)
    {
        GCAIBase::Serialize(data);
        data.AddData("SkillDist",skillDist_);
        data.AddData("AttackDist",attackDist_);
        data.AddData("AttackTimeTotal",attackTimeTotal_);
        data.AddData("AttackHitDelay",attackHitDelay_);
        data.AddData("AttackAnimationsTotal",attackAnimationsTotal_);
        data.AddData("AttackAnimationPrefix",attackAnimationPrefix_);
        data.AddData("ShootAnimationPrefix",shootAnimationPrefix_);
        data.AddData("BulletStartHeight",bulletStartHeight_);
        data.AddData("AnimationFrameRate",animationFrameRate_);
        data.AddData("SeekDist",seekDist_);
        data.AddData("LostDist",lostDist_);
        data.AddData("IsRanged",isRanged_);
        data.AddData("RunSoundTimerMax",runSoundTimerMax_);
        data.AddData("MaxSteerTime",maxSteerTime_);
        data.AddData("BulletType",bulletType_);
        data.AddData("EffectsOnSummon", effectsOnSummon_);
	}

	void GCAIGeneric::OnAwake()
	{
		GCAIBase::OnAwake();

        fsm_[ES_BIRTH].OnEnter = Simian::Delegate::CreateDelegate<GCAIGeneric, &GCAIGeneric::birthOnEnter_>(this);
		fsm_[ES_BIRTH].OnUpdate = Simian::Delegate::CreateDelegate<GCAIGeneric, &GCAIGeneric::birthOnUpdate_>(this);
		fsm_[ES_IDLE].OnEnter = Simian::Delegate::CreateDelegate<GCAIGeneric, &GCAIGeneric::idleOnEnter_>(this);
		fsm_[ES_IDLE].OnUpdate = Simian::Delegate::CreateDelegate<GCAIGeneric, &GCAIGeneric::idleOnUpdate_>(this);
		fsm_[ES_SEEKPLAYER].OnEnter = Simian::Delegate::CreateDelegate<GCAIGeneric, &GCAIGeneric::seekPlayerOnEnter_>(this);
		fsm_[ES_SEEKPLAYER].OnUpdate = Simian::Delegate::CreateDelegate<GCAIGeneric, &GCAIGeneric::seekPlayerOnUpdate_>(this);
		fsm_[ES_SEEKPLAYER_IDLE].OnEnter = Simian::Delegate::CreateDelegate<GCAIGeneric, &GCAIGeneric::seekPlayerIdleOnEnter_>(this);
		fsm_[ES_SEEKPLAYER_IDLE].OnUpdate = Simian::Delegate::CreateDelegate<GCAIGeneric, &GCAIGeneric::seekPlayerIdleOnUpdate_>(this);
        fsm_[ES_ATTACK].OnEnter = Simian::Delegate::CreateDelegate<GCAIGeneric, &GCAIGeneric::attackOnEnter_>(this);
        fsm_[ES_ATTACK].OnUpdate = Simian::Delegate::CreateDelegate<GCAIGeneric, &GCAIGeneric::attackOnUpdate_>(this);
		fsm_[ES_ATTACK_DEFAULT].OnEnter = Simian::Delegate::CreateDelegate<GCAIGeneric, &GCAIGeneric::attackDefaultOnEnter_>(this);
		fsm_[ES_ATTACK_DEFAULT].OnUpdate = Simian::Delegate::CreateDelegate<GCAIGeneric, &GCAIGeneric::attackDefaultOnUpdate_>(this);
		fsm_[ES_DEATH].OnEnter = Simian::Delegate::CreateDelegate<GCAIGeneric, &GCAIGeneric::deathOnEnter_>(this);
		fsm_[ES_DEATH].OnUpdate = Simian::Delegate::CreateDelegate<GCAIGeneric, &GCAIGeneric::deathOnUpdate_>(this);
        overrideFsm_[OS_SEEKIDLE].OnEnter = Simian::Delegate::CreateDelegate<GCAIGeneric, &GCAIGeneric::overideSeekIdleOnEnter_>(this);
        overrideFsm_[OS_SEEKIDLE].OnUpdate = Simian::Delegate::CreateDelegate<GCAIGeneric, &GCAIGeneric::overideSeekIdleOnUpdate_>(this);
        overrideFsm_[OS_SEEK].OnEnter = Simian::Delegate::CreateDelegate<GCAIGeneric, &GCAIGeneric::overideSeekOnEnter_>(this);
        overrideFsm_[OS_SEEK].OnUpdate = Simian::Delegate::CreateDelegate<GCAIGeneric, &GCAIGeneric::overideSeekOnUpdate_>(this);

        // register attribute callbacks
        attributes_->DealDamageDelegates() += 
            Simian::Delegate::CreateDelegate<GCAIGeneric, &GCAIGeneric::takeDamage_>(this);

#ifdef _DESCENDED
		if (GCEditor::EditorMode())
        {
            model_->Controller().PlayAnimation("Birth", true);
			return;
        }
#endif

        model_->Controller().PlayAnimation("Birth", false);
	}

	void GCAIGeneric::OnInit()
	{
#ifdef _DESCENDED
		if (GCEditor::EditorMode())
			return;
#endif
        
        GCAIBase::OnInit();
        playerLogic_ = GCPlayerLogic::Instance();
        if (playerLogic_)
        {
		    playerLogic_->ParentEntity()->ParentEntity()->ComponentByType(GC_PHYSICSCONTROLLER,playerPhysics_);
		    playerLogic_->ParentEntity()->ParentEntity()->ComponentByType(Simian::C_TRANSFORM,playerTransform_);        
        }

        ComponentByType(Simian::C_SOUNDEMITTER, sounds_);

		if(playerPhysics_)
		{
			Simian::f32 combinedRadius = (playerPhysics_->Radius()) + (physics_->Radius());
			attackRadiusSqr_ = attackDist_ + combinedRadius;
			// DEBUG
			attackRad_ = attackRadiusSqr_;
			attackRadiusSqr_ *= attackRadiusSqr_;

			seekRadiusSqr_ = seekDist_ + combinedRadius;
			// DEBUG
			seekRad_ = seekRadiusSqr_;
			seekRadiusSqr_ *= seekRadiusSqr_;

			lostRadiusSqr_ = lostDist_ + combinedRadius;
			// DEBUG
			lostRad_ = lostRadiusSqr_;
			lostRadiusSqr_*= lostRadiusSqr_;

            skillRadiusSqr_ = skillDist_ + combinedRadius;
            // DEBUG
            skillRad_ = skillRadiusSqr_;
            skillRadiusSqr_*= skillRadiusSqr_;
		}

		tMap_ = GCTileMap::TileMap(ParentScene());

		// for weapons and other attachments
		if (!attachments_.empty())
			for (Simian::u32 i = 0; i < attachments_.size(); ++i)
				model_->AttachEntityToJoint(attachments_[i].first, ParentEntity()->ChildByName(attachments_[i].second));

        playerAtt_ = GCPlayerLogic::Instance()->Attributes();

        if (effectsOnSummon_)
            ParentEntity()->ChildByName("SpawnParticles")->ComponentByType(Simian::C_PARTICLESYSTEM,spawnParticles_);
	}

	void GCAIGeneric::OnDeinit()
	{
		GCAIBase::OnDeinit();
	}

	void GCAIGeneric::OnSharedLoad()
	{
		RegisterCallback(Simian::P_POSTPHYSICS, Simian::Delegate::CreateDelegate<GCAIGeneric, &GCAIGeneric::postPhysics_>(this)); 
		GCAIBase::OnSharedLoad();
	}

	//-Private------------------------------------------------------------------

    void GCAIGeneric::birthOnEnter_( Simian::DelegateParameter&)
	{
        attributes_->Direction(Simian::Vector3::Zero);
		model_->Controller().PlayAnimation("Birth", false);
        effectsOn_ = effectsOnBirth_;

		// Don't show shadow
		Simian::CModel* shadowModel = 0;
		ParentEntity()->ChildByName("Shadow")->ComponentByType(Simian::C_MODEL, shadowModel);
		if (shadowModel)
			shadowModel->Visible(false);

        // Play sound
        if (sounds_)
            sounds_->Play(S_BIRTH);

        // Spawn effects on birth
        if (effectsOnSummon_)
            spawnParticles_->ParticleSystem()->Enabled(true);
	}

	void GCAIGeneric::birthOnUpdate_( Simian::DelegateParameter& param)
	{
		Simian::f32 dt = param.As<Simian::EntityUpdateParameter>().Dt;
        
        attributes_->BirthTimer(attributes_->BirthTimer()-dt);

        if(attributes_->Health() <= 0)
        {
            fsm_.ChangeState(ES_DEATH);
            effectsOn_ = true;

            // Spawn effects on birth
            if (effectsOnSummon_)
                spawnParticles_->ParticleSystem()->Enabled(false);
        }
        else if (attributes_->BirthTimer() < 0.0f)
        {
            fsm_.ChangeState(ES_IDLE);
            effectsOn_ = true;

            // Spawn effects on birth
            if (effectsOnSummon_)
                spawnParticles_->ParticleSystem()->Enabled(false);
        }
		else if (!shadowShown_)
		{
			Simian::f32 birthTimeRatio = attributes_->BirthTimer()/attributes_->BirthTimeMax();
			if (birthTimeRatio < attributes_->BirthShadowTimeRatio())
			{
				// Show shadow
				Simian::CModel* shadowModel = 0;
				ParentEntity()->ChildByName("Shadow")->ComponentByType(Simian::C_MODEL, shadowModel);
				if (shadowModel)
					shadowModel->Visible(true);
				shadowShown_ = true;
			}
		}
	}

	void GCAIGeneric::idleOnEnter_( Simian::DelegateParameter&)
    {
		attributes_->MovementSpeed(0);
		model_->Controller().PlayAnimation("Idle");
	}

	void GCAIGeneric::idleOnUpdate_( Simian::DelegateParameter& param)
    {
        if(attributes_->Health() <= 0)
        {
            fsm_.ChangeState(ES_DEATH);
            return;
        }
        if (!seekInCine_ && (GCPlayerLogic::Instance()->PlayerControlOverride() || aiPassive_ || playerAtt_->Health() <= 0))
            return;

		if(attributes_->ForceSeek() || idleBufferTimer_ < Simian::Math::EPSILON)
        {
            updateVecToPlayer_();
            Simian::Vector3 pos(transform_->Position()*TileWallCollision::PHYSICS_SCALE);
            
            bool noWallInBetween = !TileWallCollision::CheckTileCollision(pos,vecToPlayer_*TileWallCollision::PHYSICS_SCALE,0.05f,&tMap_->TileMap(),false,!isRanged_);
            if( noWallInBetween && isRanged_ && vecToPlayer_.LengthSquared() < attackRadiusSqr_ )
                fsm_.ChangeState(ES_ATTACK_DEFAULT);
            else if( attributes_->ForceSeek() || (vecToPlayer_.LengthSquared() < seekRadiusSqr_ && 
                noWallInBetween))
            {
                GCAttributes* victim;
                GCPlayerLogic::Instance()->ComponentByType(GC_ATTRIBUTES,victim);

                if(victim)
                    Attributes()->Victim(victim);
                fsm_.ChangeState(ES_SEEKPLAYER);
            }
            else
                Attributes()->Victim(0);
            return;
        }

        Simian::f32 dt = param.As<Simian::EntityUpdateParameter>().Dt;
        idleBufferTimer_ -= dt;

		// DEBUG
		/*DebugRendererDrawCircle(transform_->World().Position(),SQRT_SEEK_RAD,
			Simian::Color(1.0f, 0.0f, 0.0f));*/
	}

	void GCAIGeneric::seekPlayerOnEnter_( Simian::DelegateParameter&)
    {
		prevDir_ = attributes_->Direction();
        attributes_->MovementSpeed(Attributes()->MovementSpeedMax());
		model_->Controller().PlayAnimation("Run", true, animationFrameRate_);
		updateNextTile_(playerTransform_->World().Position());

        runSoundTimer_ = runSoundTimerMax_;

        // Play random sound
        if (sounds_)
            sounds_->Play(Simian::Math::Random(S_ALERT_1, S_ALERT_1 + TOTAL_ALERT_SOUNDS));
	}

	void GCAIGeneric::seekPlayerOnUpdate_( Simian::DelegateParameter& param)
	{
        Simian::f32 dt = param.As<Simian::EntityUpdateParameter>().Dt;
        
        if(attributes_->Health() <= 0)
        {
            fsm_.ChangeState(ES_DEATH);
            return;
        }
        else if (!seekInCine_ && (GCPlayerLogic::Instance()->PlayerControlOverride() || aiPassive_))
        {
            fsm_.ChangeState(ES_IDLE);
            return;
        }

        if(attributes_->ForceSeek() || seekBufferTimer_ < Simian::Math::EPSILON)
        {
		    attributes_->MovementSpeed(attributes_->MovementSpeedMax());
		    Simian::Vector3 pos(transform_->Position()*TileWallCollision::PHYSICS_SCALE);
		    updateVecToPlayer_();

		    if(seekIdle_)
		    {
			    fsm_.ChangeState(ES_SEEKPLAYER_IDLE);
			    return;
		    }
		
		    if (playerAtt_->Health() > 0 && !TileWallCollision::CheckTileCollision(pos,vecToPlayer_*TileWallCollision::PHYSICS_SCALE,0.05f,&tMap_->TileMap(),false,!isRanged_) && 
			    (vecToPlayer_.LengthSquared() < skillRadiusSqr_))
		    {
			    fsm_.CurrentState(ES_ATTACK);
			    return;
		    }

		    if(playerAtt_->Health() <= 0 && !attributes_->ForceSeek() && vecToPlayer_.LengthSquared() > lostRadiusSqr_)
		    {
			    fsm_.ChangeState(ES_IDLE);
			    return;
		    }
        }
        else
            seekBufferTimer_ -= dt;
        if(reachedNextTile_(dt))
            updateNextTile_(playerTransform_->World().Position());
        if (!seekIdle_)
            steer_(dt);

        // Play sound
        runSoundTimer_ += dt;
        if (runSoundTimer_ > runSoundTimerMax_)
        {
            runSoundTimer_ = 0.0f;
            if(sounds_)
                sounds_->Play(S_RUN);
        }

	   /* DebugRendererDrawCircle(transform_->World().Position(),attackRad_,
			Simian::Color(0.0f, 1.0f, 0.0f));
		DebugRendererDrawCircle(transform_->World().Position(),lostRad_,
			Simian::Color(0.0f, 0.0f, 1.0f));*/
	}

	void GCAIGeneric::seekPlayerIdleOnEnter_( Simian::DelegateParameter&  )
	{
		facePlayer_();
		updateVecToPlayer_();
		model_->Controller().PlayAnimation("Idle");
		// seekIdleDist_ = vecToPlayer_.LengthSquared();
	}

	void GCAIGeneric::seekPlayerIdleOnUpdate_( Simian::DelegateParameter& )
	{
        attributes_->MovementSpeed(0);
        if(!seekInCine_ && (GCPlayerLogic::Instance()->PlayerControlOverride() || aiPassive_))
        {
            fsm_.ChangeState(ES_IDLE);
            return;
        }
		if(!seekIdle_)
		{
			fsm_.ChangeState(ES_SEEKPLAYER);
            model_->Controller().PlayAnimation("Run");
			attributes_->MovementSpeed(attributes_->MovementSpeedMax());
			seekIdle_ = false;
			return;
		}
		Simian::Vector3 pos(transform_->Position()*TileWallCollision::PHYSICS_SCALE);
		if(playerAtt_->Health() > 0 && !TileWallCollision::CheckTileCollision(pos,vecToPlayer_*TileWallCollision::PHYSICS_SCALE,0.05f,&tMap_->TileMap(),false) && 
			(vecToPlayer_.LengthSquared() < skillRadiusSqr_)&&(!physics_->Collided()))
		{
			fsm_.CurrentState(ES_ATTACK);
			attributes_->MovementSpeed(attributes_->MovementSpeedMax());
			seekIdle_ = false;
			return;
		}
		updateVecToPlayer_();
		if(playerAtt_->Health() <= 0 || !attributes_->ForceSeek() && vecToPlayer_.LengthSquared() > lostRadiusSqr_)
		{
			fsm_.ChangeState(ES_IDLE);
			seekIdle_ = false;
			return;
		}

		//if(vecToPlayer_.LengthSquared() > seekIdleDist_)
		updateNextTile_(playerTransform_->World().Position());
	}

    void GCAIGeneric::attackOnEnter_( Simian::DelegateParameter&  )
    {
        Attributes()->MovementSpeed(0);
        model_->Controller().PlayAnimation("Idle");
        if(attributes_->ForceSeek())
            attributes_->ForceSeek(false);
    }

    void GCAIGeneric::attackOnUpdate_( Simian::DelegateParameter& param )
    {
        if(!seekInCine_ && (GCPlayerLogic::Instance()->PlayerControlOverride() || (playerAtt_->Health() <= 0) || aiPassive_))
        {
            fsm_.ChangeState(ES_IDLE);
            return;
        }
        Simian::f32 dt = param.As<Simian::EntityUpdateParameter>().Dt;
        if(!currentSkill_ || !currentSkill_->Update(dt))
        {
            Attributes()->MovementSpeed(0);
            chanceValue_ = Simian::Math::Random(0,100);
            bool chanceHit = false;
            for(Simian::u32 i = 0; i < skillList_.size(); ++i)
            {
                currentSkill_ = reinterpret_cast<AISkillBase*>(skillList_[i]);
                if(chanceValue_ < currentSkill_->Chance() && currentSkill_->Update(dt))
                {
                    chanceHit = true;
                    return;
                }
                chanceValue_ -= currentSkill_->Chance();
            }

            updateVecToPlayer_();
            if(!chanceHit && vecToPlayer_.LengthSquared() < attackRadiusSqr_)//|| (chanceHit && !currentSkill_->Update(dt)))
                fsm_.ChangeState(ES_ATTACK_DEFAULT);
            else
            {
                fsm_.ChangeState(ES_SEEKPLAYER);

                model_->Controller().PlayAnimation("Run");
                Attributes()->MovementSpeed(Attributes()->MovementSpeedMax());
                seekBufferTimer_ = SEEK_BUFFER_TIME;
            }
            
            currentSkill_ = NULL;
        }
    }
	void GCAIGeneric::attackDefaultOnEnter_( Simian::DelegateParameter& param)
	{
		Simian::f32 dt = param.As<Simian::EntityUpdateParameter>().Dt;
		helperStartAttack_(dt);
		Attributes()->MovementSpeed(0);
        facePlayer_();
	}

	void GCAIGeneric::attackDefaultOnUpdate_( Simian::DelegateParameter& param)
	{
        if(!seekInCine_ && (GCPlayerLogic::Instance()->PlayerControlOverride() ||(playerAtt_->Health() <= 0) || aiPassive_))
        {
            fsm_.ChangeState(ES_IDLE);
            return;
        }
		attributes_->MovementSpeed(0);
		Simian::Vector3 pos(transform_->Position()*TileWallCollision::PHYSICS_SCALE);

		if(attributes_->Health() <= 0)
			fsm_.ChangeState(ES_DEATH);
		else if (GCPlayerLogic::Instance()->Attributes()->Health() <= 0)
			fsm_.ChangeState(ES_IDLE);

		Simian::f32 dt = param.As<Simian::EntityUpdateParameter>().Dt;        
		attackTimer_ += dt;

		// Attack detection
		if (attackTimer_ > attackHitDelay_ && !hasAttacked_)
		{
			hasAttacked_ = true;
			if (!isRanged_)
 				meleeAttackDetection_();
            else
                createBullet_(vectorToDegreeFloat_(vecToPlayer_));

            model_->Controller().PlayAnimation(currentAttackAnimation_ + " Down", false);
            //facePlayer_();

            // Play sound
            if (sounds_)
                sounds_->Play(S_ATTACK);
		}

		if(attackTimer_ > attackTimeTotal_)
		{
			// Check if ready to change state
			if( attributes_->ForceSeek() || (vecToPlayer_.LengthSquared() > attackRadiusSqr_) || (TileWallCollision::CheckTileCollision(pos,
				vecToPlayer_*TileWallCollision::PHYSICS_SCALE,0.05f,&tMap_->TileMap(),false,!isRanged_)))
			{
				Attributes()->MovementSpeed(Attributes()->MovementSpeedMax());
				fsm_.ChangeState(ES_SEEKPLAYER);
                model_->Controller().PlayAnimation("Run");
				return;
			}
			
			helperStartAttack_(dt);
		}

		/*DebugRendererDrawCircle(transform_->World().Position(),attackRad_,
			Simian::Color(0.0f, 1.0f, 0.0f));*/
	}

	void GCAIGeneric::helperStartAttack_( Simian::f32 dt )
	{
        if (!seekInCine_ && (GCPlayerLogic::Instance()->PlayerControlOverride()))
            return;

		// Play Animation
		// Randomize attack animation
		if(reachedNextTile_(dt))
			updateNextTile_(playerTransform_->World().Position());

		attackTimer_ = 0.0f;
		Attributes()->MovementSpeed(0.0f);
		hasAttacked_ = false;

		if (!isRanged_)
        {
            std::stringstream tempSstream;
            tempSstream << attackAnimationPrefix_ << " " << 
                Simian::Math::Random(0, attackAnimationsTotal_) + 1;
            currentAttackAnimation_ = tempSstream.str();
        }
		else
        {
            std::stringstream tempSstream;
            tempSstream << shootAnimationPrefix_;
            currentAttackAnimation_ = tempSstream.str();
        }

        facePlayer_();
        
        model_->Controller().PlayAnimation(currentAttackAnimation_ + " Raise", false);
	}

	void GCAIGeneric::deathOnEnter_( Simian::DelegateParameter&  )
	{
		model_->Controller().PlayAnimation("Die", false, 48);
		Attributes()->Direction(Simian::Vector3::Zero);
		physics_->Enabled(false);
		GCPlayerLogic::Instance()->AddExperience(attributes_->ExperienceAwarded());

        // Play random sound
        if (sounds_)
            sounds_->Play(Simian::Math::Random(S_DIE_1, S_DIE_1 + TOTAL_DIE_SOUNDS));
	}

	void GCAIGeneric::deathOnUpdate_( Simian::DelegateParameter&  )
	{
	}

	void GCAIGeneric::updateVecToPlayer_()
	{
		vecToPlayer_ = playerTransform_->World().Position() - transform_->World().Position();
	}

	void GCAIGeneric::updateNextTile_( const Simian::Vector3& target )
	{
        nextTile_ = PathFinder::Instance().GeneratePath(physics_->Radius(), lostRadiusSqr_, 
            transform_->World().Position(),target);
		Simian::Vector3 dir = nextTile_ - transform_->Position();

        // Prevents from walking along walked path
		/*if(nextTile_.X() > 0 && nextTile_.Z() > 0 && dir.Dot(prevDir_) < 0)
		{
			Simian::Vector2 subtile(transform_->Position().X() * (-prevDir_.X()),
				transform_->Position().Z() * (-prevDir_.Z()));
			subtile = PathFinder::Instance().WorldToSubtile(subtile);

			if( PathFinder::Instance().IsSubTileValid( static_cast<Simian::u32>(std::abs(subtile.X())),
													   static_cast<Simian::u32>(std::abs(subtile.Y()))))
				dir = -prevDir_;
			else
				seekIdle_ = true;
		}*/

        Simian::f32 dist = dir.LengthSquared();
		if(dist < Simian::Math::EPSILON)
		{
			finalDir_ = dir;
			seekIdle_ = true;
		}
		else
		{
			finalDir_ = dir.Normalized();
			seekIdle_ = false;
		}
		steerTimer_ = 0.0f;
		startDir_ = attributes_->Direction();
		prevDir_ = finalDir_;
	}

	bool GCAIGeneric::reachedNextTile_( Simian::f32 dt )
	{
		Simian::f32 futureDist = attributes_->MovementSpeed() * dt;
		futureDist *= futureDist;
		Simian::Vector3 currentDist(nextTile_ - transform_->World().Position());
		Simian::f32 dist = currentDist.LengthSquared();
        return ((dist < Simian::Math::EPSILON) || (dist > futureDist));
	}

	void GCAIGeneric::steer_( Simian::f32 dt )
	{
		steerTimer_ += dt;

		if(steerTimer_<= maxSteerTime_)
		{
			Simian::f32 intp = steerTimer_/maxSteerTime_;
			intp = Simian::Math::Clamp(intp, 0.0f, 1.0f);
			Attributes()->Direction(Simian::Interpolation::Interpolate
				<Simian::Vector3>(startDir_,finalDir_,intp,Simian::Interpolation::EaseSquareOut));
		}

		Simian::f32 angle;
		if(attributes_->Direction().LengthSquared() > Simian::Math::EPSILON)
		{    
			angle = vectorToDegreeFloat_(attributes_->Direction());
			transform_->Rotation(Simian::Vector3(0, 1, 0), Simian::Degree(angle + 90.0f));
		}
	}

	void GCAIGeneric::facePlayer_()
	{
		updateVecToPlayer_();
        startDir_ = vecToPlayer_.Normalized();
		Simian::f32 angle = vectorToDegreeFloat_(startDir_);
		transform_->Rotation(Simian::Vector3(0, 1, 0), Simian::Degree(angle + 90.0f));
        attributes_->Direction(startDir_);
	}

	Simian::f32 GCAIGeneric::vectorToDegreeFloat_(const Simian::Vector3& vec)
	{
		// Get angle of rotation in degrees for vector
		//float x = Simian::Math::Clamp(vec.X(), -1.0f, 1.0f);
		if(vec.LengthSquared() > Simian::Math::EPSILON)
		{
			Simian::Vector3 tempVec = vec.Normalized();
			Simian::f32 angle = std::acosf(tempVec.X());
			if (tempVec.Z() > 0)
				angle = Simian::Math::TAU - angle;
			// Wrap angle within 360
			angle = Simian::Math::WrapAngle(Simian::Radian(angle)).Degrees();

			return angle;
		}
		return 0;
	}

	// Attack helper functions
	void GCAIGeneric::meleeAttackDetection_()
	{
		Simian::f32 attackAngle = ((attributes_->AttackAngle()/2.0f) * Simian::Math::PI) / 180.0f;
		updateVecToPlayer_();
		// Get distance vector to player
		Simian::f32 eToPLength = vecToPlayer_.Length();

		// Check if distance to enemy is within attack range
		if (eToPLength*eToPLength <= attackRadiusSqr_)
		{
			// Check if facing enemy
			Simian::f32 dotProd = vecToPlayer_.Dot(
				Simian::Vector3(std::cos(transform_->Angle().Radians()-Simian::Math::PI/2), 0,
				std::sin(transform_->Angle().Radians()+Simian::Math::PI/2)));

			if ((dotProd/eToPLength) > attackAngle)
			{
				CombatCommandSystem::Instance().AddCommand(attributes_, GCPlayerLogic::Instance()->Attributes(),
					CombatCommandDealDamage::Create(attributes_->Damage()));
                // Play sound
                if (sounds_)
                    playerLogic_->Sounds()->Play(playerLogic_->S_MELEE_GET_HIT);
			}
		}
	}

	void GCAIGeneric::createBullet_(Simian::f32 angle)
	{
		// Fire bullet
		Simian::Entity* bullet = 0;

        if (bulletType_ == BT_BASIC)
            bullet = ParentEntity()->ParentScene()->CreateEntity(E_ENEMYBULLET);
        else if (bulletType_ == BT_ABYSSAL_REVENANT)
            bullet = ParentEntity()->ParentScene()->CreateEntity(E_ELECTROBULLET_ABYSSAL);
		ParentEntity()->ParentScene()->AddEntity(bullet);
		// Pass bullet attribs
		GCEnemyBulletLogic* enemyBullet_;
		bullet->ComponentByType(GC_ENEMYBULLETLOGIC, enemyBullet_);
		enemyBullet_->OwnerAttribs(attributes_);
		// Set bullet transform
		Simian::CTransform* bulletTransform_;
		bullet->ComponentByType(Simian::C_TRANSFORM, bulletTransform_);
		bulletTransform_->Position(Simian::Vector3(transform_->World().Position().X(),
			transform_->World().Position().Y() + bulletStartHeight_, transform_->World().Position().Z()));
        // Set its velocity
        enemyBullet_->MovementVec(Simian::Vector3(std::cos(Simian::Degree(Simian::Math::TAU - angle - 5.0f).Radians()),
                0, std::sin(Simian::Degree(Simian::Math::TAU - angle - 5.0f).Radians())));
        
        // Play sound
        if (sounds_)
            sounds_->Play(S_SHOOT);
	}

	void GCAIGeneric::postPhysics_( Simian::DelegateParameter&)
	{
#ifdef _DESCENDED
		if (GCEditor::EditorMode())
			return;
#endif
		if(fsm_.CurrentState() == ES_DEATH)
			PathFinder::Instance().UnsetPosition(oldMin_,oldMax_);
		else
			PathFinder::Instance().SetNewPosition(transform_->World().Position(),physics_->Radius(),oldMin_,oldMax_);
	}

    void GCAIGeneric::overideSeekIdleOnEnter_( Simian::DelegateParameter&  )
    {
        model_->Controller().PlayAnimation("Idle");
        attributes_->MovementSpeed(0);
    }

    void GCAIGeneric::overideSeekIdleOnUpdate_( Simian::DelegateParameter&  )
    {
        updateNextTile_(target_);
        if(!seekIdle_)
            overrideFsm_.ChangeState(OS_SEEK);
    }

    void GCAIGeneric::overideSeekOnEnter_( Simian::DelegateParameter&  )
    {
        model_->Controller().PlayAnimation("Run");
        attributes_->MovementSpeed(attributes_->MovementSpeedMax());

        runSoundTimer_ = runSoundTimerMax_;

        // Play random sound
        if (sounds_)
            sounds_->Play(Simian::Math::Random(S_ALERT_1, S_ALERT_1 + TOTAL_ALERT_SOUNDS));
    }

    void GCAIGeneric::overideSeekOnUpdate_( Simian::DelegateParameter& param )
    {
        if(seekIdle_)
        {
            overrideFsm_.ChangeState(OS_SEEKIDLE);
            return;
        }
        float dt = param.As<Simian::EntityUpdateParameter>().Dt;

        if(reachedNextTile_(dt))
            updateNextTile_(target_);
        steer_(dt);

        // Play sound
        runSoundTimer_ += dt;
        if (runSoundTimer_ > runSoundTimerMax_)
        {
            runSoundTimer_ = 0.0f;
            if(sounds_)
                sounds_->Play(S_RUN);
        }
    }
	void GCAIGeneric::overRideStateUpdate_( Simian::f32 dt )
	{
        overrideFsm_.Update(dt);
	}

    void GCAIGeneric::takeDamage_( Simian::DelegateParameter& param )
    {
        GCAttributes::DealDamageParameter& p = param.As<GCAttributes::DealDamageParameter>();
        if(p.Damage > attributes_->Health())
            fsm_.ChangeState(ES_DEATH);
        else if (fsm_.CurrentState() == ES_IDLE)
            attributes_->ForceSeek(true);

        // Play random sound
        if (sounds_)
            sounds_->Play(Simian::Math::Random(S_MELEE_GET_HIT_1, S_MELEE_GET_HIT_1 + TOTAL_MELEE_GET_HIT_SOUNDS));
    }

}
