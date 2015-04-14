/******************************************************************************/
/*!
\file		GCAIMajen.cpp
\author 	Bryan Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	2012/08/10
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/******************************************************************************/
#include "GCAIMajen.h"
#include "GCPhysicsController.h"
#include "GCPlayerLogic.h"
#include "GCEditor.h"
#include "GCMajenTriangle.h"
#include "GCFlamingSnakeEffect.h"
#include "GCEnemyBulletLogic.h"
#include "GCTrapFireSpot.h"
#include "GCUI.h"

#include "Collision.h"
#include "EntityTypes.h"
#include "ComponentTypes.h"
#include "Pathfinder.h"
#include "CombatCommandDealDamage.h"
#include "GCParticleChainDeactivator.h"
#include "GCParticleEffectDeactivator.h"
#include "GCDescensionCamera.h"
#include "GCEventScript.h"

#include "Simian/EnginePhases.h"
#include "Simian/CTransform.h"
#include "Simian/Matrix.h"
#include "Simian/CModel.h"
#include "Simian/AnimationController.h"
#include "Simian/Math.h"
#include "Simian/Interpolation.h"
#include "Simian/Scene.h"
#include "Simian/LogManager.h"
#include "Simian/DebugRenderer.h"
#include "Simian/CParticleSystem.h"
#include "Simian/ParticleSystem.h"
#include "Simian\CSoundEmitter.h"

namespace Descension
{
    static const Simian::f32 MAJEN_PATHFINDER_RANGE = FLT_MAX;
    static const Simian::f32 MAJEN_MAX_STEER_TIME = 0.18f;
    static const Simian::f32 MAJEN_SPIN_DAMAGE_INTERVAL = 0.15f;
    static const Simian::f32 MAJEN_BULLET_HEIGHT = 0.35f;
    static const Simian::f32 MAJEN_FLINCH_TIMER_MAX = 0.25f;

    static const Simian::u32 REMINDER_MAX_TIMES = 2;
    static const Simian::s32 REMINDER_MIN_HP = 50;

    static const std::string MAJEN_IDLE_ANIMATION = "Idle";
    static const std::string MAJEN_RUN_ANIMATION = "Run";
    static const std::string MAJEN_RAISE_ANIMATION = "Attack 1 Raise";
    static const std::string MAJEN_SWING_ANIMATION = "Attack 1 Down";
    static const std::string MAJEN_CAST_TRIANGLE = "Cast";
    static const std::string MAJEN_SPIN_START = "Spin Attack Raise";
    static const std::string MAJEN_SPIN = "Spin Attack Loop";
    static const std::string MAJEN_SPIN_END = "Spin Attack Down";
    static const std::string MAJEN_SLAM_START = "Slam Ground Raise";
    static const std::string MAJEN_SLAM = "Slam Ground Down";
    static const std::string MAJEN_CLONE = "Clone";
    static const std::string MAJEN_JUMPBACK = "Jump Back";
    static const std::string MAJEN_SHOOT = "Shoot";
    static const std::string MAJEN_DASH = "Dash Down";
    static const std::string MAJEN_STUN_START = "Cripple Start";
    static const std::string MAJEN_STUN = "Cripple Loop";
    static const std::string MAJEN_STUN_END = "Cripple End";
    static const std::string MAJEN_FLINCH_1 = "Flinch 1";
    static const std::string MAJEN_FLINCH_2 = "Flinch 2";
    static const std::string MAJEN_DIE = "Die";

    // Sounds
    static const Simian::f32 RUN_SOUND_TIMER_MAX = 0.94f;
    static const Simian::f32 SPIN_SOUND_TIMER_MAX = 2.76f;
    static const Simian::f32 LAUGH_SOUND_TIMER_MAX = 5.0f;
    static const Simian::u32 TOTAL_ATTACK_GRUNT_SOUNDS = 3;
    static const Simian::u32 TOTAL_HIT_GRUNT_SOUNDS = 3;
    static const Simian::u32 TOTAL_STUN_GRUNT_SOUNDS = 3;
    static const Simian::u32 TOTAL_SHOOT_SOUNDS = 3;
    static const Simian::u32 TOTAL_MELEE_GET_HIT_SOUNDS = 2;

    Simian::FactoryPlant<Simian::EntityComponent, GCAIMajen> GCAIMajen::factoryPlant_(
        &Simian::GameFactory::Instance().ComponentFactory(), GC_AIMAJEN);

    // C++ doesn't support string literals as template params.. :(
    // so lets do this C style! xD
#define CAST_ENTER(func, anim) \
    void GCAIMajen::func( Simian::DelegateParameter& )\
    {\
        timer_ = 0.0f;\
        model_->Controller().PlayAnimation(anim, false);\
        attributes_->MovementSpeed(0.0f);\
    }

    // cast enter functions
    CAST_ENTER(spinStartEnter_, MAJEN_SPIN_START)
    CAST_ENTER(spinEndEnter_, MAJEN_SPIN_END)
    CAST_ENTER(slamStartEnter_, MAJEN_SLAM_START)
    CAST_ENTER(stunStartEnter_, MAJEN_STUN_START)
    CAST_ENTER(stunEndEnter_, MAJEN_STUN_END)

#undef CAST_ENTER

    //--------------------------------------------------------------------------

    GCAIMajen::GCAIMajen()
        : GCAIBase(MS_CHASE, MS_TOTAL),
          fireSpinDeactivator_(0),
          fireSpinTransform_(0),
          chargeAuraDeactivator_(0),
          chargeAuraTransform_(0),
          reminderDodgeScript_(0),
          reminderDemystScript_(0),
          sounds_(0),
          runSoundTimer_(0),
          runSoundTimerMax_(RUN_SOUND_TIMER_MAX),
          spinSoundTimer_(0),
          spinSoundTimerMax_(SPIN_SOUND_TIMER_MAX),
          laughSoundTimer_(0),
          laughSoundTimerMax_(LAUGH_SOUND_TIMER_MAX),
          spinDamage_(0.0f),
          spinDamageRadius_(0.0f),
          spinMovementSpeed_(0.0f),
          slamCooldown_(0.0f),
          slamRadius_(0.0f),
          slamDamage_(0),
          slamSnakeDamage_(0),
          cloneCastTime_(0),
          cloneSpinChance_(0),
          cloneSpellDelay_(0),
          cloneHp_(0),
          cloneLifetime_(0),
          jumpBackSpeed_(0),
          jumpBackTime_(0),
          bulletInterval_(0),
          dashWaitTime_(0),
          dashSpeed_(0),
          dashDuration_(0),
          dashDamage_(0),
          arcWidth_(0),
          arcHeight_(0),
          numBullets_(0),
          triangleDamage_(20),
          shotCount_(0),
          dashHit_(false),
          stunHp_(0),
          steerTimer_(0.0f),
          idleDelay_(0.0f),
          attackDelay_(0.0f),
          swingDelay_(0.0f),
          timer_(0.0f),
          spellTimer_(0.0f),
          spinDamageTimer_(0.0f),
          flinchTimer_(-1.0f),
          reminderTimer_(0),
          reminderTimerMax_(40.0f),
          reminderPrev_(0),
          currentPhase_(0),
          trapsEachPhase_(4),
          clone_(false)
    {
    }

    //--------------------------------------------------------------------------

    void GCAIMajen::chaseEnter_( Simian::DelegateParameter& )
    {
        attributes_->MovementSpeed(attributes_->MovementSpeedMax());
        model_->Controller().PlayAnimation(MAJEN_RUN_ANIMATION);
        updateNextTile_();

        runSoundTimer_ = runSoundTimerMax_;
    }

    void GCAIMajen::chaseUpdate_( Simian::DelegateParameter& param )
    {
        Simian::FiniteStateUpdateParameter* p;
        param.As(p);

        if (reachedTile_(p->Dt))
            updateNextTile_();

        moveTowardsPlayer_(p->Dt);

        // If player is already dead
        if (GCPlayerLogic::Instance()->Attributes()->Health() <= 0 && fsm_.CurrentState() != MS_AFTERPLAYERDEATH)
        {
            fsm_.ChangeState(MS_AFTERPLAYERDEATH);
            return;
        }

        if (spellTimer_ <= 0.0f)
            fsm_.ChangeState(MS_CHOOSESKILL);

        Simian::Vector3 distVect = transform_->Position() - GCPlayerLogic::Instance()->Transform().World().Position();
        Simian::f32 attackRange = attributes_->AttackRange() + physics_->Radius() + GCPlayerLogic::Instance()->Physics()->Radius();
        if (distVect.LengthSquared() < attackRange * attackRange)
            fsm_.ChangeState(MS_IDLE);

        // Play sound
        runSoundTimer_ += p->Dt;
        if (runSoundTimer_ > runSoundTimerMax_)
        {
            runSoundTimer_ = 0.0f;
            if(sounds_)
                sounds_->Play(S_RUN);
        }

        // Play sound
        laughSoundTimer_ += p->Dt;
        if (laughSoundTimer_ > laughSoundTimerMax_)
        {
            laughSoundTimer_ = 0.0f;
            if(sounds_)
                sounds_->Play(S_LAUGH);
        }
    }

    void GCAIMajen::chaseExit_( Simian::DelegateParameter& )
    {
        timer_ = idleDelay_;
        attributes_->MovementSpeed(0.0f);
    }

    void GCAIMajen::idleEnter_( Simian::DelegateParameter& )
    {
        model_->Controller().PlayAnimation(MAJEN_IDLE_ANIMATION);
    }

    void GCAIMajen::idleUpdate_( Simian::DelegateParameter& param )
    {
        Simian::FiniteStateUpdateParameter* p;
        param.As(p);

        Simian::Vector3 facingVec = GCPlayerLogic::Instance()->Transform().World().Position() - 
            transform_->World().Position();
        facingVec.Normalize();
        Simian::f32 angle = std::atan2(-facingVec.Z(), facingVec.X());
        transform_->Rotation(Simian::Vector3::UnitY, Simian::Radian(angle + Simian::Math::PI * 0.5f));
        Attributes()->Direction(facingVec);

        timer_ += p->Dt;

        if (spellTimer_ <= 0.0f)
            fsm_.ChangeState(MS_CHOOSESKILL);

        if (timer_ > idleDelay_)
            fsm_.ChangeState(MS_MELEE);

        Simian::Vector3 distVect = transform_->Position() - GCPlayerLogic::Instance()->Transform().World().Position();
        Simian::f32 attackRange = attributes_->AttackRange() + physics_->Radius() + GCPlayerLogic::Instance()->Physics()->Radius();
        if (distVect.LengthSquared() > attackRange * attackRange)
            fsm_.ChangeState(MS_CHASE);
    }

    void GCAIMajen::meleeEnter_( Simian::DelegateParameter& )
    {
        timer_ = 0.0f;
        model_->Controller().PlayAnimation(MAJEN_RAISE_ANIMATION, false);
    }

    void GCAIMajen::meleeUpdate_( Simian::DelegateParameter& param)
    {
        Simian::FiniteStateUpdateParameter* p;
        param.As(p);

        // If player is already dead
        if (GCPlayerLogic::Instance()->Attributes()->Health() <= 0 && fsm_.CurrentState() != MS_AFTERPLAYERDEATH)
        {
            fsm_.ChangeState(MS_AFTERPLAYERDEATH);
            return;
        }

        timer_ += p->Dt;

        if (timer_ >= attackDelay_)
            fsm_.ChangeState(MS_SWING);
    }

    void GCAIMajen::swingEnter_( Simian::DelegateParameter& )
    {
        timer_ = 0.0f;
        model_->Controller().PlayAnimation(MAJEN_SWING_ANIMATION, false);

        // check if we should deal damage
        Simian::Vector3 pVec = GCPlayerLogic::Instance()->Transform().World().Position() - transform_->World().Position();
        Simian::f32 playerDist = pVec.Length();
        pVec /= playerDist;
        
        float angle = std::acos(pVec.Dot(Attributes()->Direction()));
        playerDist -= GCPlayerLogic::Instance()->Physics()->Radius() + physics_->Radius();
        if (Simian::Radian(angle).Degrees() < attributes_->AttackAngle() && playerDist < attributes_->AttackRange())
        {
            CombatCommandSystem::Instance().AddCommand(attributes_, GCPlayerLogic::Instance()->Attributes(), 
                CombatCommandDealDamage::Create(attributes_->Damage()));
            
            // Play sound
            if (sounds_)
                GCPlayerLogic::Instance()->Sounds()->Play(GCPlayerLogic::Instance()->S_MELEE_GET_HIT);
        }

        // Play attack sound
        if (sounds_)
            sounds_->Play(S_ATTACK);
        // Play random sounds
        if (sounds_)
            sounds_->Play(Simian::Math::Random(S_ATTACK_GRUNT1, S_ATTACK_GRUNT1 + TOTAL_ATTACK_GRUNT_SOUNDS));
    }

    void GCAIMajen::swingUpdate_( Simian::DelegateParameter& param )
    {
        Simian::FiniteStateUpdateParameter* p;
        param.As(p);

        timer_ += p->Dt;

        if (timer_ >= swingDelay_)
            fsm_.ChangeState(MS_IDLE);
    }

    void GCAIMajen::swingExit_( Simian::DelegateParameter& )
    {
        timer_ = 0.0f;
    }


    void GCAIMajen::chooseUpdate_( Simian::DelegateParameter& )
    {
        // If player is already dead
        if (GCPlayerLogic::Instance()->Attributes()->Health() <= 0 && fsm_.CurrentState() != MS_AFTERPLAYERDEATH)
        {
            fsm_.ChangeState(MS_AFTERPLAYERDEATH);
            return;
        }

        // find the spell timer to use
        int skill = -1;

        // must change a state.. this depends on what phase we are in.
        Simian::s32 number = rand()%100;

        if (clone_)
        {
            spellTimer_ = cloneSpellDelay_;
            if (number < cloneSpinChance_)
                fsm_.ChangeState(MS_SPIN);
            else
                fsm_.ChangeState(MS_CHASE);
            return;
        }


        for (Simian::u32 i = 0; i < phaseVars_[currentPhase_].skills_.size(); ++i)
        {
            if (number < phaseVars_[currentPhase_].skills_[i].second)
            {
                skill = phaseVars_[currentPhase_].skills_[i].first;
                break;
            }
            number -= phaseVars_[currentPhase_].skills_[i].second;
        }

        if (skill >= 0)
            fsm_.ChangeState(skill);
        else
        {
            spellTimer_ = phaseVars_[currentPhase_].spellDelay_;
            fsm_.ChangeState(MS_CHASE);
        }
    }

    void GCAIMajen::triangleEnter_( Simian::DelegateParameter& )
    {
        timer_ = 0.0f;
        model_->Controller().PlayAnimation(MAJEN_CAST_TRIANGLE);

        Simian::f32 oneoverr2 = 1.0f/std::sqrt(2.0f);

        Simian::Vector3 points[3] = 
        {
            Simian::Vector3(0.0f, 0.0f, 1.0f),
            Simian::Vector3(oneoverr2, 0.0f, -oneoverr2),
            Simian::Vector3(-oneoverr2, 0.0f, -oneoverr2)
        };

        for (Simian::u32 i = 0; i < 3; ++i)
            points[i] = GCPlayerLogic::Instance()->Transform().World().Position() + 
                phaseVars_[currentPhase_].triangleSize_ * points[i];
        
        // create the majen triangle
        GCMajenTriangle* triangleComp;
        Simian::Entity* majenTriangle = ParentScene()->CreateEntity(E_MAJENTRIANGLE);
        majenTriangle->ComponentByType(GC_MAJENTRIANGLE, triangleComp);
        triangleComp->CastingTime(phaseVars_[currentPhase_].triangleCastTime_);
        triangleComp->Points(points);
        triangleComp->OwnerAttribs(attributes_);
        triangleComp->Damage(triangleDamage_);
        ParentScene()->AddEntity(majenTriangle);

        attributes_->MovementSpeed(0.0f);

        // Play sounds
        if (sounds_)
            sounds_->Play(S_FIRETRIANGLE_FORM);
    }

    void GCAIMajen::triangleUpdate_( Simian::DelegateParameter& param )
    {
        Simian::FiniteStateUpdateParameter* p;
        param.As(p);
        timer_ += p->Dt;

        if (timer_ >= phaseVars_[currentPhase_].triangleCastTime_)
        {
            spellTimer_ = phaseVars_[currentPhase_].spellDelay_;

            // Play sounds
            if (sounds_)
                sounds_->Play(S_FIRETRIANGLE_FIRE);
            fsm_.ChangeState(MS_CHASE);
        }
    }

    void GCAIMajen::spinEnter_( Simian::DelegateParameter& )
    {
        timer_ = 0.0f;
        model_->Controller().PlayAnimation(MAJEN_SPIN);
        spinDamageTimer_ = 0.0f;
        attributes_->MovementSpeed(spinMovementSpeed_);
        updateNextTile_();

        Simian::Entity* fireEff;
        fireEff = ParentScene()->CreateEntity(E_FIRESPINEFFECT);
        fireEff->ComponentByType(Simian::C_TRANSFORM,fireSpinTransform_);
        fireEff->ComponentByType(GC_PARTICLECHAINDEACTIVATOR,fireSpinDeactivator_);
        ParentScene()->AddEntity(fireEff);

        spinSoundTimer_ = spinSoundTimerMax_;
    }

    void GCAIMajen::spinUpdate_( Simian::DelegateParameter& param )
    {
        Simian::FiniteStateUpdateParameter* p;
        param.As(p);

        fireSpinTransform_->Position(transform_->Position());
        timer_ += p->Dt;
        spinDamageTimer_ -= p->Dt;
        spinDamageTimer_ = Simian::Math::Max(spinDamageTimer_, 0.0f);

        if (reachedTile_(p->Dt))
            updateNextTile_();

        // get distance to the player 
        Simian::Vector3 distVec = GCPlayerLogic::Instance()->Transform().WorldPosition() - transform_->WorldPosition();
        Simian::f32 distSq = distVec.LengthSquared();
        Simian::f32 playerRadius = GCPlayerLogic::Instance()->Physics()->Radius();
        Simian::f32 spinDamageRadius = spinDamageRadius_ + playerRadius;
        if (distSq < spinDamageRadius * spinDamageRadius)
        {
            if (spinDamageTimer_ <= 0.0f)
            {
                // damage must be dealt!
                spinDamageTimer_ = MAJEN_SPIN_DAMAGE_INTERVAL;
                CombatCommandSystem::Instance().AddCommand(attributes_, 
                    GCPlayerLogic::Instance()->Attributes(), CombatCommandDealDamage::Create(
                    static_cast<Simian::s32>(spinDamage_)));
            }
        }

        Simian::f32 damageRange = attributes_->AttackRange() + playerRadius + physics_->Radius();
        if (distSq < damageRange * damageRange)
        {
            attributes_->MovementSpeed(0.0f);
        }
        else
        {
            attributes_->MovementSpeed(spinMovementSpeed_);
            moveTowardsPlayer_(p->Dt);
        }

        DebugRendererDrawCircle(transform_->World().Position(), spinDamageRadius_, Simian::Color(1.0f, 0.0f, 0.0f, 1.0f));

        if (timer_ > phaseVars_[currentPhase_].spinDuration_)
        {
            attributes_->MovementSpeed(0.0f);
            fsm_.ChangeState(MS_SPINEND);
        }

        // Play sound
        spinSoundTimer_ += p->Dt;
        if (spinSoundTimer_ > spinSoundTimerMax_)
        {
            spinSoundTimer_ = 0.0f;
            if(sounds_)
                sounds_->Play(S_SPIN);
        }
    }

    void GCAIMajen::spinEndExit_( Simian::DelegateParameter& )
    {
        spellTimer_ = phaseVars_[currentPhase_].spellDelay_;
        fireSpinDeactivator_->Duration(0);

        // Stop sounds
        if (sounds_)
            sounds_->Stop(S_SPIN);
    }

    void GCAIMajen::slamEnter_( Simian::DelegateParameter& )
    {
        model_->Controller().PlayAnimation(MAJEN_SLAM, false);
        timer_ = 0.0f;

        // deals ze damage
        Simian::Vector3 distVec = GCPlayerLogic::Instance()->Transform().WorldPosition() - transform_->WorldPosition();
        Simian::f32 range = slamRadius_ + GCPlayerLogic::Instance()->Physics()->Radius();
        if (distVec.LengthSquared() < range * range)
        {
            // deal damage for slam
            CombatCommandSystem::Instance().AddCommand(attributes_, GCPlayerLogic::Instance()->Attributes(), 
                CombatCommandDealDamage::Create(slamDamage_));
        }

        // spawns ze fire snakes
        Simian::f32 angleInc = 360.0f/phaseVars_[currentPhase_].slamSnakeCount_;
        angleInc = Simian::Degree(angleInc).Radians();
        for (Simian::s32 i = 0; i < phaseVars_[currentPhase_].slamSnakeCount_; ++i)
        {
            Simian::f32 angle = angleInc * i;
            Simian::Vector3 dir(std::cos(angle), 0.0f, std::sin(angle));

            Simian::Entity* entity = ParentScene()->CreateEntity(E_FLAMINGSNAKEEFFECT);
            GCFlamingSnakeEffect* snake;
            entity->ComponentByType(GC_FLAMINGSNAKEEFFECT, snake);
            snake->DealDamage(true);
            snake->Damage(slamSnakeDamage_);
            snake->Direction(dir);
            Simian::CTransform* transform;
            entity->ComponentByType(Simian::C_TRANSFORM, transform);
            transform->WorldPosition(transform_->WorldPosition() + dir * physics_->Radius());
            ParentScene()->AddEntity(entity);
        }

        // shake camera
        GCDescensionCamera::Instance()->Shake(0.5f, 0.5f);

        // Play sounds
        if (sounds_)
        {
            sounds_->Play(S_STOMP);
            sounds_->Play(S_FIRESNAKES);
        }
    }

    void GCAIMajen::slamUpdate_( Simian::DelegateParameter& param )
    {
        Simian::FiniteStateUpdateParameter* p;
        param.As(p);

        timer_ += p->Dt;

        DebugRendererDrawCircle(transform_->WorldPosition(), slamRadius_, Simian::Color(1.0f, 0.0f, 0.0f, 1.0f));

        if (timer_ > slamCooldown_)
        {
            spellTimer_ = phaseVars_[currentPhase_].spellDelay_;
            fsm_.ChangeState(MS_CHASE);
        }
    }

    void GCAIMajen::cloneEnter_( Simian::DelegateParameter& )
    {
        timer_ = 0.0f;
        model_->Controller().PlayAnimation(MAJEN_CLONE);

        // Play sounds
        if (sounds_)
            sounds_->Play(S_CLONE);
    }

    void GCAIMajen::cloneUpdate_( Simian::DelegateParameter& param )
    {
        Simian::FiniteStateUpdateParameter* p;
        param.As(p);

        timer_ += p->Dt;

        if (timer_ > cloneCastTime_)
        {
            // create the clone
            Simian::f32 angleInc = 360.0f/phaseVars_[currentPhase_].cloneCount_;
            angleInc = Simian::Degree(angleInc).Radians();
            for (Simian::s32 i = 0; i < phaseVars_[currentPhase_].cloneCount_; ++i)
            {
                Simian::f32 angle = angleInc * i;
                Simian::Entity* clone = ParentScene()->CreateEntity(E_ENEMYMAJEN);
                Simian::Entity* cloneEff = ParentScene()->CreateEntity(E_MAJENCLONEEFFECT);

                GCAIMajen* majen;
                Simian::CTransform* effTrans;

                cloneEff->ComponentByType(Simian::C_TRANSFORM,effTrans);
                
                clone->ComponentByType(GC_AIMAJEN, majen);
                majen->clone_ = true;
                Simian::CTransform* transform;
                clone->ComponentByType(Simian::C_TRANSFORM, transform);
                transform->WorldPosition(transform_->WorldPosition() + 2.0f * physics_->Radius() *
                    Simian::Vector3(std::cos(angle), 0, std::sin(angle)));

                effTrans->Position(transform->Position());
                ParentScene()->AddEntity(clone);
                ParentScene()->AddEntity(cloneEff);
            }

            spellTimer_ = phaseVars_[currentPhase_].spellDelay_;
            fsm_.ChangeState(MS_CHASE);
        }
    }

    void GCAIMajen::jumpBackEnter_( Simian::DelegateParameter& )
    {
        timer_ = 0.0f;
        model_->Controller().PlayAnimation(MAJEN_JUMPBACK, false);
        Simian::Vector3 direction = transform_->WorldPosition() - GCPlayerLogic::Instance()->Transform().WorldPosition();
        attributes_->Direction(direction);
        attributes_->MovementSpeed(jumpBackSpeed_);

        // Play sounds
        if (sounds_)
            sounds_->Play(S_DEATHTOTHEEMAE);
    }

    void GCAIMajen::jumpBackUpdate_( Simian::DelegateParameter& param )
    {
        Simian::FiniteStateUpdateParameter* p;
        param.As(p);

        timer_ += p->Dt;

        if (timer_ > jumpBackTime_)
        {
            attributes_->MovementSpeed(0.0f);
            fsm_.ChangeState(MS_SHOOTARC);
        }
    }

    void GCAIMajen::shootArcEnter_( Simian::DelegateParameter& )
    {
        model_->Controller().PlayAnimation(MAJEN_SHOOT, false);

        // get the vector to the player
        Simian::Vector3 dir = GCPlayerLogic::Instance()->Transform().WorldPosition() - transform_->WorldPosition();
        dir.Y(0);
        dir.Normalize();

        // get the right vec
        Simian::Vector3 right = dir.Cross(Simian::Vector3::UnitY);
        Simian::Vector3 left = Simian::Vector3::UnitY.Cross(dir);

        // scale the vectors by the width and height of the "triangle"
        // creates the axes in global space.
        Simian::Vector3 forward(dir * arcHeight_);
        left *= arcWidth_;
        right *= arcWidth_;

        // start at corner bullet
        Simian::Vector2 posInitial(1, 0);

        // incremental term
        Simian::Vector2 inc(-1.0f/numBullets_, 1.0f/numBullets_);

        for (Simian::s32 i = 0; i < numBullets_; ++i)
        {
            // create the bullets in global
            createBullet_(transform_->WorldPosition() + left * posInitial.X()
                + forward * posInitial.Y(), dir);
            createBullet_(transform_->WorldPosition() + right * posInitial.X()
                + forward * posInitial.Y(), dir);

            posInitial += inc;
        }

        // create the bullet at the tip
        createBullet_(transform_->WorldPosition() + forward, dir);

        // Rotate
        attributes_->Direction(dir);
        Simian::f32 angle = std::atan2(-dir.Z(), dir.X());
        transform_->Rotation(Simian::Vector3::UnitY, Simian::Radian(angle + Simian::Math::PI * 0.5f));

        // Play random sound
        if (sounds_)
            sounds_->Play(Simian::Math::Random(S_SHOOT1, S_SHOOT1 + TOTAL_SHOOT_SOUNDS));
    }

    void GCAIMajen::shootArcUpdate_( Simian::DelegateParameter& )
    {
        fsm_.ChangeState(MS_ARCCOOLDOWN);
    }

    void GCAIMajen::arcCooldownEnter_( Simian::DelegateParameter& )
    {
        timer_ = 0.0f;
    }

    void GCAIMajen::arcCooldownUpdate_( Simian::DelegateParameter& param )
    {
        Simian::FiniteStateUpdateParameter* p;
        param.As(p);

        timer_ += p->Dt;

        Simian::Vector3 dir = GCPlayerLogic::Instance()->Transform().WorldPosition() -
            transform_->WorldPosition();
        dir.Normalize();
        attributes_->Direction(dir);
        Simian::f32 angle = std::atan2(-dir.Z(), dir.X());
        transform_->Rotation(Simian::Vector3::UnitY, Simian::Radian(angle + Simian::Math::PI * 0.5f));

        if (shotCount_ >= 2)
        {
            if (timer_ > bulletInterval_)
            {
                shotCount_ = 0;
                fsm_.ChangeState(MS_DASH);
            }
        }
        else
        {
            if (timer_ > bulletInterval_)
            {
                ++shotCount_;
                fsm_.ChangeState(MS_SHOOTARC);
            }
        }
    }

    void GCAIMajen::dashEnter_( Simian::DelegateParameter& )
    {
        Simian::Entity* auraeff = ParentScene()->CreateEntity(E_MAJENCHARGEAURA);
        auraeff->ComponentByType(Simian::C_TRANSFORM, chargeAuraTransform_);
        auraeff->ChildByName("Aura")->ComponentByType(GC_PARTICLEEFFECTDEACTIVATOR,chargeAuraDeactivator_);

        chargeAuraDeactivator_->MaxTimer(dashDuration_);
        ParentScene()->AddEntity(auraeff);

        timer_ = 0.0f;
        model_->Controller().PlayAnimation(MAJEN_DASH, false);
        Simian::Vector3 direction = GCPlayerLogic::Instance()->Transform().WorldPosition() - transform_->WorldPosition();
        attributes_->Direction(direction);
        attributes_->MovementSpeed(dashSpeed_);
        dashHit_ = false;
        physics_->UnsetCollisionLayer(Collision::COLLISION_L1);

        // get the angle, set the rotation
        Simian::f32 angle = std::atan2(-direction.Z(), direction.X());
        transform_->Rotation(Simian::Vector3::UnitY, Simian::Radian(angle + Simian::Math::PI * 0.5f));

        // Play sounds
        if (sounds_)
            sounds_->Play(S_SWORD_DASH);
    }

    void GCAIMajen::dashUpdate_( Simian::DelegateParameter& param )
    {
        chargeAuraTransform_->Position(transform_->Position());
        Simian::FiniteStateUpdateParameter* p;
        param.As(p);

        timer_ += p->Dt;

        Simian::f32 intp = timer_/dashDuration_;
        intp = (intp - 0.5f)/0.5f;
        intp = Simian::Math::Clamp(intp, 0.0f, 1.0f);

        Simian::f32 speed = Simian::Interpolation::Interpolate(dashSpeed_, 0.0f, intp, Simian::Interpolation::EaseCubicOut);
        attributes_->MovementSpeed(speed);

        if (!dashHit_)
        {
            Simian::Vector3 distVec = GCPlayerLogic::Instance()->Transform().WorldPosition() - transform_->WorldPosition();
            Simian::f32 range = GCPlayerLogic::Instance()->Physics()->Radius() + physics_->Radius();
            if (distVec.LengthSquared() < range * range)
            {
                dashHit_ = true;
                CombatCommandSystem::Instance().AddCommand(attributes_, GCPlayerLogic::Instance()->Attributes(),
                    CombatCommandDealDamage::Create(dashDamage_));
            }
        }

        if (timer_ > dashDuration_)
        {
            physics_->SetCollisionLayer(Collision::COLLISION_L1);
            spellTimer_ = phaseVars_[currentPhase_].spellDelay_;
            fsm_.ChangeState(MS_CHASE);
        }
    }

    void GCAIMajen::stunEnter_( Simian::DelegateParameter& )
    {
        if(fireSpinDeactivator_)
            fireSpinDeactivator_->Duration(0);
        timer_ = 0.0f;
        model_->Controller().PlayAnimation(MAJEN_STUN);
        stunHp_ = attributes_->Health();
        attributes_->MovementSpeed(0.0f);

        Simian::u32 trapSize = trapList.size();
        Simian::u32 numTraps = trapsEachPhase_;
        while(numTraps > 0 && trapSize > 0)
        {
            Simian::u32 randInt = Simian::Math::Random(0, trapSize);
            GCTrapFireSpot* fireSpot = 0;
            trapList[randInt]->ComponentByType(GC_TRAPFIRESPOT, fireSpot);
            if (fireSpot)
            {
                fireSpot->Enabled(true);
                trapList.erase(trapList.begin()+randInt);
            }
            --trapSize;
            --numTraps;
        }

    }

    void GCAIMajen::stunUpdate_( Simian::DelegateParameter& param )
    {
        Simian::FiniteStateUpdateParameter* p;
        param.As(p);
        timer_ += p->Dt;

        // Update flinch timer
        flinchTimer_ = (flinchTimer_>0.0f) ? (flinchTimer_-p->Dt) : -1.0f;

        if (timer_ > phaseVars_[currentPhase_].stunMaxDuration_ ||
            stunHp_ - attributes_->Health() > phaseVars_[currentPhase_].stunCapDmg_)
        {
            ++currentPhase_;
            currentPhase_ = Simian::Math::Min(currentPhase_, 2U);

            if (attributes_->Health() <= 0)
                fsm_.ChangeState(MS_DEATH);
            else 
            {
                // start off with a fire snake attack after the stun end
                fsm_.ChangeState(MS_STUNNEDEND); 
                // Play sounds
                if (sounds_)
                    sounds_->Play(S_THECRYSTALISMINE);
            }
        }
    }

    void GCAIMajen::deathEnter_( Simian::DelegateParameter& )
    {
        timer_ = 0.0f;
        model_->Controller().PlayAnimation(MAJEN_DIE, false);
        if (!clone_)
            GCUI::Instance()->EndBossBattle();
        attributes_->MovementSpeed(0.0f);
        physics_->Enabled(false);

        // Disable fire traps
        if (!clone_)
        {
            Simian::u32 trapSize = trapListOri.size();
            for (Simian::u32 i = 0; i < trapSize; ++i)
            {
                GCTrapFireSpot* fireSpot = 0;
                trapListOri[i]->ComponentByType(GC_TRAPFIRESPOT, fireSpot);
                if (fireSpot)
                    fireSpot->Enabled(false);
            }
        }

        Simian::Entity* backFire = ParentEntity()->ChildByType(E_MAJENBODYFIRE);
        Simian::CParticleSystem* particle;
        backFire->ChildByName("Fire Particle")->ComponentByType(Simian::C_PARTICLESYSTEM,particle);
        particle->ParticleSystem()->Enabled(false);
        
        backFire->ChildByName("Smoke")->ComponentByType(Simian::C_PARTICLESYSTEM,particle);
        particle->ParticleSystem()->Enabled(false);
    }

    void GCAIMajen::deathUpdate_( Simian::DelegateParameter& )
    {
    }

    void GCAIMajen::afterPlayerDeathEnter_( Simian::DelegateParameter& )
    {
        timer_ = 0.0f;
        model_->Controller().PlayAnimation(MAJEN_IDLE_ANIMATION);
        attributes_->MovementSpeed(0.0f);
    }

    void GCAIMajen::afterPlayerDeathUpdate_( Simian::DelegateParameter& )
    {
    }

    void GCAIMajen::onTakeDamage_( Simian::DelegateParameter& )
    {
        if (fsm_.CurrentState() == MS_STUNNED && flinchTimer_ < 0.0f)
        {
            flinchTimer_ = MAJEN_FLINCH_TIMER_MAX;
            if ((rand()%2) < 1)
                model_->Controller().PlayAnimation(MAJEN_FLINCH_1, false);
            else
                model_->Controller().PlayAnimation(MAJEN_FLINCH_2, false);
            
            // Play random sounds
            if (sounds_)
                sounds_->Play(Simian::Math::Random(S_HIT_GRUNT_1, S_HIT_GRUNT_1 + TOTAL_HIT_GRUNT_SOUNDS));
        }
        // Play random sound
        if (sounds_)
            sounds_->Play(Simian::Math::Random(S_GETHIT_1, S_GETHIT_1 + TOTAL_MELEE_GET_HIT_SOUNDS));
    }

    void GCAIMajen::readPhaseVars_( Simian::u32 index, const Simian::FileObjectSubNode* phase )
    {
        if (!phase)
            return;

        PhaseVars& v = phaseVars_[index];
        phase->Data("SpellDelay", v.spellDelay_);
        phase->Data("TriangleCastTime", v.triangleCastTime_);
        phase->Data("TriangleSize", v.triangleSize_);
        phase->Data("SpinCastTime", v.spinCastTime_);
        phase->Data("SpinDuration", v.spinDuration_);
        phase->Data("SlamCastTime", v.slamCastTime_);
        phase->Data("SlamSnakeCount", v.slamSnakeCount_);
        phase->Data("CloneCount", v.cloneCount_);
        phase->Data("NextPhaseHP", v.nextPhaseHP_);
        phase->Data("StunCapDmg", v.stunCapDmg_);
        phase->Data("StunMaxDuration", v.stunMaxDuration_);
        phase->Data("StunStartTime", v.stunStartTime_);
        phase->Data("StunEndTime", v.stunEndTime_);

        const Simian::FileArraySubNode* skills = phase->Array("Skills");
        if (skills)
        {
            for (Simian::u32 i = 0; i < skills->Size(); ++i)
            {
                const Simian::FileObjectSubNode* skill = skills->Object(i);
                int id, prob;
                skill->Data("Id", id);
                skill->Data("Probability", prob);
                v.skills_.push_back(std::make_pair(id, prob));
            }
        }
    }

    void GCAIMajen::writePhaseVars_( Simian::u32 index, Simian::FileObjectSubNode* phase )
    {
        if (!phase)
            return;

        PhaseVars& v = phaseVars_[index];
        phase->AddData("SpellDelay", v.spellDelay_);
        phase->AddData("TriangleCastTime", v.triangleCastTime_);
        phase->AddData("TriangleSize", v.triangleSize_);
        phase->AddData("SpinCastTime", v.spinCastTime_);
        phase->AddData("SpinDuration", v.spinDuration_);
        phase->AddData("SlamCastTime", v.slamCastTime_);
        phase->AddData("SlamSnakeCount", v.slamSnakeCount_);
        phase->AddData("CloneCount", v.cloneCount_);
        phase->AddData("NextPhaseHP", v.nextPhaseHP_);
        phase->AddData("StunCapDmg", v.stunCapDmg_);
        phase->AddData("StunMaxDuration", v.stunMaxDuration_);
        phase->AddData("StunStartTime", v.stunStartTime_);
        phase->AddData("StunEndTime", v.stunEndTime_);

        Simian::FileArraySubNode* skills = phase->AddArray("Skills");
        for (Simian::u32 i = 0; i < v.skills_.size(); ++i)
        {
            Simian::FileObjectSubNode* skill = skills->AddObject("Skill");
            skill->Data("Id", v.skills_[i].first);
            skill->Data("Probability", v.skills_[i].second);
        }
    }

    bool GCAIMajen::reachedTile_(Simian::f32 dt)
    {
        Simian::f32 futureDist = attributes_->MovementSpeed() * dt;
        futureDist *= futureDist;
        Simian::Vector3 currentDist(nextTile_ - transform_->World().Position());
        Simian::f32 dist = currentDist.LengthSquared();
        return ((dist < Simian::Math::EPSILON) || (dist > futureDist));
    }

    void GCAIMajen::updateNextTile_()
    {
        nextTile_ = PathFinder::Instance().GeneratePath(physics_->Radius(), MAJEN_PATHFINDER_RANGE, 
            transform_->World().Position(), GCPlayerLogic::Instance()->Transform().World().Position());
		Simian::Vector3 dir = nextTile_ - transform_->Position();

        Simian::f32 dist = dir.LengthSquared();
		if(dist > Simian::Math::EPSILON)
			finalDir_ = dir.Normalized();
		steerTimer_ = 0.0f;
		startDir_ = attributes_->Direction();
    }

    void GCAIMajen::updateSpellTimer_( Simian::DelegateParameter& param )
    {
        Simian::FiniteStateUpdateParameter* p;
        param.As(p);
        spellTimer_ -= p->Dt;
        spellTimer_ = Simian::Math::Max(spellTimer_, 0.0f);
        if (clone_)
        {
            cloneLifetime_ -= p->Dt;
            if (cloneLifetime_ <= 0.0f)
            {
                // deal enough damage to kill him instantly
                CombatCommandSystem::Instance().AddCommand(attributes_, attributes_,
                    CombatCommandDealDamage::Create(attributes_->MaxHealth()));
            }
        }
        
        if (attributes_->Health() <= 0)
        {
            if (fsm_.CurrentState() != MS_DEATH)
                fsm_.ChangeState(MS_DEATH);
        }
        else if (fsm_.CurrentState() != MS_STUNNED && fsm_.CurrentState() != MS_STUNNEDSTART &&
                 attributes_->Health() <= phaseVars_[currentPhase_].nextPhaseHP_)
        {
            // change to the stunned state
            fsm_.ChangeState(MS_STUNNEDSTART);

            // Play random sounds
            if (sounds_)
                sounds_->Play(Simian::Math::Random(S_STUN_GRUNT_1, S_STUN_GRUNT_1 + TOTAL_STUN_GRUNT_SOUNDS));
        }

        // reminders
        if (reminderPrev_ != REMINDER_MAX_TIMES+1 && attributes_->Health() > REMINDER_MIN_HP)
        {
            reminderTimer_ += p->Dt;
            if (reminderTimer_ > reminderTimerMax_)
            {
                reminderTimer_ = 0.0f;
                if (!reminderPrev_)
                {
                    if ((rand()%2) < 1)
                    {
                        reminderPrev_ = 1;
                        if (reminderDodgeScript_)
                            reminderDodgeScript_->OnTriggered(Simian::Delegate::NoParameter);
                    }
                    else
                    {
                        reminderPrev_ = 2;
                        if (reminderDemystScript_)
                            reminderDemystScript_->OnTriggered(Simian::Delegate::NoParameter);
                    }
                }
                else if (reminderPrev_ == 1)
                {
                    reminderPrev_ = 3;
                    if (reminderDemystScript_)
                        reminderDemystScript_->OnTriggered(Simian::Delegate::NoParameter);
                }
                else if (reminderPrev_ == 2)
                {
                    reminderPrev_ = 3;
                    if (reminderDodgeScript_)
                        reminderDodgeScript_->OnTriggered(Simian::Delegate::NoParameter);
                }
            }
        }
    }

    void GCAIMajen::postPhysics_( Simian::DelegateParameter& )
    {
#ifdef _DESCENDED
        if (GCEditor::EditorMode())
            return;
#endif
        if(fsm_.CurrentState() == MS_DEATH)
            PathFinder::Instance().UnsetPosition(oldMin_,oldMax_);
        else
            PathFinder::Instance().SetNewPosition(transform_->World().Position(),physics_->Radius(),oldMin_,oldMax_);
    }

    void GCAIMajen::overRideStateUpdate_( Simian::f32 )
    {
    }

    void GCAIMajen::moveTowardsPlayer_( Simian::f32 dt )
    {
        // move in my steering direction
        steerTimer_ += dt;
        Simian::f32 intp = steerTimer_/MAJEN_MAX_STEER_TIME;
        intp = Simian::Math::Clamp(intp, 0.0f, 1.0f);

        // interpolate to get current direction
        Simian::Vector3 curDirection = Simian::Interpolation::Interpolate(startDir_, finalDir_, intp, Simian::Interpolation::Linear);
        if (curDirection.LengthSquared() > Simian::Math::EPSILON)
        {
            curDirection.Normalize();
            Simian::f32 angle = std::atan2(-curDirection.Z(), curDirection.X());
            transform_->Rotation(Simian::Vector3::UnitY, Simian::Radian(angle + Simian::Math::PI * 0.5f));
            attributes_->Direction(curDirection);
        }
    }

    void GCAIMajen::createBullet_( const Simian::Vector3& position, const Simian::Vector3& direction )
    {
        Simian::Entity* entity = ParentScene()->CreateEntity(E_ENEMYFIREBULLET);

        GCEnemyBulletLogic* bullet;
        entity->ComponentByType(GC_ENEMYBULLETLOGIC, bullet);
        bullet->OwnerAttribs(attributes_);
        bullet->MovementVec(direction);

        Simian::CTransform* transform;
        entity->ComponentByType(Simian::C_TRANSFORM, transform);
        transform->WorldPosition(position);

        ParentScene()->AddEntity(entity);
    }

    //--------------------------------------------------------------------------

    void GCAIMajen::OnSharedLoad()
    {
        RegisterCallback(Simian::P_UPDATE, Simian::Delegate::CreateDelegate<GCAIMajen, &GCAIMajen::updateSpellTimer_>(this));
        RegisterCallback(Simian::P_POSTPHYSICS, Simian::Delegate::CreateDelegate<GCAIMajen, &GCAIMajen::postPhysics_>(this));
        GCAIBase::OnSharedLoad();
    }

    void GCAIMajen::OnInit()
	{
#ifdef _DESCENDED
		if (GCEditor::EditorMode())
			return;
#endif
        
        GCAIBase::OnInit();

		// for weapons and other attachments
		if (!attachments_.empty())
			for (Simian::u32 i = 0; i < attachments_.size(); ++i)
				model_->AttachEntityToJoint(attachments_[i].first, ParentEntity()->ChildByName(attachments_[i].second));

        // if it is a clone
        if (clone_)
        {
            attributes_->DeathFadeToBlack(true);
            phaseVars_[0].nextPhaseHP_ = 0;
        }
        else
            GCUI::Instance()->StartBossBattle(this, GCUI::UI_BOSS0);

        // register attribute callbacks
        attributes_->DealDamageDelegates() += 
            Simian::Delegate::CreateDelegate<GCAIMajen, &GCAIMajen::onTakeDamage_>(this);

        // Get list of traps
        ParentScene()->EntitiesByType(E_TRAPFIRESPOT,trapListOri);
        trapList = trapListOri;

        Simian::Entity* ent = 0;
        // Getting the reminder dodge event script
        ent = ParentScene()->EntityByNameRecursive("BossDodgeReminder");
        if( ent )
            ent->ComponentByType(GC_EVENTSCRIPT,reminderDodgeScript_);

        // Getting the reminder demyst event script
        ent = ParentScene()->EntityByNameRecursive("BossDemystReminder");
        if( ent )
            ent->ComponentByType(GC_EVENTSCRIPT,reminderDemystScript_);
	}

    void GCAIMajen::OnAwake()
    {
        GCAIBase::OnAwake();

        // get transform and physics
        ComponentByType(Simian::C_TRANSFORM, transform_);
        ComponentByType(Simian::C_SOUNDEMITTER, sounds_);

        startDir_ = finalDir_ = transform_->Direction();
        attributes_->Direction(startDir_);

        // set up state machine
        fsm_[MS_CHASE].OnEnter = Simian::Delegate::CreateDelegate<GCAIMajen, &GCAIMajen::chaseEnter_>(this);
        fsm_[MS_CHASE].OnUpdate = Simian::Delegate::CreateDelegate<GCAIMajen, &GCAIMajen::chaseUpdate_>(this);
        fsm_[MS_CHASE].OnExit = Simian::Delegate::CreateDelegate<GCAIMajen, &GCAIMajen::chaseExit_>(this);

        fsm_[MS_IDLE].OnEnter = Simian::Delegate::CreateDelegate<GCAIMajen, &GCAIMajen::idleEnter_>(this);
        fsm_[MS_IDLE].OnUpdate = Simian::Delegate::CreateDelegate<GCAIMajen, &GCAIMajen::idleUpdate_>(this);

        fsm_[MS_MELEE].OnEnter = Simian::Delegate::CreateDelegate<GCAIMajen, &GCAIMajen::meleeEnter_>(this);
        fsm_[MS_MELEE].OnUpdate = Simian::Delegate::CreateDelegate<GCAIMajen, &GCAIMajen::meleeUpdate_>(this);

        fsm_[MS_SWING].OnEnter = Simian::Delegate::CreateDelegate<GCAIMajen, &GCAIMajen::swingEnter_>(this);
        fsm_[MS_SWING].OnUpdate = Simian::Delegate::CreateDelegate<GCAIMajen, &GCAIMajen::swingUpdate_>(this);
        fsm_[MS_SWING].OnExit = Simian::Delegate::CreateDelegate<GCAIMajen, &GCAIMajen::swingExit_>(this);

        fsm_[MS_CHOOSESKILL].OnUpdate = Simian::Delegate::CreateDelegate<GCAIMajen, &GCAIMajen::chooseUpdate_>(this);

        fsm_[MS_TRIANGLE].OnEnter = Simian::Delegate::CreateDelegate<GCAIMajen, &GCAIMajen::triangleEnter_>(this);
        fsm_[MS_TRIANGLE].OnUpdate = Simian::Delegate::CreateDelegate<GCAIMajen, &GCAIMajen::triangleUpdate_>(this);

        fsm_[MS_SPINSTART].OnEnter = Simian::Delegate::CreateDelegate<GCAIMajen, &GCAIMajen::spinStartEnter_>(this);
        fsm_[MS_SPINSTART].OnUpdate = Simian::Delegate::CreateDelegate<GCAIMajen, &GCAIMajen::castUpdate_<&PhaseVars::spinCastTime_, MS_SPIN> >(this);
        
        fsm_[MS_SPIN].OnEnter = Simian::Delegate::CreateDelegate<GCAIMajen, &GCAIMajen::spinEnter_>(this);
        fsm_[MS_SPIN].OnUpdate = Simian::Delegate::CreateDelegate<GCAIMajen, &GCAIMajen::spinUpdate_>(this);

        fsm_[MS_SPINEND].OnEnter = Simian::Delegate::CreateDelegate<GCAIMajen, &GCAIMajen::spinEndEnter_>(this);
        fsm_[MS_SPINEND].OnUpdate = Simian::Delegate::CreateDelegate<GCAIMajen, &GCAIMajen::castUpdate_<&PhaseVars::spinCastTime_, MS_CHASE> >(this);
        fsm_[MS_SPINEND].OnExit = Simian::Delegate::CreateDelegate<GCAIMajen, &GCAIMajen::spinEndExit_>(this);

        fsm_[MS_FIRESNAKESSTART].OnEnter = Simian::Delegate::CreateDelegate<GCAIMajen, &GCAIMajen::slamStartEnter_>(this);
        fsm_[MS_FIRESNAKESSTART].OnUpdate = Simian::Delegate::CreateDelegate<GCAIMajen, &GCAIMajen::castUpdate_<&PhaseVars::slamCastTime_, MS_FIRESNAKES> >(this);

        fsm_[MS_FIRESNAKES].OnEnter = Simian::Delegate::CreateDelegate<GCAIMajen, &GCAIMajen::slamEnter_>(this);
        fsm_[MS_FIRESNAKES].OnUpdate = Simian::Delegate::CreateDelegate<GCAIMajen, &GCAIMajen::slamUpdate_>(this);

        fsm_[MS_MIRRORIMAGE].OnEnter = Simian::Delegate::CreateDelegate<GCAIMajen, &GCAIMajen::cloneEnter_>(this);
        fsm_[MS_MIRRORIMAGE].OnUpdate = Simian::Delegate::CreateDelegate<GCAIMajen, &GCAIMajen::cloneUpdate_>(this);

        fsm_[MS_JUMPBACK].OnEnter = Simian::Delegate::CreateDelegate<GCAIMajen, &GCAIMajen::jumpBackEnter_>(this);
        fsm_[MS_JUMPBACK].OnUpdate = Simian::Delegate::CreateDelegate<GCAIMajen, &GCAIMajen::jumpBackUpdate_>(this);

        fsm_[MS_SHOOTARC].OnEnter = Simian::Delegate::CreateDelegate<GCAIMajen, &GCAIMajen::shootArcEnter_>(this);
        fsm_[MS_SHOOTARC].OnUpdate = Simian::Delegate::CreateDelegate<GCAIMajen, &GCAIMajen::shootArcUpdate_>(this);

        fsm_[MS_ARCCOOLDOWN].OnEnter = Simian::Delegate::CreateDelegate<GCAIMajen, &GCAIMajen::arcCooldownEnter_>(this);
        fsm_[MS_ARCCOOLDOWN].OnUpdate = Simian::Delegate::CreateDelegate<GCAIMajen, &GCAIMajen::arcCooldownUpdate_>(this);

        fsm_[MS_DASH].OnEnter = Simian::Delegate::CreateDelegate<GCAIMajen, &GCAIMajen::dashEnter_>(this);
        fsm_[MS_DASH].OnUpdate = Simian::Delegate::CreateDelegate<GCAIMajen, &GCAIMajen::dashUpdate_>(this);

        fsm_[MS_SPINSTART].OnEnter = Simian::Delegate::CreateDelegate<GCAIMajen, &GCAIMajen::spinStartEnter_>(this);
        fsm_[MS_SPINSTART].OnUpdate = Simian::Delegate::CreateDelegate<GCAIMajen, &GCAIMajen::castUpdate_<&PhaseVars::spinCastTime_, MS_SPIN> >(this);

        fsm_[MS_STUNNED].OnEnter = Simian::Delegate::CreateDelegate<GCAIMajen, &GCAIMajen::stunEnter_>(this);
        fsm_[MS_STUNNED].OnUpdate = Simian::Delegate::CreateDelegate<GCAIMajen, &GCAIMajen::stunUpdate_>(this);

        fsm_[MS_STUNNEDSTART].OnEnter = Simian::Delegate::CreateDelegate<GCAIMajen, &GCAIMajen::stunStartEnter_>(this);
        fsm_[MS_STUNNEDSTART].OnUpdate = Simian::Delegate::CreateDelegate<GCAIMajen, &GCAIMajen::castUpdate_<&PhaseVars::stunStartTime_, MS_STUNNED> >(this);

        fsm_[MS_STUNNEDEND].OnEnter = Simian::Delegate::CreateDelegate<GCAIMajen, &GCAIMajen::stunEndEnter_>(this);
        fsm_[MS_STUNNEDEND].OnUpdate = Simian::Delegate::CreateDelegate<GCAIMajen, &GCAIMajen::castUpdate_<&PhaseVars::stunEndTime_, MS_FIRESNAKESSTART> >(this);

        fsm_[MS_DEATH].OnEnter = Simian::Delegate::CreateDelegate<GCAIMajen, &GCAIMajen::deathEnter_>(this);
        fsm_[MS_DEATH].OnUpdate = Simian::Delegate::CreateDelegate<GCAIMajen, &GCAIMajen::deathUpdate_>(this);

        fsm_[MS_AFTERPLAYERDEATH].OnEnter = Simian::Delegate::CreateDelegate<GCAIMajen, &GCAIMajen::afterPlayerDeathEnter_>(this);
        fsm_[MS_AFTERPLAYERDEATH].OnUpdate = Simian::Delegate::CreateDelegate<GCAIMajen, &GCAIMajen::afterPlayerDeathUpdate_>(this);

        spellTimer_ = clone_ ? cloneSpellDelay_ : phaseVars_[currentPhase_].spellDelay_;
        if (clone_)
            attributes_->Health(cloneHp_);
    }

    void GCAIMajen::Serialize( Simian::FileObjectSubNode& data )
    {
        GCAIBase::Serialize(data);

        data.AddData("IdleDelay", idleDelay_);
        data.AddData("AttackDelay", attackDelay_);
        data.AddData("SwingDelay", swingDelay_);
        data.AddData("SpinDamageRadius", spinDamageRadius_);
        data.AddData("SpinDamage", spinDamage_);
        data.AddData("SpinMovementSpeed", spinMovementSpeed_);
        data.AddData("SlamCooldown", slamCooldown_);
        data.AddData("SlamRadius", slamRadius_);
        data.AddData("SlamDamage", slamDamage_);
        data.AddData("SlamSnakeDamage", slamSnakeDamage_);
        data.AddData("CloneCastTime", cloneCastTime_);
        data.AddData("CloneSpinChance", cloneSpinChance_);
        data.AddData("CloneSpellDelay", cloneSpellDelay_);
        data.AddData("CloneHp", cloneHp_);
        data.AddData("CloneLifetime", cloneLifetime_);
        data.AddData("JumpBackSpeed", jumpBackSpeed_);
        data.AddData("JumpBackTime", jumpBackTime_);
        data.AddData("BulletInterval", bulletInterval_);
        data.AddData("DashWaitTime", dashWaitTime_);
        data.AddData("ReminderTimer", reminderTimer_);
        data.AddData("ReminderTimerMax", reminderTimerMax_);

        data.AddData("DashSpeed", dashSpeed_);
        data.AddData("DashDuration", dashDuration_);
        data.AddData("DashDamage", dashDamage_);
        data.AddData("ArcWidth", arcWidth_);
        data.AddData("ArcHeight", arcHeight_);
        data.AddData("BulletCount", numBullets_);
        data.AddData("TrapsEachPhase", trapsEachPhase_);

        for (int i = 0; i < 3; ++i)
        {
            std::string name = "Phase";
            name += static_cast<char>('1' + i);
            writePhaseVars_(i, data.AddObject(name));
        }
    }

    void GCAIMajen::Deserialize( const Simian::FileObjectSubNode& data )
    {
        GCAIBase::Deserialize(data);

        data.Data("IdleDelay", idleDelay_, idleDelay_);
        data.Data("AttackDelay", attackDelay_, attackDelay_);
        data.Data("SwingDelay", swingDelay_, swingDelay_);
        data.Data("SpinDamageRadius", spinDamageRadius_, spinDamageRadius_);
        data.Data("SpinDamage", spinDamage_, spinDamage_);
        data.Data("SpinMovementSpeed", spinMovementSpeed_, spinMovementSpeed_);
        data.Data("SlamCooldown", slamCooldown_, slamCooldown_);
        data.Data("SlamRadius", slamRadius_, slamRadius_);
        data.Data("SlamDamage", slamDamage_, slamDamage_);
        data.Data("SlamSnakeDamage", slamSnakeDamage_, slamSnakeDamage_);
        data.Data("CloneCastTime", cloneCastTime_, cloneCastTime_);
        data.Data("CloneSpinChance", cloneSpinChance_, cloneSpinChance_);
        data.Data("CloneSpellDelay", cloneSpellDelay_, cloneSpellDelay_);
        data.Data("CloneHp", cloneHp_, cloneHp_);
        data.Data("CloneLifetime", cloneLifetime_, cloneLifetime_);
        data.Data("JumpBackSpeed", jumpBackSpeed_, jumpBackSpeed_);
        data.Data("JumpBackTime", jumpBackTime_, jumpBackTime_);
        data.Data("BulletInterval", bulletInterval_, bulletInterval_);
        data.Data("DashWaitTime", dashWaitTime_, dashWaitTime_);
        data.Data("ReminderTimer", reminderTimer_, reminderTimer_);
        data.Data("ReminderTimerMax", reminderTimerMax_, reminderTimerMax_);

        data.Data("DashSpeed", dashSpeed_, dashSpeed_);
        data.Data("DashDuration", dashDuration_, dashDuration_);
        data.Data("DashDamage", dashDamage_, dashDamage_);
        data.Data("ArcWidth", arcWidth_, arcWidth_);
        data.Data("ArcHeight", arcHeight_, arcHeight_);
        data.Data("TriangleDamage", triangleDamage_, triangleDamage_);
        data.Data("BulletCount", numBullets_, numBullets_);
        data.Data("TrapsEachPhase", trapsEachPhase_, trapsEachPhase_);

        for (int i = 0; i < 3; ++i)
        {
            std::string name = "Phase";
            name += static_cast<char>('1' + i);
            readPhaseVars_(i, data.Object(name));
        }
    }
}
