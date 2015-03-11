/*****************************************************************************/
/*!
\file		GCPlayerLogic.cpp
\author 	Gavin Yeo Wei Ming, gavin.yeo, 290000111
\par    	email: gavin.yeo\@digipen.edu
\date   	August 10, 2012
\brief
Component of the logic, controls and animation for the player character.

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*****************************************************************************/

#include "Simian/AnimationController.h"
#include "Simian/EnginePhases.h"
#include "Simian/EngineComponents.h"
#include "Simian/CTransform.h"
#include "Simian/CModel.h"
#include "Simian/CCamera.h"
#include "Simian/Camera.h"
#include "Simian/Keyboard.h"
#include "Simian/Mouse.h"
#include "Simian/Game.h"
#include "Simian/FiniteStateMachine.h"
#include "Simian/Math.h"
#include "Simian/Ray.h"
#include "Simian/Plane.h"
#include "Simian/Delegate.h"
#include "Simian/Utility.h"
#include "Simian/Interpolation.h"
#include "Simian/Scene.h"
#include "Simian/Entity.h"
#include "Simian/LogManager.h"
#include "Simian/DebugRenderer.h"
#include "Simian/CSoundEmitter.h"
#include "Simian/Material.h"
#include "Simian/Shader.h"
#include "Simian/ShaderFloatParameter.h"
#include "Simian/CModelColorOperation.h"
#include "Simian/CParticleSystem.h"
#include "Simian/ParticleSystem.h"
#include "Simian/CParticleTimeline.h"

#include "EntityTypes.h"
#include "ComponentTypes.h"
#include "Collision.h"
#include "CombatCommandSystem.h"
#include "CombatCommandDealDamage.h"
#include "StateLoading.h"
#include "StateGame.h"
#include "MetricLogManager.h"
#include "SaveManager.h"

#include "GCPlayerLogic.h"
#include "GCPhysicsController.h"
#include "GCPlayerBulletLogic.h"
#include "GCAIBase.h"
#include "GCEditor.h"
#include "GCAttributes.h"
#include "GCGestureInterpreter.h"
#include "GCUI.h"
#include "GCUpgradeExchange.h"
#include "GCBoneTrailVelocityController.h"
#include "GCCinematicUI.h"
#include "GCDescensionCamera.h"
#include "GCGameOver.h"
#include "GCGesturePuzzleTrigger.h"

#include <sstream>
#include <iostream>
#include "GCMousePlaneIntersection.h"

namespace Descension
{
    Simian::FactoryPlant<Simian::EntityComponent, GCPlayerLogic> GCPlayerLogic::factoryPlant_(
        &Simian::GameFactory::Instance().ComponentFactory(), GC_PLAYERLOGIC);

    // Pointer to myself
    GCPlayerLogic* GCPlayerLogic::player_;

    // Movement diagonal and rotation values
    const Simian::f32 MOVEMENT_DIAGONAL_RATIO = 0.7f;
    const Simian::f32 ROTATION_TIME_TOTAL = 0.2f;

    // Attacking values
    const Simian::f32 BULLET_START_HEIGHT = 0.2f;
    const Simian::f32 FLINCH_TIME_TOTAL = 0.1f;
    const Simian::f32 SHOOT_ANIM_TIME_TOTAL = 0.4f;

    // Sounds
    const Simian::u32 TOTAL_ATTACK_SOUNDS = 2;
    const Simian::u32 TOTAL_DODGE_SOUNDS = 3;
    const Simian::f32 RUN_SOUND_TIMER_MAX = 0.63f;
    const Simian::f32 CAST_SOUND_TIMER_MAX = 2.0f;
    const Simian::f32 HEARTBEAT_SOUND_TIMER_MAX = 3.0f;

    // Controls
    const Simian::u32 GCPlayerLogic::KEY_FORWARD = Simian::Keyboard::KEY_W;
    const Simian::u32 GCPlayerLogic::KEY_BACKWARD = Simian::Keyboard::KEY_S;
    const Simian::u32 GCPlayerLogic::KEY_LEFT = Simian::Keyboard::KEY_A;
    const Simian::u32 GCPlayerLogic::KEY_RIGHT = Simian::Keyboard::KEY_D;
    const Simian::u32 KEY_DODGE = Simian::Keyboard::KEY_SPACE;
    const Simian::u32 KEY_ATTACK = Simian::Mouse::KEY_LBUTTON;
    const Simian::u32 KEY_BLOCK = Simian::Keyboard::KEY_MBUTTON;
    const Simian::u32 KEY_SHOOT = Simian::Keyboard::KEY_O; //KEY_RBUTTON
    const Simian::u32 KEY_RELOAD = Simian::Keyboard::KEY_R;
    const Simian::u32 KEY_ENTERSPELLCASTING = Simian::Keyboard::KEY_RBUTTON; //KEY_CONTROL

    // Camera
    const Simian::f32 SPRING_CONSTANT = 20.0f;
    const Simian::f32 DAMP_CONSTANT = 10.0f;
    const Simian::f32 MASS_CONSTANT = 10.0f;
	const Simian::f32 EASE_FACTOR = 5.0f;

    // Teleporting Effect
    const Simian::f32 TELEPORT_MIN_SQUASH = 0.0f;
    const Simian::f32 TELEPORT_MAX_STRETCH = 0.11f;
    const Simian::f32 TELEPORT_DURATION = 0.125f;
    const Simian::f32 TELEPORT_ORIGINAL_SCALE = 0.055f;
    const Simian::f32 TELEPORTED_MAX_TIME = 1.2f;

    // Swirl Variables
    const Simian::f32 SWIRL_SPEED = 5.0f;
    const Simian::f32 SWIRL_DISTANCE = 0.7f;

    // Revive Variables
    const Simian::f32 REVIVE_TIMER_MAX = 2.0f;

    GCPlayerLogic::GCPlayerLogic()
        : healthLogger_(0),
          posLogger_(0),
          transform_(0),
          parentTransform_(0),
          physics_(0),
          sounds_(0),
          swordTrail_(0),
          reviveParticles_(0),
          movementSpeed_(0),
          movementVec_(0),
          movementFlag_(0),
          model_(0),
          camera_(0),
          cameraTransform_(0),
          cameraBaseTransform_(0),
          cameraLocked_(true),
          playerControlOverride_(false),
          stateMachine_(PS_IDLE, PS_TOTAL),
          screenHalfX_(0),
          screenHalfY_(0),
          inSpellCasting_(false),
          dodgeEffective_(false),
          blockEffective_(false),
          rotForwardAngle_(0),
          rotSideAngle_(0),
          initialAngle_(0.0f),
          finalAngle_(0.0f),
          rotTimer_(0),
          dodgeTimer_(0),
          knockbackAmt_(0),
          attackTimer_(0),
          attackDashing_(false),
          attackDashedPrev_(false),
          hasAttacked_(false),
          manaEnterCasting_(5.0f),
          manaGradualCasting_(0.05f),
          manaDrawCasting_(2.0f),
          manaGradualRegen_(0.02f),
          attributes_(0),
          animationFrameRate_(48),
          movementSpeedDodge_(8.0f),
          dodgeTimeTotal_(0.6f),
          dodgeEffectiveTime_(0.4f),
          blockTimer_(0.0f),
          blockTimerUp_(0.42f),
          blockTimerMax_(1.42f),
          blockEffectiveTime_(1.0f),
          blockOver_(false),
          movementSpeedAttackDash_(4.0f),
          attackDashTimeTotal_(0.5f),
          attackMovementSpeed_(0.8f),
          attackTimeTotal_(0.75f),
		  attackDashDamageMultiplier_(1.5f),
          shootTimeTotal_(0.5f),
          attackHitDelay_(0.4f),
          attackMeleeRange_(0.75f),
          attackMeleeAngle_(40.0f),
          runSoundTimer_(0.0f),
          castSoundTimer_(0.0f),
          attackAnimationsTotal_(2),
          currentAttackAnimationNum_(1),
		  blockAngle_(35.0f),
          bulletsMax_(6),
          bulletsCurr_(bulletsMax_),
          reloadTimeCurr_(0.0f),
          reloadTimeMax_(1.5f),
          heartbeatLimit1_(0.5f),
          heartbeatLimit2_(0.35f),
          heartbeatLimit3_(0.25f),
          heartbeatLimit4_(0.15f),
          heartbeatSoundTimer_(HEARTBEAT_SOUND_TIMER_MAX),
          heartbeatState_(0),
          dyingEffectMaterial_(0),
          isReloading_(false),
		  rotateAtStartBool_(false),
          teleporting_(false),
          teleportTimer_(0.0f),
          teleportedAnimTimer_(-1.0f),
          teleportSquashFrom_(TELEPORT_ORIGINAL_SCALE),
          teleportSquashTo_(TELEPORT_MIN_SQUASH),
          teleportStretchFrom_(TELEPORT_ORIGINAL_SCALE),
          teleportStretchTo_(TELEPORT_MAX_STRETCH),
          reviveTimer_(0.0f),
          swirling_(false),
          swirlAmount_(0.0f),
          spirit_(0),
          levelupEffect_(0),
          deathRollover_(0)
    {
    }

    GCPlayerLogic::~GCPlayerLogic()
    {
    }

    //--------------------------------------------------------------------------

    Simian::CTransform& GCPlayerLogic::Transform()
    {
        return *transform_;
    }

    GCPhysicsController* GCPlayerLogic::Physics() const
    {
        return physics_;
    }

    GCPlayerLogic* GCPlayerLogic::Instance()
    {
        return player_;
    }

    GCAttributes* GCPlayerLogic::Attributes() const
    {
        return attributes_;
    }

    Simian::CCamera* GCPlayerLogic::Camera() const
    {
        return camera_;
    }

    bool GCPlayerLogic::CameraLocked() const
    {
        return cameraLocked_;
    }

    void GCPlayerLogic::CameraLocked( bool val )
    {
        cameraLocked_ = val;
    }

    bool GCPlayerLogic::PlayerControlOverride() const
    {
        return playerControlOverride_;
    }

    void GCPlayerLogic::PlayerControlOverride( bool val )
    {
        playerControlOverride_ = val;
        if (val && stateMachine_.CurrentState() != PS_DIE)
            stateMachine_.ChangeState(PS_CINE);
    }

    bool GCPlayerLogic::DodgeEffective() const
    {
        return dodgeEffective_;
    }

    void GCPlayerLogic::DodgeEffective( bool val )
    {
        dodgeEffective_ = val;
    }

    bool GCPlayerLogic::BlockEffective() const
    {
        return blockEffective_;
    }

    void GCPlayerLogic::BlockEffective( bool val )
    {
        blockEffective_ = val;
    }

    void GCPlayerLogic::RevivePlayer()
    {
        if (stateMachine_.CurrentState() != PS_REVIVE)
            stateMachine_.ChangeState(PS_REVIVE);
    }

    void GCPlayerLogic::TeleportedPlayer()
    {
        if (stateMachine_.CurrentState() != PS_TELEPORTED)
        {
            teleportedAnimTimer_ = TELEPORTED_MAX_TIME;
            stateMachine_.ChangeState(PS_TELEPORTED);
        }
    }

    Simian::CSoundEmitter* GCPlayerLogic::Sounds() const
    {
        return sounds_;
    }


    Simian::f32 GCPlayerLogic::ManaDrawCasting() const
    {
        return manaDrawCasting_;
    }

    void GCPlayerLogic::AddExperience(Simian::u32 val)
    {
        attributes_->Experience(attributes_->Experience()+val);
    }

    void GCPlayerLogic::ResetHeartBeatAndDyingEffect()
    {
        heartbeatState_ = 0;
        sounds_->Stop(S_HEARTBEAT_4);
        sounds_->Stop(S_HEARTBEAT_3);
        sounds_->Stop(S_HEARTBEAT_2);
        sounds_->Stop(S_HEARTBEAT_1);

        // update shader param
        Simian::f32 amount = 0.0f;
        if (dyingEffectAmount_)
            dyingEffectAmount_->SetFloat(&amount, 1);
    }

    //--------------------------------------------------------------------------

    void GCPlayerLogic::updateCamera_( Simian::DelegateParameter& param )
    {
        // don't update camera if it isn't locked
        if (!cameraLocked_)
            return;

		// interpolate camera back to player before allowing normal camera code 
		// to take control.
		if (cameraLocked_ && GCDescensionCamera::Instance()->Interpolating())
        {
            // camera is returning to me...
            GCDescensionCamera::Instance()->FinalPosition(transform_->World().Position());
            return;
        }

        Simian::EntityUpdateParameter* p;
        param.As(p);
		
		// spring based camera (has weird jerks :( )
#if 0
        // get direction to player
        Simian::Vector3 dir = parentTransform_->World().Position() - cameraBaseTransform_->World().Position();
		if (dir.Length() < Simian::Math::EPSILON)
			return;

        // set camera's force as a function of velocity
        cameraForce_ = dir * SPRING_CONSTANT - DAMP_CONSTANT * cameraVelocity_;
        
        // calculate velocity
        cameraVelocity_ = cameraForce_ * MASS_CONSTANT * p->Dt;

        // integrate velocity
        cameraBaseTransform_->Position(cameraBaseTransform_->World().Position() + cameraVelocity_ * p->Dt);
#endif

#if 1
		// stupid easing camera (surprisingly good :D)
		Simian::Vector3 pos = Simian::Interpolation::Interpolate(cameraBaseTransform_->WorldPosition(), 
			parentTransform_->WorldPosition(), Simian::Math::Min(EASE_FACTOR * p->Dt, 1.0f), 
			Simian::Interpolation::Linear);
		cameraBaseTransform_->Position(pos);
#endif

#if 0
		// instant snap camera (vanilla style ;) )
        cameraBaseTransform_->Position(transform_->World().Position());
#endif

		cameraTransform_->RecomputeWorld();
    }

    void GCPlayerLogic::update_( Simian::DelegateParameter& param )
    {
#ifdef _DESCENDED
        if (GCEditor::EditorMode())
            return;
#endif
        Simian::f32 dt = param.As<Simian::EntityUpdateParameter>().Dt;
        // Hack debug text
        /*std::stringstream sstream;
		sstream << transform_->Axis().Y() << " " << transform_->Angle().Degrees() << " " << parentTransform_->Angle().Degrees();
        DebugRendererDrawText(sstream.str(), Simian::Vector2(screenHalfX_-(screenHalfX_/5), screenHalfY_-4*(screenHalfY_/8)), Simian::Color(1,1,1,1));*/
		
		// Rotate angle at start
        rotateAtStart_();

        // update teleporting effect
        updateTeleportEffect_(dt);

        updateSpiritSwirl_(dt);

        if (GCUI::GameStatus() == GCUI::GAME_PAUSED)
        {
            stateMachine_.ChangeState(PS_IDLE);
            movementFlag_ = 0;
            movementSpeed_ = 0.0f;
            movementVec_ = Simian::Vector3::Zero;
            return;
        }

        if (playerControlOverride_ || GCUpgradeExchange::Exchanging())
        {
            if (stateMachine_.CurrentState() != PS_DIE && stateMachine_.CurrentState() != PS_CINE)
            {
                updateInput_();
                stateMachine_.ChangeState(PS_CINE);
            }
            if (stateMachine_.CurrentState() != PS_DIE)
                stateMachine_.Update(dt);
            return;
        }

        if (attributes_->Health() <= 0 && stateMachine_.CurrentState() != PS_DIE)
            stateMachine_.ChangeState(PS_DIE);

        // Calculate direction angles
        calcDirAngles_();
        // Update rotation
        updateRotation_(dt);
        // Update spell casting
        updateSpellCasting_(dt);
        // Update gun reloading
        if (isReloading_)
            updateGunReloading_(dt);
        // Update heartbeat sound
        updateHeartBeatSound_();
        // Update the dying compositor
        updateDyingEffect_();

        // Update state machine and physics
        stateMachine_.Update(dt);
        updatePhysics_();
    }

    void GCPlayerLogic::updateInput_()
    {
        if (stateMachine_.CurrentState() != PS_SHOOT)
        {
            // Moving forward
            if (Simian::Keyboard::IsPressed(KEY_FORWARD) && !Simian::Keyboard::IsPressed(KEY_BACKWARD))
            {
                if (!SIsFlagSet(movementFlag_, FLAG_MOVEMENT_BACKWARD))
                {
                    SFlagSet(movementFlag_, FLAG_MOVEMENT_FORWARD);
                    initialAngle_ = transform_->Angle().Degrees();
                    rotTimer_ = 0.0f;
                    if (SIsFlagSet(movementFlag_, FLAG_MOVEMENT_LEFT))
                        setFinalRotationAngle_(rotSideAngle_+315.0f);
                    else if (SIsFlagSet(movementFlag_, FLAG_MOVEMENT_RIGHT))
                        setFinalRotationAngle_(rotSideAngle_+225.0f);
                    else
                        setFinalRotationAngle_(rotForwardAngle_);
                }
                if (stateMachine_.CurrentState() == PS_IDLE && !playerControlOverride_
                    && !GCUpgradeExchange::Exchanging())
                    stateMachine_.ChangeState(PS_RUN);
            }
            // Moving backward
            if (Simian::Keyboard::IsPressed(KEY_BACKWARD) && !Simian::Keyboard::IsPressed(KEY_FORWARD))
            {
                if (!SIsFlagSet(movementFlag_, FLAG_MOVEMENT_FORWARD))
                {
                    SFlagSet(movementFlag_, FLAG_MOVEMENT_BACKWARD);
                    initialAngle_ = transform_->Angle().Degrees();
                    rotTimer_ = 0.0f;
                    if (SIsFlagSet(movementFlag_, FLAG_MOVEMENT_LEFT))
                        setFinalRotationAngle_(rotSideAngle_+45.0f);
                    else if (SIsFlagSet(movementFlag_, FLAG_MOVEMENT_RIGHT))
                        setFinalRotationAngle_(rotSideAngle_+135.0f);
                    else
                        setFinalRotationAngle_(rotForwardAngle_+180.0f);
                }
                if (stateMachine_.CurrentState() == PS_IDLE && !playerControlOverride_
                    && !GCUpgradeExchange::Exchanging())
                    stateMachine_.ChangeState(PS_RUN);
            }
            // Moving left
            if (Simian::Keyboard::IsPressed(KEY_LEFT) && !Simian::Keyboard::IsPressed(KEY_RIGHT))
            {
                if (!SIsFlagSet(movementFlag_, FLAG_MOVEMENT_RIGHT))
                {
                    SFlagSet(movementFlag_, FLAG_MOVEMENT_LEFT);
                    initialAngle_ = transform_->Angle().Degrees();
                    rotTimer_ = 0.0f;
                    if (SIsFlagSet(movementFlag_, FLAG_MOVEMENT_FORWARD))
                        setFinalRotationAngle_(rotSideAngle_+315.0f);
                    else if (SIsFlagSet(movementFlag_, FLAG_MOVEMENT_BACKWARD))
                        setFinalRotationAngle_(rotSideAngle_+45.0f);
                    else
                        setFinalRotationAngle_(rotSideAngle_);
                }
                if (stateMachine_.CurrentState() == PS_IDLE && !playerControlOverride_
                    && !GCUpgradeExchange::Exchanging())
                    stateMachine_.ChangeState(PS_RUN);
            }
            // Moving right
            if (Simian::Keyboard::IsPressed(KEY_RIGHT) && !Simian::Keyboard::IsPressed(KEY_LEFT))
            {
                if (!SIsFlagSet(movementFlag_, FLAG_MOVEMENT_LEFT))
                {
                    SFlagSet(movementFlag_, FLAG_MOVEMENT_RIGHT);
                    initialAngle_ = transform_->Angle().Degrees();
                    rotTimer_ = 0.0f;
                    if (SIsFlagSet(movementFlag_, FLAG_MOVEMENT_FORWARD))
                        setFinalRotationAngle_(rotSideAngle_+225.0f);
                    else if (SIsFlagSet(movementFlag_, FLAG_MOVEMENT_BACKWARD))
                        setFinalRotationAngle_(rotSideAngle_+135.0f);
                    else
                        setFinalRotationAngle_(rotSideAngle_+180.0f);
                }
                if (stateMachine_.CurrentState() == PS_IDLE && !playerControlOverride_
                    && !GCUpgradeExchange::Exchanging())
                    stateMachine_.ChangeState(PS_RUN);
            }
            // Melee Attack
            if (Simian::Mouse::IsTriggered(KEY_ATTACK) && !inSpellCasting_ && stateMachine_.CurrentState() != PS_ATTACK
                                                                           && stateMachine_.CurrentState() != PS_DODGE
                                                                           && !GCUpgradeExchange::Exchanging())
            {
                stateMachine_.ChangeState(PS_IDLE);
                stateMachine_.ChangeState(PS_ATTACK);
            }

            // Dodge
            if (Simian::Keyboard::IsTriggered(KEY_DODGE) && stateMachine_.CurrentState() != PS_DODGE
                    && !GCUpgradeExchange::Exchanging())
                stateMachine_.ChangeState(PS_DODGE);

            // Block
            if (Simian::Keyboard::IsTriggered(KEY_BLOCK)
                && stateMachine_.CurrentState() != PS_BLOCK && stateMachine_.CurrentState() != PS_DODGE
                    && !GCUpgradeExchange::Exchanging())
                stateMachine_.ChangeState(PS_BLOCK);

            // Reload
            /*if (Simian::Keyboard::IsTriggered(KEY_RELOAD))
            {
            if (bulletsCurr_ < bulletsMax_)
            {
            isReloading_ = true;
            reloadTimeCurr_ = reloadTimeMax_;
            }
            }*/
        }

        // Shooting Attack
        /*if (Simian::Mouse::IsTriggered(KEY_SHOOT) && !isReloading_)
        {
            // Able to shoot if not in shooting state
            if (stateMachine_.CurrentState() != PS_SHOOT)
                stateMachine_.ChangeState(PS_SHOOT);
            // Or if attack timer is more than shooting interval
            else if (attackTimer_ > shootTimeTotal_)
                stateMachine_.ChangeState(PS_SHOOT);
        }*/

        // Stop moving
        if (stateMachine_.CurrentState() == PS_RUN || stateMachine_.CurrentState() == PS_ATTACK)
        {
            // Stop moving forward
            if (Simian::Keyboard::IsReleased(KEY_FORWARD))
            {
                SFlagUnset(movementFlag_, FLAG_MOVEMENT_FORWARD);
                if (SIsFlagSet(movementFlag_, FLAG_MOVEMENT_LEFT))
                    setFinalRotationAngle_(rotSideAngle_);
                else if (SIsFlagSet(movementFlag_, FLAG_MOVEMENT_RIGHT))
                    setFinalRotationAngle_(rotSideAngle_+180.0f);
            }
            // Stop moving backward
            if (Simian::Keyboard::IsReleased(KEY_BACKWARD))
            {
                SFlagUnset(movementFlag_, FLAG_MOVEMENT_BACKWARD);
                if (SIsFlagSet(movementFlag_, FLAG_MOVEMENT_LEFT))
                    setFinalRotationAngle_(rotSideAngle_);
                else if (SIsFlagSet(movementFlag_, FLAG_MOVEMENT_RIGHT))
                    setFinalRotationAngle_(rotSideAngle_+180.0f);
            }
            // Stop moving left
            if (Simian::Keyboard::IsReleased(KEY_LEFT))
            {
                SFlagUnset(movementFlag_, FLAG_MOVEMENT_LEFT);
                if (SIsFlagSet(movementFlag_, FLAG_MOVEMENT_FORWARD))
                    setFinalRotationAngle_(rotForwardAngle_);
                else if (SIsFlagSet(movementFlag_, FLAG_MOVEMENT_BACKWARD))
                    setFinalRotationAngle_(rotForwardAngle_+180.0f);
            }
            // Stop moving right
            if (Simian::Keyboard::IsReleased(KEY_RIGHT))
            {
                SFlagUnset(movementFlag_, FLAG_MOVEMENT_RIGHT);
                if (SIsFlagSet(movementFlag_, FLAG_MOVEMENT_FORWARD))
                    setFinalRotationAngle_(rotForwardAngle_);
                else if (SIsFlagSet(movementFlag_, FLAG_MOVEMENT_BACKWARD))
                    setFinalRotationAngle_(rotForwardAngle_+180.0f);
            }
            if (!SIsFlagSet(movementFlag_, FLAG_MOVEMENT_FORWARD) &&
                !SIsFlagSet(movementFlag_, FLAG_MOVEMENT_BACKWARD) &&
                !SIsFlagSet(movementFlag_, FLAG_MOVEMENT_RIGHT) &&
                !SIsFlagSet(movementFlag_, FLAG_MOVEMENT_LEFT) &&
                stateMachine_.CurrentState() != PS_ATTACK)
                stateMachine_.ChangeState(PS_IDLE);
        }
    }

    // Updating movement through input
    void GCPlayerLogic::updateMovement_()
    {
        movementVec_ = Simian::Vector3::Zero;

        // Rotating character directly after button press
        int numPresses = 0;
        if (SIsFlagSet(movementFlag_, FLAG_MOVEMENT_FORWARD))
        {
            ++numPresses;
            movementVec_.Z(movementVec_.Z()+movementSpeed_);
        }
        if (SIsFlagSet(movementFlag_, FLAG_MOVEMENT_BACKWARD))
        {
            ++numPresses;
            movementVec_.Z(movementVec_.Z()-movementSpeed_);
        }
        if (SIsFlagSet(movementFlag_, FLAG_MOVEMENT_LEFT))
        {
            ++numPresses;
            movementVec_.X(movementVec_.X()-movementSpeed_);
        }
        if (SIsFlagSet(movementFlag_, FLAG_MOVEMENT_RIGHT))
        {
            ++numPresses;
            movementVec_.X(movementVec_.X()+movementSpeed_);
        }
        
        movementVec_ = movementVec_.Z()*vecForward_ + movementVec_.X()*vecSide_;
        if (numPresses > 1)
            movementVec_ *= MOVEMENT_DIAGONAL_RATIO;
    }

    void GCPlayerLogic::updateRotation_(Simian::f32 dt)
    {
        // Wrap angles
        finalAngle_ = Simian::Math::WrapAngle(Simian::Degree(finalAngle_)).Degrees();
        initialAngle_ = Simian::Math::WrapAngle(Simian::Degree(initialAngle_)).Degrees();
        // Interpolate to direction
        rotTimer_ += dt;
        Simian::f32 intp = rotTimer_/ROTATION_TIME_TOTAL;
        intp = Simian::Math::Clamp(intp, 0.0f, 1.0f);
        // Ignore huge angles
        if (std::abs(finalAngle_-initialAngle_) > 145.0f)
        {
            finalAngle_ -= 360.0f;
            transform_->Rotation(Simian::Vector3(0, -1, 0), Simian::Degree(finalAngle_) );
            return;
        }
        Simian::f32 finalAngle = Simian::Interpolation::Interpolate<Simian::f32>(initialAngle_,finalAngle_,intp,Simian::Interpolation::EaseCubicOut);
        transform_->Rotation(Simian::Vector3(0, -1, 0), Simian::Degree(finalAngle) );
    }

    // Update spell casting
    void GCPlayerLogic::updateSpellCasting_(Simian::f32 dt)
    {
        if (stateMachine_.CurrentState() == PS_DIE)
        {
            sounds_->Stop(S_CAST_LOOP);
            return;
        }

        // Spell Casting
        if (GCGestureInterpreter::Instance())
        {
            if (Simian::Keyboard::IsPressed(KEY_ENTERSPELLCASTING) && attributes_->Mana() > manaGradualCasting_ && stateMachine_.CurrentState() != PS_DIE)
            {
                if (!inSpellCasting_)
                {
                    inSpellCasting_ = true;
                    // Decrement an amount of mana when entering casting mode
                    attributes_->Mana(attributes_->Mana() - manaEnterCasting_);
                    castSoundTimer_ = CAST_SOUND_TIMER_MAX;
                }
                else
                {
                    // Slowly decrement mana in casting mode
                    attributes_->Mana(attributes_->Mana()
                                     -(manaGradualCasting_*Simian::Game::Instance().FrameTime()));
                    // Play sound
                    castSoundTimer_ += dt;
                    if (castSoundTimer_ > CAST_SOUND_TIMER_MAX)
                    {
                        castSoundTimer_ = 0.0f;
                        sounds_->Play(S_CAST_LOOP);
                    }
                }
            }
            else
            {
                if (inSpellCasting_)
                {
                    inSpellCasting_ = false;
                    sounds_->Stop(S_CAST_LOOP);
                }

                // Slowly regenerate mana in non-casting mode
                if (attributes_->Mana() < attributes_->MaxMana())
                    attributes_->Mana(attributes_->Mana()
                                        +(manaGradualRegen_*Simian::Game::Instance().FrameTime()));
                else
                    attributes_->Mana(attributes_->MaxMana());
            }
        }
    }

    // Update gun reloading
    void GCPlayerLogic::updateGunReloading_(Simian::f32 dt)
    {
        if (reloadTimeCurr_ > Simian::Math::EPSILON)
            reloadTimeCurr_ -= dt;
        else if (bulletsCurr_ < bulletsMax_)
        {
            reloadTimeCurr_ = 0.0f;
            bulletsCurr_ = bulletsMax_;
            isReloading_ = false;
        }
    }

    // Update heartbeat sound
    void GCPlayerLogic::updateHeartBeatSound_()
    {
        // Play sound only if health below limit
        Simian::f32 healthRatio = static_cast<Simian::f32>(attributes_->Health())
                                   /static_cast<Simian::f32>(attributes_->MaxHealth());

        if (healthRatio < heartbeatLimit1_ && healthRatio > Simian::Math::EPSILON)
        {
            heartbeatSoundTimer_ += Simian::Game::Instance().FrameTime();
            
            if (heartbeatSoundTimer_ > HEARTBEAT_SOUND_TIMER_MAX && heartbeatState_ != 0)
            {
                heartbeatSoundTimer_ = 0.0f;
                sounds_->Play(S_HEARTBEAT_1 + heartbeatState_ - 1);
            }

            if (heartbeatState_ == 0 )
            {
                sounds_->Play(S_HEARTBEAT_1);
                heartbeatState_ = 1;
            }
            else if (healthRatio < heartbeatLimit4_)
            {
                if (heartbeatState_ != 4)
                {
                    sounds_->Play(S_HEARTBEAT_4);
                    heartbeatState_ = 4;

                    sounds_->Stop(S_HEARTBEAT_3);
                    sounds_->Stop(S_HEARTBEAT_2);
                    sounds_->Stop(S_HEARTBEAT_1);
                }
            }
            else if (healthRatio < heartbeatLimit3_)
            {
                if (heartbeatState_ != 3)
                {
                    sounds_->Play(S_HEARTBEAT_3);
                    heartbeatState_ = 3;

                    sounds_->Stop(S_HEARTBEAT_4);
                    sounds_->Stop(S_HEARTBEAT_2);
                    sounds_->Stop(S_HEARTBEAT_1);
                }
            }
            else if (healthRatio < heartbeatLimit2_)
            {
                if (heartbeatState_ != 2)
                {
                    sounds_->Play(S_HEARTBEAT_2);
                    heartbeatState_ = 2;

                    sounds_->Stop(S_HEARTBEAT_4);
                    sounds_->Stop(S_HEARTBEAT_3);
                    sounds_->Stop(S_HEARTBEAT_1);
                }
            }
            else if (heartbeatState_ != 1)
            {
                sounds_->Play(S_HEARTBEAT_1);
                heartbeatState_ = 1;

                sounds_->Stop(S_HEARTBEAT_4);
                sounds_->Stop(S_HEARTBEAT_3);
                sounds_->Stop(S_HEARTBEAT_2);
            }
        }
        else if (heartbeatState_ != 0)
        {
            heartbeatState_ = 0;
            sounds_->Stop(S_HEARTBEAT_4);
            sounds_->Stop(S_HEARTBEAT_3);
            sounds_->Stop(S_HEARTBEAT_2);
            sounds_->Stop(S_HEARTBEAT_1);
        }
    }

    void GCPlayerLogic::updateDyingEffect_()
    {
        Simian::f32 healthRatio = static_cast<Simian::f32>(attributes_->Health())/
            static_cast<Simian::f32>(attributes_->MaxHealth());

        // amount of color to interpolate
        Simian::f32 amount = (healthRatio - heartbeatLimit1_)/(heartbeatLimit4_ - heartbeatLimit1_);
        // clamp from 0 to 1
        amount = Simian::Math::Clamp(amount, 0.0f, 1.0f);

        // update shader param
        if (dyingEffectAmount_)
            dyingEffectAmount_->SetFloat(&amount, 1);
    }

    void GCPlayerLogic::updateTeleportEffect_(Simian::f32 dt)
    {
        if (teleporting_)
        {
            teleportTimer_ += dt;
            Simian::f32 intp = teleportTimer_/TELEPORT_DURATION;
            intp = Simian::Math::Clamp(intp, 0.0f, 1.0f);

            Simian::f32 squash = Simian::Interpolation::Interpolate(teleportSquashFrom_, teleportSquashTo_, intp, Simian::Interpolation::EaseSquareOut);
            Simian::f32 stretch = Simian::Interpolation::Interpolate(teleportStretchFrom_, teleportStretchTo_, intp, Simian::Interpolation::EaseSquareOut);

            transform_->Scale(Simian::Vector3(squash, stretch, squash));
            attributes_->ModelColor()->Color(Simian::Color(intp, intp, intp, 1.0f));
        }
    }

    void GCPlayerLogic::updatePhysics_()
    {
        physics_->Velocity(movementVec_);
    }

    void GCPlayerLogic::updateSpiritSwirl_( Simian::f32 dt )
    {
        if (!swirling_ || !spirit_)
            return;

        if (spirit_->Active())
        {
            spirit_->Active(false);
            spirit_->InterpolateToPos(true);
        }

        swirlAmount_ += SWIRL_SPEED * dt;
        Simian::Vector3 dir(std::cos(swirlAmount_), 0.5f, std::sin(swirlAmount_));
        dir *= SWIRL_DISTANCE;
        
        spirit_->SetTargetPos(transform_->WorldPosition() + dir);
    }

    // State machine functions
    void GCPlayerLogic::idleOnEnter_(Simian::DelegateParameter&)
    {
        model_->Controller().PlayAnimation("Idle", true, animationFrameRate_);
        movementFlag_ = 0;
        movementSpeed_ = 0.0f;
        movementVec_ = Simian::Vector3::Zero;
        sounds_->Stop(S_RUN);

        attackDashedPrev_ = false;
    }

    void GCPlayerLogic::idleOnUpdate_(Simian::DelegateParameter&)
    {
        updateInput_();

        // Change to cine if player control overriden
        if (playerControlOverride_)
            stateMachine_.ChangeState(PS_CINE);
    }

    void GCPlayerLogic::runOnEnter_(Simian::DelegateParameter&)
    {
        model_->Controller().PlayAnimation("Run", true, static_cast<Simian::u32>(animationFrameRate_*0.6f));
        movementSpeed_ = attributes_->MovementSpeed();
        sounds_->Stop(S_RUN);
        runSoundTimer_ = RUN_SOUND_TIMER_MAX;
    }

    void GCPlayerLogic::runOnUpdate_(Simian::DelegateParameter& param)
    {
        Simian::f32 dt = param.As<Simian::EntityUpdateParameter>().Dt;
        
        // Change to cine if player control overriden
        if (playerControlOverride_)
            stateMachine_.ChangeState(PS_CINE);

        updateInput_();
        updateMovement_();

        // Play sound
        runSoundTimer_ += dt;
        if (runSoundTimer_ > RUN_SOUND_TIMER_MAX)
        {
            runSoundTimer_ = 0.0f;
            sounds_->Play(S_RUN);
        }
    }

    void GCPlayerLogic::attackOnEnter_(Simian::DelegateParameter&)
    {
       helperStartAttack_();
    }

    void GCPlayerLogic::attackOnUpdate_(Simian::DelegateParameter& param)
    {
        Simian::f32 dt = param.As<Simian::EntityUpdateParameter>().Dt;

        // Change to cine if player control overriden
        if (playerControlOverride_)
            stateMachine_.ChangeState(PS_CINE);

        // Set movement vector
        if (movementFlag_)
            movementVec_ = Simian::Vector3(-std::sin(transform_->Angle().Radians()),
                            0, std::cos(transform_->Angle().Radians())) * movementSpeed_;

        // Change speed of attacking
        attackTimer_ += dt;

        // Attack detection
        if (attackTimer_ > attackHitDelay_ && !hasAttacked_)
        {
            hasAttacked_ = true;
            meleeAttackDetection_();
        }
        
        movementSpeed_ = attackMovementSpeed_;

        if (attackDashing_)
        {
            // Interpolate attack dash speed
            Simian::f32 intp = attackTimer_/attackDashTimeTotal_;
            intp = Simian::Math::Clamp(intp, 0.0f, 1.0f);
            movementSpeed_ = Simian::Interpolation::Interpolate<Simian::f32>(movementSpeedAttackDash_,
                                                0.0f, intp, Simian::Interpolation::EaseCubicOut);
            if (intp >= 0.8)
                attackDashing_ = false;
        }
        else
            updateInput_();
        // If attack is over
        if (!attackDashing_ && attackTimer_ > attackHitDelay_)
        {
            if (Simian::Keyboard::IsTriggered(KEY_ATTACK))
            {
                movementFlag_ = 0;
                movementSpeed_ = 0.0f;
                movementVec_ = Simian::Vector3::Zero;
                helperStartAttack_();
            }

            if (attackTimer_ >attackTimeTotal_)
            {
                currentAttackAnimationNum_ = 1;
                // If no longer holding on to forward
                if (!Simian::Keyboard::IsPressed(KEY_FORWARD))
                {
                    SFlagUnset(movementFlag_, FLAG_MOVEMENT_FORWARD);
                    if (SIsFlagSet(movementFlag_, FLAG_MOVEMENT_LEFT))
                        setFinalRotationAngle_(rotSideAngle_);
                    else if (SIsFlagSet(movementFlag_, FLAG_MOVEMENT_RIGHT))
                        setFinalRotationAngle_(rotSideAngle_+180.0f);
                }
                // If no longer holding on to backward
                if (!Simian::Keyboard::IsPressed(KEY_BACKWARD))
                {
                    SFlagUnset(movementFlag_, FLAG_MOVEMENT_BACKWARD);
                    if (SIsFlagSet(movementFlag_, FLAG_MOVEMENT_LEFT))
                        setFinalRotationAngle_(rotSideAngle_);
                    else if (SIsFlagSet(movementFlag_, FLAG_MOVEMENT_RIGHT))
                        setFinalRotationAngle_(rotSideAngle_+180.0f);
                }
                // If no longer holding on to left
                if (!Simian::Keyboard::IsPressed(KEY_LEFT))
                {
                    SFlagUnset(movementFlag_, FLAG_MOVEMENT_LEFT);
                    DManualLog(posLogger_);
                    if (SIsFlagSet(movementFlag_, FLAG_MOVEMENT_FORWARD))
                        setFinalRotationAngle_(rotForwardAngle_);
                    else if (SIsFlagSet(movementFlag_, FLAG_MOVEMENT_BACKWARD))
                        setFinalRotationAngle_(rotForwardAngle_+180.0f);
                }
                // If no longer holding on to right
                if (!Simian::Keyboard::IsPressed(KEY_RIGHT))
                {
                    SFlagUnset(movementFlag_, FLAG_MOVEMENT_RIGHT);
                    if (SIsFlagSet(movementFlag_, FLAG_MOVEMENT_FORWARD))
                        setFinalRotationAngle_(rotForwardAngle_);
                    else if (SIsFlagSet(movementFlag_, FLAG_MOVEMENT_BACKWARD))
                        setFinalRotationAngle_(rotForwardAngle_+180.0f);
                }
                
                if (!SIsFlagSet(movementFlag_, FLAG_MOVEMENT_FORWARD) &&
                    !SIsFlagSet(movementFlag_, FLAG_MOVEMENT_BACKWARD) &&
                    !SIsFlagSet(movementFlag_, FLAG_MOVEMENT_RIGHT) &&
                    !SIsFlagSet(movementFlag_, FLAG_MOVEMENT_LEFT))
                    stateMachine_.ChangeState(PS_IDLE);
                else
                    stateMachine_.ChangeState(PS_RUN);
            }
        }
        
    }

    void GCPlayerLogic::attackOnExit_(Simian::DelegateParameter&)
    {
       swordTrail_->Enabled(false);
    }

    void GCPlayerLogic::shootOnEnter_(Simian::DelegateParameter&)
    {
        // Disable all movementFlag
        movementFlag_ = 0;
        movementVec_ = Simian::Vector3::Zero;

        // Picking a point of intersection
        Simian::Vector3 intersectionPt;
        Simian::Vector2 mouseVec = Simian::Mouse::GetMouseNormalizedPosition();
        if (fabs(mouseVec.X()) < 1 && fabs(mouseVec.Y()) < 1 &&
            Simian::Plane::Z.Intersect(
            Simian::Ray::FromProjection(cameraTransform_->World(), 
            camera_->Camera().Projection(), mouseVec),
            intersectionPt))
        {
            Simian::Vector3 attackVec(Simian::f32(intersectionPt.X()-parentTransform_->Position().X()),
                        0, Simian::f32(intersectionPt.Z()-parentTransform_->Position().Z()));
            attackVec.Normalize();
            // Get angle of rotation in degrees for attack vector
            Simian::f32 attackAngle = -vectorToDegreeFloat_(attackVec)-90.0f;

            // Get final angle from attack angle
            setFinalRotationAngle_(attackAngle);
        }

        initialAngle_ = finalAngle_;

        // Play shoot animation
        model_->Controller().PlayAnimation("Shoot", false, animationFrameRate_);
        attackTimer_ = 0.0f;

        // Fire bullet
        --bulletsCurr_;
        if (bulletsCurr_ <= 0)
        {
            isReloading_ = true;
            reloadTimeCurr_ = reloadTimeMax_;
        }
        createBullet_(finalAngle_);
    }

    void GCPlayerLogic::shootOnUpdate_(Simian::DelegateParameter& param)
    {        
        Simian::f32 dt = param.As<Simian::EntityUpdateParameter>().Dt;
        updateInput_();
        
        // Change speed of attacking
        attackTimer_ += dt;
        // If attack is over
        if (attackTimer_ > SHOOT_ANIM_TIME_TOTAL)
        {
            if (!Simian::Keyboard::IsPressed(KEY_FORWARD))
                SFlagUnset(movementFlag_, FLAG_MOVEMENT_FORWARD);
            if (!Simian::Keyboard::IsPressed(KEY_BACKWARD))
                SFlagUnset(movementFlag_, FLAG_MOVEMENT_BACKWARD);
            if (!Simian::Keyboard::IsPressed(KEY_LEFT))
                SFlagUnset(movementFlag_, FLAG_MOVEMENT_LEFT);
            if (!Simian::Keyboard::IsPressed(KEY_RIGHT))
                SFlagUnset(movementFlag_, FLAG_MOVEMENT_RIGHT);

            if (!SIsFlagSet(movementFlag_, FLAG_MOVEMENT_FORWARD) &&
                !SIsFlagSet(movementFlag_, FLAG_MOVEMENT_BACKWARD) &&
                !SIsFlagSet(movementFlag_, FLAG_MOVEMENT_RIGHT) &&
                !SIsFlagSet(movementFlag_, FLAG_MOVEMENT_LEFT))
                stateMachine_.ChangeState(PS_IDLE);
            else
                stateMachine_.ChangeState(PS_RUN);
        }
    }

    void GCPlayerLogic::dodgeOnEnter_(Simian::DelegateParameter&)
    {
        model_->Controller().PlayAnimation("Dodge", false, static_cast<Simian::u32>(animationFrameRate_*1.1f));
        movementSpeed_ = movementSpeedDodge_;
        dodgeTimer_ = 0.0f;
        // Roll through enemies!
        physics_->UnsetCollisionLayer(Collision::COLLISION_L1);

        // Play random sound
        if (sounds_)
            sounds_->Play(Simian::Math::Random(S_DODGE_1, S_DODGE_1 + TOTAL_DODGE_SOUNDS));
    }

    void GCPlayerLogic::dodgeOnUpdate_(Simian::DelegateParameter& param)
    {
        Simian::f32 dt = param.As<Simian::EntityUpdateParameter>().Dt;
        updateInput_();

        // Change to cine if player control overriden
        if (playerControlOverride_)
            stateMachine_.ChangeState(PS_CINE);

        // Change speed of dodging
        dodgeTimer_ += dt;
        Simian::f32 intp = dodgeTimer_/dodgeTimeTotal_;
        intp = Simian::Math::Clamp(intp, 0.0f, 1.0f);

        // Update dodge effective bool
        dodgeEffective_ = dodgeTimer_ < dodgeEffectiveTime_ ? true : false;

        movementSpeed_ = Simian::Interpolation::Interpolate<Simian::f32>(movementSpeedDodge_,
                                        0.0f, intp, Simian::Interpolation::EaseCubicOut);
        movementVec_ = Simian::Vector3(-std::sin(transform_->Angle().Radians()),
            0, std::cos(transform_->Angle().Radians())) * movementSpeed_;

        if (intp >= 0.8)
        {
            // Set collision back with enemies
            physics_->SetCollisionLayer(Collision::COLLISION_L1);
            if (!Simian::Keyboard::IsPressed(KEY_FORWARD))
                SFlagUnset(movementFlag_, FLAG_MOVEMENT_FORWARD);
            if (!Simian::Keyboard::IsPressed(KEY_BACKWARD))
                SFlagUnset(movementFlag_, FLAG_MOVEMENT_BACKWARD);
            if (!Simian::Keyboard::IsPressed(KEY_LEFT))
                SFlagUnset(movementFlag_, FLAG_MOVEMENT_LEFT);
            if (!Simian::Keyboard::IsPressed(KEY_RIGHT))
                SFlagUnset(movementFlag_, FLAG_MOVEMENT_RIGHT);

            if (!SIsFlagSet(movementFlag_, FLAG_MOVEMENT_FORWARD) &&
                !SIsFlagSet(movementFlag_, FLAG_MOVEMENT_BACKWARD) &&
                !SIsFlagSet(movementFlag_, FLAG_MOVEMENT_RIGHT) &&
                !SIsFlagSet(movementFlag_, FLAG_MOVEMENT_LEFT))
                stateMachine_.ChangeState(PS_IDLE);
            else
                stateMachine_.ChangeState(PS_RUN);
        }
    }

    void GCPlayerLogic::dodgeOnExit_(Simian::DelegateParameter&)
    {
        // Set collision back with enemies
        physics_->SetCollisionLayer(Collision::COLLISION_L1);
        dodgeEffective_ = false;
    }

    void GCPlayerLogic::blockOnEnter_(Simian::DelegateParameter&)
    {
        // If not moving, face direction of mouse
        if (!movementFlag_)
        {
            // Picking a point of intersection
            Simian::Vector3 intersectionPt;
            Simian::Vector2 mouseVec = Simian::Mouse::GetMouseNormalizedPosition();
            if (fabs(mouseVec.X()) < 1 && fabs(mouseVec.Y()) < 1 &&
                Simian::Plane::Z.Intersect(
                Simian::Ray::FromProjection(cameraTransform_->World(), 
                camera_->Camera().Projection(), mouseVec),
                intersectionPt))
            {
                Simian::Vector3 blockVec(Simian::f32(intersectionPt.X()-parentTransform_->Position().X()),
                    0, Simian::f32(intersectionPt.Z()-parentTransform_->Position().Z()));
                blockVec.Normalize();
                // Get angle of rotation in degrees for attack vector
                Simian::f32 blockAngle = -vectorToDegreeFloat_(blockVec)-90.0f;

                // Get final angle from attack angle
                setFinalRotationAngle_(blockAngle);
            }
        }

        model_->Controller().PlayAnimation("Block Up", false);
        movementFlag_ = 0;
        movementSpeed_ = 0.0f;
        movementVec_ = Simian::Vector3::Zero;
        sounds_->Stop(S_RUN);

        blockTimer_ = 0.0f;
        blockOver_ = false;
    }

    void GCPlayerLogic::blockOnUpdate_(Simian::DelegateParameter& param)
    {
        Simian::f32 dt = param.As<Simian::EntityUpdateParameter>().Dt;
        
        // Change speed of dodging
        blockTimer_ += dt;

        // Update dodge effective bool
        blockEffective_ = blockTimer_ < blockEffectiveTime_ ? true : false;

        if (!blockOver_ && blockTimer_ > blockTimerUp_)
        {
            model_->Controller().PlayAnimation("Block Down", false);
            blockOver_ = true;
        }
        else if (blockTimer_ > blockTimerMax_)
        {
            // If no longer holding on to forward
            if (!Simian::Keyboard::IsPressed(KEY_FORWARD))
            {
                SFlagUnset(movementFlag_, FLAG_MOVEMENT_FORWARD);
                if (SIsFlagSet(movementFlag_, FLAG_MOVEMENT_LEFT))
                    setFinalRotationAngle_(rotSideAngle_);
                else if (SIsFlagSet(movementFlag_, FLAG_MOVEMENT_RIGHT))
                    setFinalRotationAngle_(rotSideAngle_+180.0f);
            }
            // If no longer holding on to backward
            if (!Simian::Keyboard::IsPressed(KEY_BACKWARD))
            {
                SFlagUnset(movementFlag_, FLAG_MOVEMENT_BACKWARD);
                if (SIsFlagSet(movementFlag_, FLAG_MOVEMENT_LEFT))
                    setFinalRotationAngle_(rotSideAngle_);
                else if (SIsFlagSet(movementFlag_, FLAG_MOVEMENT_RIGHT))
                    setFinalRotationAngle_(rotSideAngle_+180.0f);
            }
            // If no longer holding on to left
            if (!Simian::Keyboard::IsPressed(KEY_LEFT))
            {
                SFlagUnset(movementFlag_, FLAG_MOVEMENT_LEFT);
                if (SIsFlagSet(movementFlag_, FLAG_MOVEMENT_FORWARD))
                    setFinalRotationAngle_(rotForwardAngle_);
                else if (SIsFlagSet(movementFlag_, FLAG_MOVEMENT_BACKWARD))
                    setFinalRotationAngle_(rotForwardAngle_+180.0f);
            }
            // If no longer holding on to right
            if (!Simian::Keyboard::IsPressed(KEY_RIGHT))
            {
                SFlagUnset(movementFlag_, FLAG_MOVEMENT_RIGHT);
                if (SIsFlagSet(movementFlag_, FLAG_MOVEMENT_FORWARD))
                    setFinalRotationAngle_(rotForwardAngle_);
                else if (SIsFlagSet(movementFlag_, FLAG_MOVEMENT_BACKWARD))
                    setFinalRotationAngle_(rotForwardAngle_+180.0f);
            }

            if (!SIsFlagSet(movementFlag_, FLAG_MOVEMENT_FORWARD) &&
                !SIsFlagSet(movementFlag_, FLAG_MOVEMENT_BACKWARD) &&
                !SIsFlagSet(movementFlag_, FLAG_MOVEMENT_RIGHT) &&
                !SIsFlagSet(movementFlag_, FLAG_MOVEMENT_LEFT))
                stateMachine_.ChangeState(PS_IDLE);
            else
                stateMachine_.ChangeState(PS_RUN);
        }
    }

    void GCPlayerLogic::flinchOnEnter_(Simian::DelegateParameter&)
    {
        model_->Controller().PlayAnimation("Block", false, animationFrameRate_);
        // Set up variables needed for flinch
        movementFlag_ = 0;
        movementSpeed_ = knockbackAmt_;
        // Face direction of hit
        setFinalRotationAngle_(vectorToDegreeFloat_(-knockbackVec_));
        // If there is a knockback
        movementVec_ = knockbackVec_ * movementSpeed_;
        flinchTimer_ = 0.0f;
    }

    void GCPlayerLogic::flinchOnUpdate_(Simian::DelegateParameter& param)
    {
        Simian::f32 dt = param.As<Simian::EntityUpdateParameter>().Dt;
        
        flinchTimer_ += dt;

        // Set movement vec
        movementVec_ = knockbackVec_ * movementSpeed_;

        // Interpolate knockback speed
        Simian::f32 intp = flinchTimer_/FLINCH_TIME_TOTAL;
        intp = Simian::Math::Clamp(intp, 0.0f, 1.0f);
        movementSpeed_ = Simian::Interpolation::Interpolate<Simian::f32>(knockbackAmt_, 0.0f, intp, Simian::Interpolation::EaseCubicOut);
        // End of flinch
        if (intp >= 1)
        {
            if (!Simian::Keyboard::IsPressed(KEY_FORWARD))
                SFlagUnset(movementFlag_, FLAG_MOVEMENT_FORWARD);
            if (!Simian::Keyboard::IsPressed(KEY_BACKWARD))
                SFlagUnset(movementFlag_, FLAG_MOVEMENT_BACKWARD);
            if (!Simian::Keyboard::IsPressed(KEY_LEFT))
                SFlagUnset(movementFlag_, FLAG_MOVEMENT_LEFT);
            if (!Simian::Keyboard::IsPressed(KEY_RIGHT))
                SFlagUnset(movementFlag_, FLAG_MOVEMENT_RIGHT);

            stateMachine_.ChangeState(PS_RUN);
        }
    }

    void GCPlayerLogic::cineOnEnter_(Simian::DelegateParameter&)
    {
        model_->Controller().PlayAnimation("Idle", true);
        physics_->Enabled(false);
    }

    void GCPlayerLogic::cineOnUpdate_(Simian::DelegateParameter&)
    {
        // Stop moving player
        movementFlag_ = 0;
        movementSpeed_ = 0;
        movementVec_ = Simian::Vector3::Zero;
        updatePhysics_();

        if (!playerControlOverride_)
        {
            if (teleportedAnimTimer_ > 0.0f)
                stateMachine_.ChangeState(PS_TELEPORTED);
            else
                stateMachine_.ChangeState(PS_IDLE);
        }
    }

    void GCPlayerLogic::cineOnExit_(Simian::DelegateParameter&)
    {
        // Set collision back with enemies
        physics_->Enabled(true);
        physics_->SetCollisionLayer(Collision::COLLISION_L1);
        updatePhysics_();
    }

    void GCPlayerLogic::teleportedOnEnter_(Simian::DelegateParameter&)
    {
        model_->Controller().PlayAnimation("Landing Up", false);
        teleportedAnimTimer_ = TELEPORTED_MAX_TIME;
    }

    void GCPlayerLogic::teleportedOnUpdate_(Simian::DelegateParameter& param)
    {
        Simian::f32 dt = param.As<Simian::EntityUpdateParameter>().Dt;

        // Stop moving player
        movementFlag_ = 0;
        movementSpeed_ = 0;
        movementVec_ = Simian::Vector3::Zero;
        updatePhysics_();

        teleportedAnimTimer_ -= dt;
        
        if (teleportedAnimTimer_ < (TELEPORTED_MAX_TIME-TELEPORT_DURATION))
        {
            GCPlayerLogic::Instance()->StopTeleportingEffect();
            if (teleportedAnimTimer_ < 0.0f)
            {
                teleportedAnimTimer_ = -1.0f;
                stateMachine_.ChangeState(PS_IDLE);
            }
        }
    }

    void GCPlayerLogic::dieOnEnter_(Simian::DelegateParameter&)
    {
        model_->Controller().PlayAnimation("Die", false, animationFrameRate_);
        // Stop moving player
        movementFlag_ = 0;
        movementSpeed_ = 0;
        movementVec_ = Simian::Vector3::Zero;
        physics_->Enabled(false);

        // Play sound
        sounds_->Play(S_DIE);

        // create the death roll over effect
        Simian::Entity* deathEffect = ParentScene()->CreateEntity(E_REVIVEBEAMEFFECT);
        deathEffect->ComponentByType(Simian::C_PARTICLESYSTEM, deathRollover_);
        deathRollover_->ParticleSystem()->Enabled(false);

        // set it to the player's position
        Simian::CTransform* transform;
        deathEffect->ComponentByType(Simian::C_TRANSFORM, transform);
        transform->WorldPosition(transform_->WorldPosition());

        // add to the scene
        ParentScene()->AddEntity(deathEffect);
    }

    void GCPlayerLogic::dieOnUpdate_(Simian::DelegateParameter&)
    {
    }

    void GCPlayerLogic::reviveOnEnter_(Simian::DelegateParameter&)
    {
        deathRollover_->ParticleSystem()->Enabled(false);
        model_->Controller().PlayAnimation("Revive", false);
        attributes_->Death(false);
        attributes_->Health(10);
        reviveParticles_->ParticleSystem()->Enabled(true);
        GCCinematicUI::Instance().FadeOut(Simian::Color(1, 1, 1, 1), REVIVE_TIMER_MAX, -10.0f);
    }

    void GCPlayerLogic::reviveOnUpdate_(Simian::DelegateParameter& param)
    {
        Simian::f32 dt = param.As<Simian::EntityUpdateParameter>().Dt;
        
        reviveTimer_ += dt;
        if (reviveTimer_ > REVIVE_TIMER_MAX)
        {
            StateLoading::Instance().Timer(1.0f);
            StateLoading::Instance().BackgroundColor(Simian::Color(1.0f, 1.0f, 1.0f, 1.0f));
            StateLoading::Instance().HideLoadScreen(true);
            SaveManager::Instance().LoadCheckpoint();
            //Simian::Game::Instance().GameStateManager().ChangeState(StateLoading::InstancePtr());
            //StateLoading::Instance().NextState(StateGame::InstancePtr());
        }
    }


    void GCPlayerLogic::createBullet_(Simian::f32 angle)
    {
        // Fire bullet
        Simian::Entity* bullet = ParentEntity()->ParentScene()->CreateEntity(E_PLAYERBULLET);
        ParentEntity()->ParentScene()->AddEntity(bullet);
        // Pass bullet attribs
        GCPlayerBulletLogic* playerBullet_;
        bullet->ComponentByType(GC_PLAYERBULLETLOGIC, playerBullet_);
        playerBullet_->OwnerAttribs(attributes_);
        // Set bullet transform
        Simian::CTransform* bulletTransform_;
        bullet->ComponentByType(Simian::C_TRANSFORM, bulletTransform_);
        bulletTransform_->Position(Simian::Vector3(parentTransform_->Position().X(),
                parentTransform_->Position().Y() + BULLET_START_HEIGHT, parentTransform_->Position().Z()));
        bulletTransform_->Rotation(Simian::Vector3(0, 1, 0), Simian::Degree(angle+90.0f));
    }

    // Calculate direction angles
    void GCPlayerLogic::calcDirAngles_()
    {
        vecForward_ = cameraBaseTransform_->World().Position()-cameraTransform_->World().Position();
        vecForward_.Y(0);
        vecForward_.Normalize();
        vecSide_ = Simian::Vector3(vecForward_.Z(), 0, -vecForward_.X());
        // Get angle of rotation in degrees for forward vector
        rotForwardAngle_ = vectorToDegreeFloat_(vecForward_);
        // Get angle of rotation in degrees for right side vector
        rotSideAngle_ = vectorToDegreeFloat_(vecSide_);
    }

    void GCPlayerLogic::setFinalRotationAngle_(Simian::f32 angle)
    {
        finalAngle_ = angle;
        initialAngle_ = transform_->Angle().Degrees();
        rotTimer_ = 0.0f;
    }

    Simian::f32 GCPlayerLogic::vectorToDegreeFloat_(const Simian::Vector3& vec)
    {
        // Get angle of rotation in degrees for vector
        Simian::f32 angle = std::acosf(vec.X());
        if (vec.Z() > 0)
            angle = Simian::Math::TAU - angle;
        angle = Simian::Radian(angle).Degrees();
        // Wrap angle within 360
        angle = Simian::Math::WrapAngle(Simian::Degree(angle)).Degrees();
        
        return angle;
    }

	void GCPlayerLogic::rotateAtStart_()
	{
		if (!rotateAtStartBool_ && (transform_->Angle().Degrees() > Simian::Math::EPSILON))
		{
			setFinalRotationAngle_(transform_->Angle().Degrees());
			rotateAtStartBool_ = true;
		}
	}

    // Attack helper functions
    void GCPlayerLogic::meleeAttackDetection_()
    {
        Simian::f32 attackAngle = ((attackMeleeAngle_/2.0f) * Simian::Math::PI) / 180.0f;

        Simian::u32 enemyNum = GCAIBase::AllEnemiesVec().size();
        // Loop through all enemies
        for (Simian::u32 i=0; i < enemyNum; ++i)
        {
            // Get vector to enemy
            GCAIBase* enemy = GCAIBase::AllEnemiesVec()[i];
            if(!enemy->Physics()->Enabled())
                continue;
            Simian::Vector3 playerToEnemy = enemy->Transform()->WorldPosition() - parentTransform_->Position();
            Simian::f32 pToELength = playerToEnemy.Length() - physics_->Radius() - enemy->Radius();

            // Check if distance to enemy is within attack range
            if (pToELength <= attackMeleeRange_)
            {
                // Check if facing enemy
                Simian::f32 dotProd = playerToEnemy.Dot(
                    Simian::Vector3(std::cos(transform_->Angle().Radians()+Simian::Math::PI/2), 0,
                                    std::sin(transform_->Angle().Radians()+Simian::Math::PI/2)));

                if ((dotProd/pToELength) > attackAngle)
                {
                    GCAttributes* attribs;
                    //Simian::CTransform* enemyTransform;
                    //GCPhysicsController* physics;
                    GCAIBase::AllEnemiesVec()[i]->ComponentByType(GC_ATTRIBUTES, attribs);
                    //GCAIBase::AllEnemiesVec()[i]->ComponentByType(Simian::C_TRANSFORM, enemyTransform);
                    //GCAIBase::AllEnemiesVec()[i]->ComponentByType(GC_PHYSICSCONTROLLER, physics);
                    if (attribs && attribs->Health() > 0)
                    {
                        /*if (enemyTransform && physics)
                        {
                            // spawn the hit effect
                            Simian::Entity* hitEffect = ParentScene()->CreateEntity(E_HITEFFECT);
                            Simian::CTransform* transform;
                            hitEffect->ComponentByType(Simian::C_TRANSFORM, transform);
                            Simian::Vector3 dirVec = transform_->WorldPosition() - enemyTransform->WorldPosition();
                            dirVec.Normalize();
                            transform->Position(enemyTransform->WorldPosition() + dirVec * physics->Radius());
                            ParentScene()->AddEntity(hitEffect);
                        }*/

                        // deal damage if the enemy has attributes
						// deal more damage if attack dash
						if (attackDashing_)
							CombatCommandSystem::Instance().AddCommand(attributes_, attribs,
								CombatCommandDealDamage::Create(static_cast<Simian::u32>(
									static_cast<Simian::f32>(attributes_->Damage())*attackDashDamageMultiplier_)));
						else
							CombatCommandSystem::Instance().AddCommand(attributes_, attribs,
								CombatCommandDealDamage::Create(attributes_->Damage()));
                    }
                }
            }
        }
    }

    void GCPlayerLogic::helperStartAttack_()
    {
        if (GCUI::GameStatus() == GCUI::GAME_PAUSED
            || GCUpgradeExchange::Exchanging()
            || GCGesturePuzzleTrigger::PlayerInRange())
        {
            stateMachine_.ChangeState(PS_IDLE);
            return;
        }

        std::stringstream tempSstream;
        attackDashing_ = false;

        // If not moving, face direction of mouse
        // Standstill attack
        if (!movementFlag_ || attackDashedPrev_)
        {
            // Picking a point of intersection
            Simian::Vector3 intersectionPt;
            Simian::Vector2 mouseVec = Simian::Mouse::GetMouseNormalizedPosition();
            if (fabs(mouseVec.X()) < 1 && fabs(mouseVec.Y()) < 1 &&
                Simian::Plane::Z.Intersect(
                Simian::Ray::FromProjection(cameraTransform_->World(), 
                camera_->Camera().Projection(), mouseVec),
                intersectionPt))
            {
                Simian::Vector3 attackVec(Simian::f32(intersectionPt.X()-parentTransform_->Position().X()),
                            0, Simian::f32(intersectionPt.Z()-parentTransform_->Position().Z()));
                attackVec.Normalize();
                // Get angle of rotation in degrees for attack vector
                Simian::f32 attackAngle = -vectorToDegreeFloat_(attackVec)-90.0f;

                // Get final angle from attack angle
                setFinalRotationAngle_(attackAngle);
            }
            // Randomize attack animation
            //tempSstream << "Attack " << Simian::Math::Random(1, attackAnimationsTotal_+1);
            tempSstream << "Attack " << currentAttackAnimationNum_;
            (++currentAttackAnimationNum_) > attackAnimationsTotal_ ? (currentAttackAnimationNum_ = 1) : 0;
            model_->Controller().PlayAnimation(tempSstream.str(), false, static_cast<Simian::u32>(animationFrameRate_*1.4f));
            attackDashedPrev_ = false;
        }
        // Moving attack
        else
        {
            tempSstream << "Attack Move 1";
            model_->Controller().PlayAnimation("Attack Move 1", false, static_cast<Simian::u32>(animationFrameRate_*1.0f));
            attackDashing_ = true;
            attackDashedPrev_ = true;
        }

        movementSpeed_ = movementSpeedAttackDash_;
        attackTimer_ = 0.0f;
        hasAttacked_ = false;

        // Play random sound
        if (sounds_)
            sounds_->Play(Simian::Math::Random(S_ATTACK_1, S_ATTACK_1 + TOTAL_ATTACK_SOUNDS));

        swordTrail_->Enabled(true);
    }

	bool GCPlayerLogic::helperBlockDetection_(GCAttributes* source)
	{
		if (blockTimer_ < blockEffectiveTime_)
		{
			if (!source) // if source of damage cannot be determined
				return false;

			Simian::f32 blockAngle = ((blockAngle_/2.0f) * Simian::Math::PI) / 180.0f;

			// Get vector to enemy
			Simian::Entity* sourceOfDamage = source->ParentEntity();
			Simian::CTransform* sourceTransform = 0;
			sourceOfDamage->ComponentByType(Simian::C_TRANSFORM, sourceTransform);
			Simian::Vector3 playerToEnemy = Simian::Vector3::Zero;
			if (sourceTransform)
				playerToEnemy = sourceTransform->Position() - parentTransform_->Position();
			Simian::f32 pToELength = playerToEnemy.Length();

			// Check if facing enemy
			Simian::f32 dotProd = playerToEnemy.Dot(
				Simian::Vector3(std::cos(transform_->Angle().Radians()+Simian::Math::PI/2), 0,
								std::sin(transform_->Angle().Radians()+Simian::Math::PI/2)));

			// Check if distance to enemy is within attack range
			if (pToELength > Simian::Math::EPSILON && pToELength <= attackMeleeRange_
												   && ((dotProd/pToELength) > blockAngle))
				return true;
			else
				return false;
		}
		return false;
	}

    void GCPlayerLogic::onTakeDamage_( Simian::DelegateParameter& p )
    {
        GCAttributes::DealDamageParameter& param = p.As<GCAttributes::DealDamageParameter>();

        // If dodging effectively, ignore the hit
        // Or if already dead
        if (stateMachine_.CurrentState() == PS_DODGE && dodgeTimer_ < dodgeEffectiveTime_
            || stateMachine_.CurrentState() == PS_DIE)
            param.Handled = true;
        else if (stateMachine_.CurrentState() == PS_BLOCK && helperBlockDetection_(param.Source))
            param.Handled = true;
        else if (stateMachine_.CurrentState() == PS_REVIVE)
            param.Handled = true;
        else
        {
            GCDescensionCamera::Instance()->Shake(0.05f, 0.25f);
        }
    }

    void GCPlayerLogic::onDeath_( Simian::DelegateParameter& )
    {
        if (stateMachine_.CurrentState() != PS_DIE && stateMachine_.CurrentState() != PS_REVIVE)
        {
            // activate the game over screen
            Simian::Entity* entity = ParentScene()->EntityByTypeRecursive(E_GAMEOVERUI);
            if (entity)
            {
                GCGameOver* gameover;
                entity->ComponentByType(GC_GAMEOVER, gameover);
                gameover->Show();
            }

            DManualLog(healthLogger_);
            stateMachine_.ChangeState(PS_DIE);
        }
    }

    void GCPlayerLogic::onLevelUp_( Simian::DelegateParameter& )
    {
        // play the effect of levelling up
        levelupEffect_->Play();
        if (sounds_)
            sounds_->Play(S_LEVEL_UP);
    }

    //--------------------------------------------------------------------------

    void GCPlayerLogic::OnSharedLoad()
    {
        RegisterCallback(Simian::P_POSTTRANSFORM, Simian::Delegate::CreateDelegate<GCPlayerLogic, &GCPlayerLogic::updateCamera_>(this));
        RegisterCallback(Simian::P_UPDATE, Simian::Delegate::CreateDelegate<GCPlayerLogic, &GCPlayerLogic::update_>(this));

        dyingEffectMaterial_ = Simian::Game::Instance().MaterialCache().Load("Materials/Compositor_HealthCritical.mat");
    }

    void GCPlayerLogic::OnAwake()
    {
        ComponentByType(Simian::C_TRANSFORM, transform_);
        ComponentByType(Simian::C_MODEL, model_);
        ParentEntity()->ParentEntity()->ComponentByType(GC_PHYSICSCONTROLLER, physics_);
        ComponentByType(Simian::C_SOUNDEMITTER, sounds_);

        Simian::Entity* entity = ParentScene()->EntityByTypeRecursive(E_CAMERA);
        entity->ComponentByType(Simian::C_TRANSFORM, cameraBaseTransform_);
        entity = entity->ChildByName("Camera Yaw")->ChildByName("Camera Pitch")->ChildByName("Camera");
        entity->ComponentByType(Simian::C_CAMERA, camera_);
        entity->ComponentByType(Simian::C_TRANSFORM, cameraTransform_);
        
        entity = ParentEntity()->ChildByType(E_SWORD_1);
        entity->ComponentByType(GC_BONETRAILVELOCITYCONTROLLER, swordTrail_);

        ParentEntity()->ParentEntity()->ComponentByType(Simian::C_TRANSFORM, parentTransform_);
        ComponentByType(GC_ATTRIBUTES, attributes_);
        movementSpeed_ = attributes_->MovementSpeed();

        ParentEntity()->ChildByType(E_PLAYER_LIGHT_BEAMS)->ComponentByType(Simian::C_PARTICLESYSTEM,reviveParticles_);
        reviveParticles_->ParticleSystem()->Enabled(false);

        entity = ParentEntity()->ParentEntity()->ChildByType(E_LEVELUP_EFFECT);
        entity->ComponentByType(Simian::C_PARTICLETIMELINE, levelupEffect_);

        entity = ParentScene()->EntityByType(E_SPIRITOFDEMAE);
        if (entity)
            entity->ComponentByType(GC_MOUSEPLANEINTERSECTION, spirit_);

        // Get center of screen
        screenHalfX_ = Simian::Game::Instance().Size().X()*0.5f;
        screenHalfY_ = Simian::Game::Instance().Size().Y()*0.5f;

        // Get pointer to myself
        player_ = this;

        // Create delegates for state machine
        stateMachine_[PS_IDLE].OnEnter = Simian::Delegate::CreateDelegate<GCPlayerLogic, &GCPlayerLogic::idleOnEnter_>(this);
        stateMachine_[PS_IDLE].OnUpdate = Simian::Delegate::CreateDelegate<GCPlayerLogic, &GCPlayerLogic::idleOnUpdate_>(this);
        stateMachine_[PS_RUN].OnEnter = Simian::Delegate::CreateDelegate<GCPlayerLogic, &GCPlayerLogic::runOnEnter_>(this);
        stateMachine_[PS_RUN].OnUpdate = Simian::Delegate::CreateDelegate<GCPlayerLogic, &GCPlayerLogic::runOnUpdate_>(this);
        stateMachine_[PS_ATTACK].OnEnter = Simian::Delegate::CreateDelegate<GCPlayerLogic, &GCPlayerLogic::attackOnEnter_>(this);
        stateMachine_[PS_ATTACK].OnUpdate = Simian::Delegate::CreateDelegate<GCPlayerLogic, &GCPlayerLogic::attackOnUpdate_>(this);
        stateMachine_[PS_ATTACK].OnExit = Simian::Delegate::CreateDelegate<GCPlayerLogic, &GCPlayerLogic::attackOnExit_>(this);
        stateMachine_[PS_SHOOT].OnEnter = Simian::Delegate::CreateDelegate<GCPlayerLogic, &GCPlayerLogic::shootOnEnter_>(this);
        stateMachine_[PS_SHOOT].OnUpdate = Simian::Delegate::CreateDelegate<GCPlayerLogic, &GCPlayerLogic::shootOnUpdate_>(this);
        stateMachine_[PS_DODGE].OnEnter = Simian::Delegate::CreateDelegate<GCPlayerLogic, &GCPlayerLogic::dodgeOnEnter_>(this);
        stateMachine_[PS_DODGE].OnUpdate = Simian::Delegate::CreateDelegate<GCPlayerLogic, &GCPlayerLogic::dodgeOnUpdate_>(this);
        stateMachine_[PS_DODGE].OnExit = Simian::Delegate::CreateDelegate<GCPlayerLogic, &GCPlayerLogic::dodgeOnExit_>(this);
        stateMachine_[PS_BLOCK].OnEnter = Simian::Delegate::CreateDelegate<GCPlayerLogic, &GCPlayerLogic::blockOnEnter_>(this);
        stateMachine_[PS_BLOCK].OnUpdate = Simian::Delegate::CreateDelegate<GCPlayerLogic, &GCPlayerLogic::blockOnUpdate_>(this);
        stateMachine_[PS_FLINCH].OnEnter = Simian::Delegate::CreateDelegate<GCPlayerLogic, &GCPlayerLogic::flinchOnEnter_>(this);
        stateMachine_[PS_FLINCH].OnUpdate = Simian::Delegate::CreateDelegate<GCPlayerLogic, &GCPlayerLogic::flinchOnUpdate_>(this);
        stateMachine_[PS_CINE].OnEnter = Simian::Delegate::CreateDelegate<GCPlayerLogic, &GCPlayerLogic::cineOnEnter_>(this);
        stateMachine_[PS_CINE].OnUpdate = Simian::Delegate::CreateDelegate<GCPlayerLogic, &GCPlayerLogic::cineOnUpdate_>(this);
        stateMachine_[PS_CINE].OnExit = Simian::Delegate::CreateDelegate<GCPlayerLogic, &GCPlayerLogic::cineOnExit_>(this);

        stateMachine_[PS_TELEPORTED].OnEnter = Simian::Delegate::CreateDelegate<GCPlayerLogic, &GCPlayerLogic::teleportedOnEnter_>(this);
        stateMachine_[PS_TELEPORTED].OnUpdate = Simian::Delegate::CreateDelegate<GCPlayerLogic, &GCPlayerLogic::teleportedOnUpdate_>(this);
        stateMachine_[PS_DIE].OnEnter = Simian::Delegate::CreateDelegate<GCPlayerLogic, &GCPlayerLogic::dieOnEnter_>(this);
        stateMachine_[PS_DIE].OnUpdate = Simian::Delegate::CreateDelegate<GCPlayerLogic, &GCPlayerLogic::dieOnUpdate_>(this);
        stateMachine_[PS_REVIVE].OnEnter = Simian::Delegate::CreateDelegate<GCPlayerLogic, &GCPlayerLogic::reviveOnEnter_>(this);
        stateMachine_[PS_REVIVE].OnUpdate = Simian::Delegate::CreateDelegate<GCPlayerLogic, &GCPlayerLogic::reviveOnUpdate_>(this);

        // register attribute callbacks
        attributes_->DealDamageDelegates() += 
            Simian::Delegate::CreateDelegate<GCPlayerLogic, &GCPlayerLogic::onTakeDamage_>(this);
        attributes_->DeathDelegates() += 
            Simian::Delegate::CreateDelegate<GCPlayerLogic, &GCPlayerLogic::onDeath_>(this);
        attributes_->LevelUpDelegates() +=
            Simian::Delegate::CreateDelegate<GCPlayerLogic, &GCPlayerLogic::onLevelUp_>(this);

        attributes_->LogicComponent(this);

        // return pointer to logger
        //healthLogger_ = MetricLogManager::Instance().AddTimedLogger("Player Health",MetricLogManager::MakeMemFuncPtr(attributes_,&GCAttributes::Health));
        //DLOG_ADD_MANUAL_LOGGER(posLogger_,"Player Pos",DLOG_MAKE_MEM_FUNC_PTR(transform_,&Simian::CTransform::Position));
        //posLogger_ = MetricLogManager::Instance().AddManualLogger("Player Pos",MetricLogManager::MakeMemFuncPtr<Simian::Vector3,const Simian::Matrix>(&transform_->World(),&Simian::Matrix::Position));
        healthLogger_ = DAddManualLogger("PlayerDeath",DMakeMemFuncPtrExplicit(Simian::Vector3,const Simian::Matrix,&transform_->World(),&Simian::Matrix::Position));
        posLogger_ = DAddTimedLogger("PlayerPos",DMakeMemFuncPtrExplicit(Simian::Vector3,const Simian::Matrix,&transform_->World(),&Simian::Matrix::Position),2.5f);

        // Parent weapons
        model_->AttachEntityToJoint("RightWristJoint2", ParentEntity()->ChildByType(E_SWORD_1));
        model_->AttachEntityToJoint("SpineJoint1", ParentEntity()->ChildByType(E_PLAYER_LIGHT_BEAMS));
        
        //model_->AttachEntityToJoint("LeftWristJoint", ParentEntity()->ChildByType(E_GUN_1));

		model_->Controller().PlayAnimation("Idle", true, animationFrameRate_);

        // get the compositor for the dying..
        if (dyingEffectMaterial_ && (*dyingEffectMaterial_)[0].Shader())
            (*dyingEffectMaterial_)[0].Shader()->FragmentProgramConstant("Amount", dyingEffectAmount_);

        //--Load data for player
        if (SaveManager::Instance().CurrentData().SaveID != SaveManager::SAVE_INVALID
            && SaveManager::Instance().CurrentData().HP != 0)
        {
            Attributes()->MaxHealth(SaveManager::Instance().CurrentData().HP);
            Attributes()->MaxMana(SaveManager::Instance().CurrentData().MP);
            Attributes()->Experience(SaveManager::Instance().CurrentData().Exp);
            Attributes()->Health(SaveManager::Instance().CurrentData().HP);
            Attributes()->Mana(SaveManager::Instance().CurrentData().MP);

            //--Use position only on checkpoint
            if (SaveManager::Instance().CurrentData().SaveID == SaveManager::SAVE_CHECKPOINT)
            {
                Simian::Vector2& pos = SaveManager::Instance().CurrentData().Position;
                GCPlayerLogic::Instance()->Physics()->Enabled(false);
                GCPlayerLogic::Instance()->Transform().ParentTransform()->
                    Position(Simian::Vector3(pos.X(), 0, pos.Y()));
                GCPlayerLogic::Instance()->Physics()->Enabled(true);
            }
        }
    }

    void GCPlayerLogic::PlayTeleportingEffect(bool enter)
    {
        if (teleporting_)
            return;
        teleporting_ = true;
        teleportTimer_ = 0.0f;
        attributes_->EnableHitColor(false);
        attributes_->ModelColor()->Operation(Simian::CModelColorOperation::CO_ADD);
        attributes_->ModelColor()->Color(Simian::Color(0.0f, 0.0f, 0.0f, 1.0f));

        if (enter)
        {
            teleportSquashFrom_ = TELEPORT_ORIGINAL_SCALE;
            teleportSquashTo_ = TELEPORT_MIN_SQUASH;
            teleportStretchFrom_ = TELEPORT_ORIGINAL_SCALE;
            teleportStretchTo_ = TELEPORT_MAX_STRETCH;
        }
        else
        {
            teleportSquashFrom_ = TELEPORT_MIN_SQUASH;
            teleportSquashTo_ = TELEPORT_ORIGINAL_SCALE;
            teleportStretchFrom_ = TELEPORT_MAX_STRETCH;
            teleportStretchTo_ = TELEPORT_ORIGINAL_SCALE;
        }
    }

    void GCPlayerLogic::StopTeleportingEffect()
    {
        teleporting_ = false;
        transform_->Scale(Simian::Vector3(TELEPORT_ORIGINAL_SCALE, TELEPORT_ORIGINAL_SCALE, TELEPORT_ORIGINAL_SCALE));
        attributes_->EnableHitColor(true);
        attributes_->ModelColor()->Color(Simian::Color(0.0f, 0.0f, 0.0f, 1.0f));
    }

    void GCPlayerLogic::ShowReviveBeam()
    {
        if (stateMachine_.CurrentState() == PS_DIE && deathRollover_)
            deathRollover_->ParticleSystem()->Enabled(true);
    }

    void GCPlayerLogic::HideReviveBeam()
    {
        if (stateMachine_.CurrentState() == PS_DIE && deathRollover_)
            deathRollover_->ParticleSystem()->Enabled(false);
    }

    void GCPlayerLogic::PlaySpiritSwirl()
    {
        if (!spirit_)
            return;

        spirit_->Active(false);
        swirling_ = true;
        swirlAmount_ = 0.0f;
        spirit_->InterpolateToPos(true);
    }

    void GCPlayerLogic::StopSpiritSwirl()
    {
        if (!spirit_)
            return;

        swirling_ = false;
        swirlAmount_ = 0.0f;
        spirit_->Active(true);
    }

    void GCPlayerLogic::Serialize( Simian::FileObjectSubNode& data )
    {
        data.AddData("MovementSpeedDodge",movementSpeedDodge_);
        data.AddData("DodgeTimeTotal",dodgeTimeTotal_);
        data.AddData("DodgeEffectiveTime",dodgeEffectiveTime_);

        data.AddData("BlockTimerUp",blockTimerUp_);
        data.AddData("BlockTimerMax",blockTimerMax_);
        data.AddData("BlockEffectiveTime",blockEffectiveTime_);
		data.AddData("BlockAngle",blockAngle_);

        data.AddData("ManaEnterCasting",manaEnterCasting_);
        data.AddData("ManaGradualCasting",manaGradualCasting_);
        data.AddData("ManaDrawCasting",manaDrawCasting_);
        data.AddData("ManaGradualRegen",manaGradualRegen_);

        data.AddData("MovementSpeedAttackDash",movementSpeedAttackDash_);
        data.AddData("MovementSpeedAttack",attackMovementSpeed_);
        data.AddData("AttackTimeTotal",attackTimeTotal_);
        data.AddData("AttackHitDelay",attackHitDelay_);
        data.AddData("AttackMeleeRange",attackMeleeRange_);
        data.AddData("AttackMeleeAngle",attackMeleeAngle_);
        data.AddData("AttackAnimationsTotal",attackAnimationsTotal_);
		data.AddData("AttackDashDamageMultiplier",attackDashDamageMultiplier_);

        data.AddData("BulletsMax",bulletsMax_);
        data.AddData("BulletsCurrent",bulletsCurr_);
        data.AddData("ReloadTimeMax",reloadTimeMax_);
        data.AddData("ReloadTimeCurrent",reloadTimeCurr_);

        data.AddData("CameraLocked", cameraLocked_);
    }

    void GCPlayerLogic::Deserialize( const Simian::FileObjectSubNode& data )
    {
        // get the data
        const Simian::FileDataSubNode* ml = data.Data("");
        ml = data.Data("AnimationFrameRate");
        ml ? animationFrameRate_ = static_cast<Simian::u32>(ml->AsF32()) : 0;

        ml = data.Data("MovementSpeedDodge");
        ml ? movementSpeedDodge_ = ml->AsF32() : 0;
        ml = data.Data("DodgeTimeTotal");
        ml ? dodgeTimeTotal_ = ml->AsF32() : 0;
        ml = data.Data("DodgeEffectiveTime");
        ml ? dodgeEffectiveTime_ = ml->AsF32() : 0;

        ml = data.Data("BlockTimerUp");
        ml ? blockTimerUp_ = ml->AsF32() : 0;
        ml = data.Data("BlockTimerMax");
        ml ? blockTimerMax_ = ml->AsF32() : 0;
        ml = data.Data("BlockEffectiveTime");
        ml ? blockEffectiveTime_ = ml->AsF32() : 0;
		ml = data.Data("BlockAngle");
        ml ? blockAngle_ = ml->AsF32() : 0;

        ml = data.Data("ManaEnterCasting");
        ml ? manaEnterCasting_ = ml->AsF32() : 0;
        ml = data.Data("ManaGradualCasting");
        ml ? manaGradualCasting_ = ml->AsF32() : 0;
        ml = data.Data("ManaDrawCasting");
        ml ? manaDrawCasting_ = ml->AsF32() : 0;
        ml = data.Data("ManaGradualRegen");
        ml ? manaGradualRegen_ = ml->AsF32() : 0;

        ml = data.Data("MovementSpeedAttackDash");
        ml ? movementSpeedAttackDash_ = ml->AsF32() : 0;
        ml = data.Data("MovementSpeedAttack");
        ml ? attackMovementSpeed_ = ml->AsF32() : 0;
        ml = data.Data("AttackTimeTotal");
        ml ? attackTimeTotal_ = ml->AsF32() : 0;
        ml = data.Data("ShootTimeTotal");
        ml ? shootTimeTotal_ = ml->AsF32() : 0;
        ml = data.Data("AttackHitDelay");
        ml ? attackHitDelay_ = ml->AsF32() : 0;
        ml = data.Data("AttackMeleeRange");
        ml ? attackMeleeRange_ = ml->AsF32() : 0;
        ml = data.Data("AttackMeleeAngle");
        ml ? attackMeleeAngle_ = ml->AsF32() : 0;
        ml = data.Data("AttackAnimationsTotal");
        ml ? attackAnimationsTotal_ = static_cast<Simian::u32>(ml->AsF32()) : 0;
		ml = data.Data("AttackDashDamageMultiplier");
        ml ? attackDashDamageMultiplier_ = ml->AsF32() : 0;

        ml = data.Data("BulletsMax");
        ml ? bulletsMax_ = static_cast<Simian::u32>(ml->AsF32()) : 0;
        ml = data.Data("BulletsCurrent");
        ml ? bulletsCurr_ = static_cast<Simian::u32>(ml->AsF32()) : 0;
        ml = data.Data("ReloadTimeMax");
        ml ? reloadTimeMax_ = ml->AsF32() : 0;
        ml = data.Data("ReloadTimeCurrent");
        ml ? reloadTimeCurr_ = ml->AsF32() : 0;

        data.Data("CameraLocked", cameraLocked_, cameraLocked_);
    }
}
