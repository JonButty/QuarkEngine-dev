/******************************************************************************/
/*!
\file		GCAIGeneric.h
\author 	Jonathan Butt, j.butt, 250000211
\par    	email: j.butt\@digipen.edu
\date   	2012/08/10
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/******************************************************************************/
#ifndef DESCENSION_GCAIGENERIC_H_
#define DESCENSION_GCAIGENERIC_H_

#include "Simian/EntityComponent.h"
#include "Simian/Vector3.h"
#include "Simian/Vector2.h"

#include "GCAIBase.h"

namespace Simian
{
    class CTransform;
    class CModel;
    class CSoundEmitter;
    class CParticleSystem;
}

namespace Descension
{
    class GCTileMap;
    class GCPhysicsController;
    class GCPlayerLogic;
    class GCAttributes;

    enum EnemyStates
    {
        ES_BIRTH,
        ES_IDLE,
        ES_SEEKPLAYER,
        ES_SEEKPLAYER_IDLE,
        ES_ATTACK,
        ES_ATTACK_DEFAULT,
        ES_DEATH,
        ES_TOTAL
    };

    enum BulletType
    {
        BT_BASIC,
        BT_ABYSSAL_REVENANT
    };

    enum OverideStates
    {
        OS_SEEKIDLE,
        OS_SEEK,
        OS_TOTAL
    };

    enum Sounds
    {
        S_ALERT_1,
        S_ALERT_2,
        S_ALERT_3,
        S_DASH,
        S_DIE_1,
        S_DIE_2,
        S_RUN,
        S_SHOOT,
        S_ATTACK,
        S_BIRTH,
        S_MELEE_GET_HIT_1,
        S_MELEE_GET_HIT_2,
        S_POOF,
        S_ROAR,
        S_SLAM,
    };

    class GCAIGeneric : public GCAIBase
    {
    private:
        GCTileMap* tMap_;
        GCPlayerLogic* playerLogic_;
        Simian::CTransform* playerTransform_;
        GCPhysicsController* playerPhysics_;
        GCAttributes* playerAtt_;
        Simian::CSoundEmitter* sounds_;
        Simian::CParticleSystem* spawnParticles_;
        bool isRanged_;
        bool hasAttacked_;
        bool seekIdle_;
		bool shadowShown_, effectsOnSummon_;
        Simian::u32 attackAnimationsTotal_;
        std::string attackAnimationPrefix_;
        std::string shootAnimationPrefix_;
        Simian::u32 animationFrameRate_;
        Simian::u32 bulletType_;
        Simian::f32 idleBufferTimer_;
        Simian::f32 seekRad_; 
        Simian::f32 lostRad_; 
        Simian::f32 attackRad_;
        Simian::f32 skillRad_;
        Simian::f32 attackDist_;
        Simian::f32 seekDist_;
        Simian::f32 lostDist_;
        Simian::f32 skillDist_;
        Simian::f32 seekIdleDist_;
        Simian::f32 seekRadiusSqr_;
        Simian::f32 attackRadiusSqr_;
        Simian::f32 skillRadiusSqr_;
        Simian::f32 lostRadiusSqr_;
        Simian::f32 attackTimer_;
        Simian::f32 seekBufferTimer_;
        Simian::f32 steerTimer_;
        Simian::f32 attackTimeTotal_;
        Simian::f32 attackHitDelay_;
        Simian::f32 bulletStartHeight_;
        Simian::f32 runSoundTimer_, runSoundTimerMax_;
        Simian::f32 maxSteerTime_;
        Simian::Vector2 oldMin_;
        Simian::Vector2 oldMax_;
        Simian::Vector3 vecToPlayer_;
        Simian::Vector3 nextTile_;
        Simian::Vector3 prevDir_;
        Simian::Vector3 startDir_;
        Simian::Vector3 finalDir_;
        Simian::FiniteStateMachine overrideFsm_;
        std::string currentAttackAnimation_;

        static Simian::FactoryPlant<Simian::EntityComponent, GCAIGeneric> factoryPlant_;
    public:
        Simian::s32 Health() const;
        bool HasAttacked() const;
        Simian::f32 SeekRadiusSqr() const;
        Simian::f32 AttackRadiusSqr() const;
        Simian::f32 LostRadiusSqr() const;
        Simian::CSoundEmitter* Sounds() const;
    private:
        void birthOnEnter_( Simian::DelegateParameter& param );
        void birthOnUpdate_( Simian::DelegateParameter& param );
        void idleOnEnter_( Simian::DelegateParameter& param );
        void idleOnUpdate_( Simian::DelegateParameter& param );
        void seekPlayerOnEnter_( Simian::DelegateParameter& param );
        void seekPlayerOnUpdate_( Simian::DelegateParameter& param );
        void seekPlayerIdleOnEnter_( Simian::DelegateParameter& param );
        void seekPlayerIdleOnUpdate_( Simian::DelegateParameter& param );
        void attackOnEnter_ (Simian::DelegateParameter& param);
        void attackOnUpdate_ (Simian::DelegateParameter& param);
        void attackDefaultOnEnter_( Simian::DelegateParameter& param );
        void attackDefaultOnUpdate_( Simian::DelegateParameter& param );
        void deathOnEnter_( Simian::DelegateParameter& param );
        void deathOnUpdate_( Simian::DelegateParameter& param );
        void updateVecToPlayer_();
        void updateNextTile_( const Simian::Vector3& target );
        bool reachedNextTile_(Simian::f32 dt);
        void steer_( Simian::f32 dt );
        void facePlayer_();
        Simian::f32 vectorToDegreeFloat_(const Simian::Vector3& vec);
        void overideSeekIdleOnEnter_( Simian::DelegateParameter& param );
        void overideSeekIdleOnUpdate_( Simian::DelegateParameter& param );
        void overideSeekOnEnter_( Simian::DelegateParameter& param );
        void overideSeekOnUpdate_( Simian::DelegateParameter& param );
        void overRideStateUpdate_( Simian::f32 dt );
        // Attack functions
        void helperStartAttack_(Simian::f32 dt);
        void meleeAttackDetection_();
        void createBullet_(Simian::f32 angle);
        void takeDamage_(Simian::DelegateParameter& param);
        void postPhysics_(Simian::DelegateParameter& param);
    public:
        GCAIGeneric();
        void OnSharedLoad();
        void Deserialize( const Simian::FileObjectSubNode& data );
        void Serialize( Simian::FileObjectSubNode& data );
        void OnAwake();
        void OnInit();
        void OnDeinit();
    };
}

#endif
