/*****************************************************************************/
/*!
\file		GCPlayerLogic.h
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
#ifndef DESCENSION_GCPLAYERLOGIC_H_
#define DESCENSION_GCPLAYERLOGIC_H_

#include "Simian/EntityComponent.h"
#include "Simian/Vector3.h"
#include "Simian/FiniteStateMachine.h"

namespace Simian
{
    class CTransform;
    class CModel;
    class CCamera;
    class Keyboard;
    class Mouse;
    class FiniteStateMachine;
    class CSoundEmitter;
    class ShaderFloatParameter;
    class Material;
    class CParticleSystem;
    class CParticleTimeline;
}

namespace Descension
{
    class GCPhysicsController;
    class GCAttributes;
    class GCBoneTrailVelocityController;
    class MetricLoggerBase;
    class GCMousePlaneIntersection;

    class GCPlayerLogic: public Simian::EntityComponent
    {
    public:
        static const Simian::u32 TOTAL_SLASH_SOUNDS = 4;
        static const Simian::u32 KEY_FORWARD;
        static const Simian::u32 KEY_BACKWARD;
        static const Simian::u32 KEY_LEFT;
        static const Simian::u32 KEY_RIGHT;
        enum Sounds
        {
            S_ATTACK_1,
            S_ATTACK_2,
            S_CAST_LOOP,
            S_DIE,
            S_DODGE_1,
            S_DODGE_2,
            S_DODGE_3,
            S_RUN,
            S_CAST_FIRE,
            S_CAST_SLASH_1,
            S_CAST_SLASH_2,
            S_CAST_SLASH_3,
            S_CAST_SLASH_4,
            S_MELEE_GET_HIT,
            S_RANGED_GET_HIT,
            S_HEARTBEAT_1,
            S_HEARTBEAT_2,
            S_HEARTBEAT_3,
            S_HEARTBEAT_4,
            S_CAST_FIRE_CRACKLE,
            S_LIGHTNING_GET_HIT,
            S_LEVEL_UP,
            S_TOTAL
        };
    private:
        enum Player_States
        {
            PS_IDLE,
            PS_RUN,
            PS_ATTACK,
            PS_SHOOT,
            PS_DODGE,
            PS_BLOCK,
            PS_FLINCH,
            PS_CINE,
            PS_TELEPORTED,
            PS_DIE,
            PS_REVIVE,
            PS_TOTAL
        };
        enum Flags
        {
            FLAG_MOVEMENT_FORWARD,
            FLAG_MOVEMENT_BACKWARD,
            FLAG_MOVEMENT_LEFT,
            FLAG_MOVEMENT_RIGHT
        };
    private:
        MetricLoggerBase* healthLogger_,* posLogger_;
        Simian::CTransform* transform_;
        Simian::CTransform* parentTransform_;
        GCPhysicsController* physics_;
        Simian::CSoundEmitter* sounds_;
        GCBoneTrailVelocityController* swordTrail_;
        Simian::CParticleSystem* reviveParticles_;
        static GCPlayerLogic* player_;

        Simian::Vector2 oldMin_;
        Simian::Vector2 oldMax_;

        Simian::f32 movementSpeed_;
        Simian::Vector3 movementVec_;
        Simian::u32 movementFlag_;
        Simian::FiniteStateMachine stateMachine_;
        Simian::f32 screenHalfX_;
        Simian::f32 screenHalfY_;
        bool inSpellCasting_, dodgeEffective_, blockEffective_;

        // Vectors for forward and side motion
        // Angle floats for forward and side motion
        Simian::Vector3 vecForward_;
        Simian::Vector3 vecSide_;
        Simian::f32 rotForwardAngle_;
        Simian::f32 rotSideAngle_;

        // Angle floats for interpolation
        Simian::f32 initialAngle_;
        Simian::f32 finalAngle_;
        Simian::f32 rotTimer_;
		bool rotateAtStartBool_;

        // Dodge Timers
        Simian::f32 dodgeTimer_;

        // Block Timers
        Simian::f32 blockTimer_;
        Simian::f32 blockTimerUp_;
        Simian::f32 blockTimerMax_;
        Simian::f32 blockEffectiveTime_;
        bool blockOver_;

        // Attack Timers
        Simian::f32 attackTimer_;
        bool attackDashing_;
        bool attackDashedPrev_;
        bool hasAttacked_;
        Simian::u32 bulletsMax_;
        Simian::u32 bulletsCurr_;
        Simian::f32 reloadTimeMax_;
        Simian::f32 reloadTimeCurr_;
		Simian::f32 attackDashDamageMultiplier_;
        bool isReloading_;
        
        // Sound Timers
        Simian::f32 runSoundTimer_;
        Simian::f32 castSoundTimer_;

        // Mana Variables
        Simian::f32 manaEnterCasting_;
        Simian::f32 manaGradualCasting_;
        Simian::f32 manaDrawCasting_;
        Simian::f32 manaGradualRegen_;

        // Flinch Variables
        Simian::Vector3 knockbackVec_;
        Simian::f32 knockbackAmt_;
        Simian::f32 flinchTimer_;

        // Revive Variables
        Simian::f32 reviveTimer_;

        // Model and Animation
        Simian::CModel* model_;
        // Camera
        Simian::CCamera* camera_;
        Simian::CTransform* cameraTransform_;
        Simian::CTransform* cameraBaseTransform_;
        Simian::Vector3 cameraForce_;
        Simian::Vector3 cameraVelocity_;
        bool cameraLocked_, playerControlOverride_;

        // Deserialized data, attributes of player
        GCAttributes* attributes_;

        // Deserialized statistics
        Simian::u32 animationFrameRate_;
        Simian::f32 movementSpeedDodge_;
        Simian::f32 dodgeTimeTotal_;
        Simian::f32 dodgeEffectiveTime_;

        Simian::f32 movementSpeedAttackDash_;
        Simian::f32 attackDashTimeTotal_;
        Simian::f32 attackMovementSpeed_;
        Simian::f32 attackTimeTotal_;
        Simian::f32 shootTimeTotal_;
        Simian::f32 attackHitDelay_;
        Simian::f32 attackMeleeRange_;
        Simian::f32 attackMeleeAngle_;
        Simian::u32 attackAnimationsTotal_;
        Simian::u32 currentAttackAnimationNum_;
		Simian::f32 blockAngle_;

        // heartbeat sound percentage limits
        Simian::f32 heartbeatLimit1_;
        Simian::f32 heartbeatLimit2_;
        Simian::f32 heartbeatLimit3_;
        Simian::f32 heartbeatLimit4_;
        Simian::f32 heartbeatSoundTimer_;
        Simian::u32 heartbeatState_;

        Simian::Material* dyingEffectMaterial_;
        Simian::ShaderFloatParameter* dyingEffectAmount_;

        // teleport effect
        bool teleporting_;
        Simian::f32 teleportTimer_;
        Simian::f32 teleportedAnimTimer_;
        Simian::f32 teleportSquashFrom_;
        Simian::f32 teleportSquashTo_;
        Simian::f32 teleportStretchFrom_;
        Simian::f32 teleportStretchTo_;

        // spirit swirl
        bool swirling_;
        Simian::f32 swirlAmount_;
        GCMousePlaneIntersection* spirit_;

        // levelup effect
        Simian::CParticleTimeline* levelupEffect_;

        // death rollover
        Simian::CParticleSystem* deathRollover_;

        // factory plant
        static Simian::FactoryPlant<Simian::EntityComponent, GCPlayerLogic> factoryPlant_;
    public:
        Simian::CTransform& Transform();
        GCPhysicsController* Physics() const;
        // Getter to myself! In all my glory.
        static GCPlayerLogic* Instance();
        GCAttributes* Attributes() const;
        //camera getter
        Simian::CCamera* Camera() const;

        // locking or unlocking camera
        bool CameraLocked() const;
        void CameraLocked(bool val);

        // player control override
        bool PlayerControlOverride() const;
        void PlayerControlOverride(bool val);

        // dodge effective
        bool DodgeEffective() const;
        void DodgeEffective(bool val);

        // block effective
        bool BlockEffective() const;
        void BlockEffective(bool val);

        // revive
        void RevivePlayer();

        // teleported
        void TeleportedPlayer();

        // update heartbeat and dying effect
        void ResetHeartBeatAndDyingEffect();

        Simian::CSoundEmitter* Sounds() const;

        Simian::f32 ManaDrawCasting() const;
        void AddExperience(Simian::u32 val);
    private:
        void updateCamera_(Simian::DelegateParameter& param);
        void update_(Simian::DelegateParameter& param);
        void updateInput_();
        void updateMovement_();
        void updateRotation_(Simian::f32 dt);
        void updateSpellCasting_(Simian::f32 dt);
        void updateGunReloading_(Simian::f32 dt);
        void updateHeartBeatSound_();
        void updateDyingEffect_();
        void updateTeleportEffect_(Simian::f32 dt);
        void updatePhysics_();
        void updateSpiritSwirl_(Simian::f32 dt);

        // State machine functions
        void idleOnEnter_(Simian::DelegateParameter& param);
        void idleOnUpdate_(Simian::DelegateParameter& param);
        void runOnEnter_(Simian::DelegateParameter& param);
        void runOnUpdate_(Simian::DelegateParameter& param);
        void attackOnEnter_(Simian::DelegateParameter& param);
        void attackOnUpdate_(Simian::DelegateParameter& param);
        void attackOnExit_(Simian::DelegateParameter& param);
        void shootOnEnter_(Simian::DelegateParameter& param);
        void shootOnUpdate_(Simian::DelegateParameter& param);
        void dodgeOnEnter_(Simian::DelegateParameter& param);
        void dodgeOnUpdate_(Simian::DelegateParameter& param);
        void dodgeOnExit_(Simian::DelegateParameter& param);
        void blockOnEnter_(Simian::DelegateParameter& param);
        void blockOnUpdate_(Simian::DelegateParameter& param);
        void flinchOnEnter_(Simian::DelegateParameter& param);
        void flinchOnUpdate_(Simian::DelegateParameter& param);
        void cineOnEnter_(Simian::DelegateParameter& param);
        void cineOnUpdate_(Simian::DelegateParameter& param);
        void cineOnExit_(Simian::DelegateParameter& param);
        void teleportedOnEnter_(Simian::DelegateParameter& param);
        void teleportedOnUpdate_(Simian::DelegateParameter& param);
        void dieOnEnter_(Simian::DelegateParameter& param);
        void dieOnUpdate_(Simian::DelegateParameter& param);
        void reviveOnEnter_(Simian::DelegateParameter& param);
        void reviveOnUpdate_(Simian::DelegateParameter& param);

        // Helper functions
        Simian::f32 vectorToDegreeFloat_(const Simian::Vector3& vec);
        void calcDirAngles_();
        void setFinalRotationAngle_(Simian::f32 angle);
		void rotateAtStart_();
        void createBullet_(Simian::f32 angle);
        void helperStartAttack_();
		bool helperBlockDetection_(GCAttributes* source);

        // Attack functions
        void meleeAttackDetection_();

        // Damage callback
        void onTakeDamage_(Simian::DelegateParameter& param);
        void onDeath_(Simian::DelegateParameter& param);

        // Levelup callback
        void onLevelUp_(Simian::DelegateParameter& param);

        void postPhysics_(Simian::DelegateParameter& param);
    public:
        GCPlayerLogic();
        ~GCPlayerLogic();

        void OnSharedLoad();
        void OnAwake();

        void PlayTeleportingEffect(bool enter);
        void StopTeleportingEffect();

        void ShowReviveBeam();
        void HideReviveBeam();

        void PlaySpiritSwirl();
        void StopSpiritSwirl();

        void Serialize(Simian::FileObjectSubNode& data);
        void Deserialize(const Simian::FileObjectSubNode& data);
    };
}

#endif
