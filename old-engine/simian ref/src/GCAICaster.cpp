/******************************************************************************/
/*!
\file		GCAICaster.cpp
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

#include "EntityTypes.h"
#include "ComponentTypes.h"
#include "Collision.h"
#include "EntityCollision.h"
#include "TileWallCollision.h"
#include "CombatCommandSystem.h"
#include "CombatCommandDealDamage.h"
#include "PathFinder.h"
#include "GCTileMap.h"
#include "GCPhysicsController.h"
#include "GCPlayerLogic.h"
#include "GCAICaster.h"
#include "GCAttributes.h"
#include "GCEditor.h"

#include <iostream>
#include "Simian/LogManager.h"

namespace Descension
{
	Simian::FactoryPlant<Simian::EntityComponent, GCAICaster> factoryPlant_(
		&Simian::GameFactory::Instance().ComponentFactory(), GC_AICASTER);

    // Sounds
    static const Simian::u32 TOTAL_ALERT_SOUNDS = 3;
    static const Simian::u32 TOTAL_DIE_SOUNDS = 2;
    static const Simian::u32 TOTAL_MELEE_GET_HIT_SOUNDS = 2;

	GCAICaster::GCAICaster()
		:   GCAIBase(ES_BIRTH, ES_TOTAL),
		    tMap_(0),
		    playerAttr_(0),
		    playerTransform_(0),
		    playerPhysics_(0),
            sounds_(0),
		    hasAttacked_(false),
			shadowShown_(false),
		    attackAnimationsTotal_(1),
            animationFrameRate_(48),
            chanceValue_(0),
		    attackTimer_(0),
		    steerTimer_(0),
            idleTimer_(0),
            idleTime_(0),
            attackRadiusSqr_(0),
            attackTimeTotal_(1.5f),
            attackHitDelay_(1.0f),
            bulletStartHeight_(0),
            attackDist_(0),
		    oldMin_(-1.f,-1.f),
		    oldMax_(-1.f,-1.f),
            vecToPlayer_(Simian::Vector3::Zero),
            nextTile_(Simian::Vector3::Zero),
		    prevDir_(Simian::Vector3::Zero),
            startDir_(Simian::Vector3::Zero),
            finalDir_(Simian::Vector3::Zero)
	{ 
	}

	//-Public-------------------------------------------------------------------

	bool GCAICaster::HasAttacked() const
	{
		return hasAttacked_;
	}

    Simian::CSoundEmitter* GCAICaster::Sounds() const
    {
        return sounds_;
    }

    //-Public-------------------------------------------------------------------

    void GCAICaster::Deserialize( const Simian::FileObjectSubNode& data )
    {
        GCAIBase::Deserialize(data);

        ///////////////
        // Variables //
        ///////////////

        const Simian::FileDataSubNode* temp = data.Data("AttackDist");
        attackDist_ = temp? temp->AsF32() : attackDist_;

        temp = data.Data("AttackTimeTotal");
        attackTimeTotal_ = temp? temp->AsF32() : attackTimeTotal_;

        temp = data.Data("AttackHitDelay");
        attackHitDelay_ = temp? temp->AsF32() : attackHitDelay_;

        temp = data.Data("AttackAnimationsTotal");
        attackAnimationsTotal_ = temp? temp->AsU32() : attackAnimationsTotal_;

        temp = data.Data("BulletStartHeight");
        bulletStartHeight_ = temp? temp->AsF32() : bulletStartHeight_;

        temp = data.Data("AnimationFrameRate");
        animationFrameRate_ = temp? temp->AsU32() : animationFrameRate_;

        temp = data.Data("IdleTime");
        idleTime_ = temp? temp->AsF32() : idleTime_;
    }

    void GCAICaster::Serialize( Simian::FileObjectSubNode& data )
    {
        GCAIBase::Serialize(data);
        data.AddData("AttackDist",attackTimeTotal_);
        data.AddData("AttackTimeTotal",attackTimeTotal_);
        data.AddData("AttackHitDelay",attackHitDelay_);
        data.AddData("AttackAnimationsTotal",attackAnimationsTotal_);
        data.AddData("BulletStartHeight",bulletStartHeight_);
        data.AddData("AnimationFrameRate",animationFrameRate_);
        data.AddData("IdleTime",idleTime_);
    }

    void GCAICaster::OnAwake()
    {
        GCAIBase::OnAwake();
        fsm_[ES_BIRTH].OnEnter = Simian::Delegate::CreateDelegate<GCAICaster, &GCAICaster::birthOnEnter_>(this);
        fsm_[ES_BIRTH].OnUpdate = Simian::Delegate::CreateDelegate<GCAICaster, &GCAICaster::birthOnUpdate_>(this);
        fsm_[ES_IDLE].OnEnter = Simian::Delegate::CreateDelegate<GCAICaster, &GCAICaster::idleOnEnter_>(this);
        fsm_[ES_IDLE].OnUpdate = Simian::Delegate::CreateDelegate<GCAICaster, &GCAICaster::idleOnUpdate_>(this);
        fsm_[ES_ATTACK_SKILL].OnEnter = Simian::Delegate::CreateDelegate<GCAICaster, &GCAICaster::attackSkillOnEnter_>(this);
        fsm_[ES_ATTACK_SKILL].OnUpdate = Simian::Delegate::CreateDelegate<GCAICaster, &GCAICaster::attackSkillOnUpdate_>(this);
        fsm_[ES_DEATH].OnEnter = Simian::Delegate::CreateDelegate<GCAICaster, &GCAICaster::deathOnEnter_>(this);
        fsm_[ES_DEATH].OnUpdate = Simian::Delegate::CreateDelegate<GCAICaster, &GCAICaster::deathOnUpdate_>(this);

#ifdef _DESCENDED
		if (GCEditor::EditorMode())
        {
            model_->Controller().PlayAnimation("Birth", true);
			return;
        }
#endif

        model_->Controller().PlayAnimation("Birth", false);

        // register attribute callbacks
        attributes_->DealDamageDelegates() += 
            Simian::Delegate::CreateDelegate<GCAICaster, &GCAICaster::takeDamage_>(this);
    }

    void GCAICaster::OnInit()
    {
#ifdef _DESCENDED
        if (GCEditor::EditorMode())
            return;
#endif

        GCAIBase::OnInit();

        playerAttr_ = GCPlayerLogic::Instance()->Attributes();
        GCPlayerLogic::Instance()->ParentEntity()->ParentEntity()->ComponentByType(GC_PHYSICSCONTROLLER,playerPhysics_);
        GCPlayerLogic::Instance()->ParentEntity()->ParentEntity()->ComponentByType(Simian::C_TRANSFORM,playerTransform_);

		ComponentByType(Simian::C_SOUNDEMITTER, sounds_);

        if(playerPhysics_)
        {
            Simian::f32 combinedRadius = (playerPhysics_->Radius()) + (physics_->Radius());
            attackRadiusSqr_ = attackDist_ + combinedRadius;
            // DEBUG
            attackRad_ = attackRadiusSqr_;
            attackRadiusSqr_ *= attackRadiusSqr_;
        }

        tMap_ = GCTileMap::TileMap(ParentScene());
    }

    void GCAICaster::OnDeinit()
    {
        GCAIBase::OnDeinit();
    }

    void GCAICaster::OnSharedLoad()
    {
        RegisterCallback(Simian::P_POSTPHYSICS, Simian::Delegate::CreateDelegate<GCAICaster, &GCAICaster::postPhysics_>(this)); 
        GCAIBase::OnSharedLoad();
    }

	//-Private------------------------------------------------------------------

    void GCAICaster::birthOnEnter_( Simian::DelegateParameter&)
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
	}

	void GCAICaster::birthOnUpdate_( Simian::DelegateParameter& param)
	{
		Simian::f32 dt = param.As<Simian::EntityUpdateParameter>().Dt;
        
        attributes_->BirthTimer(attributes_->BirthTimer()-dt);

        if(attributes_->Health() <= 0)
        {
            fsm_.ChangeState(ES_DEATH);
            effectsOn_ = true;
        }
        else if (attributes_->BirthTimer() < 0.0f)
        {
            fsm_.ChangeState(ES_IDLE);
            effectsOn_ = true;
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

	void GCAICaster::idleOnEnter_( Simian::DelegateParameter&)
	{
		//attributes_->Direction(Simian::Vector3::Zero);
        attributes_->MovementSpeed(0);
		//model_->Controller().PlayAnimation("Idle");
        idleTimer_ = 0.0f;
	}

	void GCAICaster::idleOnUpdate_( Simian::DelegateParameter& param)
	{
        idleTimer_ += param.As<Simian::EntityUpdateParameter>().Dt;

        if (GCPlayerLogic::Instance()->PlayerControlOverride() || aiPassive_ || playerAttr_->Health() <= 0)
            return;

        else
        {
            updateVecToPlayer_();
            Simian::Vector3 pos(transform_->Position()*TileWallCollision::PHYSICS_SCALE);
            if(attributes_->Health() <= 0)
            {
                fsm_.ChangeState(ES_DEATH);
                return;
            }

            if (idleTimer_ > idleTime_ && 
                !TileWallCollision::CheckTileCollision(pos,vecToPlayer_*TileWallCollision::PHYSICS_SCALE,0.05f,&tMap_->TileMap(),false,false) && 
                (vecToPlayer_.LengthSquared() < attackRadiusSqr_))
            {
                GCAttributes* victim;
                GCPlayerLogic::Instance()->ComponentByType(GC_ATTRIBUTES,victim);

                if(victim)
                    Attributes()->Victim(victim);
                fsm_.CurrentState(ES_ATTACK_SKILL);
                return;
            }
            else
                Attributes()->Victim(0);
            if (idleTimer_ > idleTime_)
                idleTimer_ = 0.0f;
        }
	}

	void GCAICaster::attackSkillOnEnter_( Simian::DelegateParameter& )
	{
		Attributes()->MovementSpeed(0);
        //model_->Controller().PlayAnimation("Cast Charge");
        
        // Play sound
        if (sounds_)
            sounds_->Play(S_ATTACK);
	}

	void GCAICaster::attackSkillOnUpdate_( Simian::DelegateParameter& param)
	{
        Simian::f32 dt = param.As<Simian::EntityUpdateParameter>().Dt;

        if(GCPlayerLogic::Instance()->PlayerControlOverride() ||aiPassive_ || playerAttr_->Health() <= 0.0f)
        {
            fsm_.ChangeState(ES_IDLE);
            return;
        }
        if(attributes_->Health() <= 0)
        {
            fsm_.ChangeState(ES_DEATH);
            return;
        }
        if(!currentSkill_ || !currentSkill_->Update(dt))
        {
            Attributes()->MovementSpeed(0);
            chanceValue_ = Simian::Math::Random(0,100);

            bool chanceHit = false;
            for(Simian::u32 i = 0; i < skillList_.size(); ++i)
            {
                currentSkill_ = reinterpret_cast<AISkillBase*>(skillList_[i]);
                if(chanceValue_ < currentSkill_->Chance())
                {
                    chanceHit = true;
                    break;
                }
                else
                    chanceValue_ -= currentSkill_->Chance();
            }
            
            if(!chanceHit || (chanceHit && !currentSkill_->Update(dt)))
                fsm_.ChangeState(ES_IDLE);
        }
	}

	void GCAICaster::deathOnEnter_( Simian::DelegateParameter&  )
	{
		model_->Controller().PlayAnimation("Die", false, 48);
		Attributes()->Direction(Simian::Vector3::Zero);
		physics_->Enabled(false);

        // Play random sound
        if (sounds_)
            sounds_->Play(Simian::Math::Random(S_DIE_1, S_DIE_1 + TOTAL_DIE_SOUNDS));
	}

	void GCAICaster::deathOnUpdate_( Simian::DelegateParameter&  )
	{
	}

	void GCAICaster::updateVecToPlayer_()
	{
		vecToPlayer_ = playerTransform_->World().Position() - transform_->World().Position();
	}

	void GCAICaster::facePlayer_()
	{
		updateVecToPlayer_();
		Simian::f32 angle = vectorToDegreeFloat_(vecToPlayer_.Normalized());
		transform_->Rotation(Simian::Vector3(0, 1, 0), Simian::Degree(angle + 90.0f));
	}

	Simian::f32 GCAICaster::vectorToDegreeFloat_(const Simian::Vector3& vec)
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

	void GCAICaster::createBullet_(Simian::f32 angle)
	{
		// Fire bullet
		Simian::Entity* bullet = ParentEntity()->ParentScene()->CreateEntity(E_ENEMYBULLET);
		ParentEntity()->ParentScene()->AddEntity(bullet);
		// Set bullet transform
		Simian::CTransform* bulletTransform_;
		bullet->ComponentByType(Simian::C_TRANSFORM, bulletTransform_);
		bulletTransform_->Position(Simian::Vector3(transform_->World().Position().X(),
			transform_->World().Position().Y() + bulletStartHeight_, transform_->World().Position().Z()));
		bulletTransform_->Rotation(Simian::Vector3(0, 1, 0), Simian::Degree(Simian::Math::TAU - angle - 5.0f));
	}

    void GCAICaster::takeDamage_( Simian::DelegateParameter& )
    {
        // Play random sound
        if (sounds_)
            sounds_->Play(Simian::Math::Random(S_MELEE_GET_HIT_1, S_MELEE_GET_HIT_1 + TOTAL_MELEE_GET_HIT_SOUNDS));
    }

	void GCAICaster::postPhysics_( Simian::DelegateParameter&)
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

    void GCAICaster::overRideStateUpdate_( Simian::f32  )
    {
    }
}
