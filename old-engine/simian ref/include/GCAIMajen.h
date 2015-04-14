/******************************************************************************/
/*!
\file		GCAIMajen.h
\author 	Bryan Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	2012/08/10
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/******************************************************************************/
#ifndef SIMIAN_GCAIMAJEN_H_
#define SIMIAN_GCAIMAJEN_H_

#include "GCAIBase.h"

#include "Simian/Vector2.h"
#include "Simian/CModel.h"
#include "Simian/AnimationController.h"

namespace Simian
{
    class CTransform;
    class CModel;
    class Entity;
    class CSoundEmitter;
}

namespace Descension
{
    class GCPhysicsController;
    class GCAttributes;
    class GCParticleChainDeactivator;
    class GCParticleEffectDeactivator;
    class GCEventScript;

    class GCAIMajen: public GCAIBase
    {
    private:
        enum MajenStates
        {
            MS_STUNNED,
            MS_IDLE,
            MS_MELEE,
            MS_SWING,
            MS_CHASE,
            MS_DEATH,

            // chooses a skill to cast
            MS_CHOOSESKILL,      // 6
            
            // P1
            MS_TRIANGLE,         // 7
            MS_SPINSTART,        // 8, charge up to spin
            MS_SPIN,             // 9

            // P2
            MS_FIRESNAKESSTART,  // 10
            MS_FIRESNAKES,       // 11
            MS_MIRRORIMAGE,      // 12

            // P3
            MS_JUMPBACK,         // 13
            MS_SHOOTARC,         // 14
            MS_ARCCOOLDOWN,      // 15
            MS_DASH,             // 16

            // P1
            MS_SPINEND,          // 17, punishment after spin
            MS_STUNNEDSTART,     // 18
            MS_STUNNEDEND,       // 19
            MS_AFTERPLAYERDEATH, // 20

            MS_TOTAL
        };

        enum Sounds
        {
            S_CLONE,
            S_FIRESNAKES,
            S_FIRETRIANGLE_FORM,
            S_FIRETRIANGLE_FIRE,
            S_RUN,
            S_SHOOT1,
            S_SHOOT2,
            S_SHOOT3,
            S_SPIN,
            S_STOMP,
            S_ATTACK,
            S_GETHIT_1,
            S_GETHIT_2,
            S_SWORD_DASH,
            S_ATTACK_GRUNT1,
            S_ATTACK_GRUNT2,
            S_ATTACK_GRUNT3,
            S_DEATHTOTHEEMAE,
            S_HIT_GRUNT_1,
            S_HIT_GRUNT_2,
            S_HIT_GRUNT_3,
            S_LAUGH,
            S_STUN_GRUNT_1,
            S_STUN_GRUNT_2,
            S_STUN_GRUNT_3,
            S_THECRYSTALISMINE,
            S_TOTAL
        };

        struct PhaseVars
        {
            Simian::f32 spellDelay_;

            Simian::f32 triangleSize_;
            Simian::f32 triangleCastTime_;

            Simian::f32 spinCastTime_;
            Simian::f32 spinDuration_;

            Simian::f32 slamCastTime_;
            Simian::s32 slamSnakeCount_;

            Simian::s32 cloneCount_;

            Simian::s32 nextPhaseHP_;
            Simian::s32 stunCapDmg_;
            Simian::f32 stunMaxDuration_;
            Simian::f32 stunStartTime_;
            Simian::f32 stunEndTime_;

            std::vector<std::pair<Simian::s32, Simian::s32> > skills_;
        };
    private:
        Simian::EntityList trapListOri;
        Simian::EntityList trapList;
        Simian::Vector2 oldMin_, oldMax_;
        Simian::Vector3 nextTile_;
        GCParticleChainDeactivator* fireSpinDeactivator_;
        GCParticleEffectDeactivator* chargeAuraDeactivator_;
        Simian::CTransform* fireSpinTransform_;
        Simian::CTransform* chargeAuraTransform_;
        GCEventScript* reminderDodgeScript_;
        GCEventScript* reminderDemystScript_;
        Simian::CSoundEmitter* sounds_;
        Simian::f32 runSoundTimer_, runSoundTimerMax_;
        Simian::f32 spinSoundTimer_, spinSoundTimerMax_;
        Simian::f32 laughSoundTimer_, laughSoundTimerMax_;
        Simian::f32 spinDamage_;
        Simian::f32 spinDamageRadius_;
        Simian::f32 spinMovementSpeed_;
        Simian::f32 slamCooldown_;
        Simian::f32 slamRadius_;
        Simian::s32 slamDamage_;
        Simian::s32 slamSnakeDamage_;
        Simian::f32 cloneCastTime_;
        Simian::s32 cloneSpinChance_;
        Simian::f32 cloneSpellDelay_;
        Simian::s32 cloneHp_;
        Simian::f32 cloneLifetime_;

        Simian::f32 jumpBackSpeed_;
        Simian::f32 jumpBackTime_;
        Simian::f32 bulletInterval_;
        Simian::f32 dashWaitTime_;
        Simian::f32 dashSpeed_;
        Simian::f32 dashDuration_;
        Simian::s32 dashDamage_;
        Simian::f32 arcWidth_;
        Simian::f32 arcHeight_;
        Simian::u32 triangleDamage_;
        Simian::s32 numBullets_;
        Simian::s32 shotCount_;
        bool dashHit_;
        Simian::s32 stunHp_;
        Simian::u32 trapsEachPhase_;

        // steering
        Simian::f32 steerTimer_;
        Simian::Vector3 finalDir_;
        Simian::Vector3 startDir_;

        // serializable
        Simian::f32 idleDelay_;
        Simian::f32 attackDelay_;
        Simian::f32 swingDelay_;
        PhaseVars phaseVars_[3];

        // timers
        Simian::f32 timer_;
        Simian::f32 spellTimer_;
        Simian::f32 spinDamageTimer_;
        Simian::u32 currentPhase_;
        Simian::f32 flinchTimer_;
        Simian::f32 reminderTimer_;
        Simian::f32 reminderTimerMax_;
        Simian::u32 reminderPrev_;

        // is this a clone?
        bool clone_;

        static Simian::FactoryPlant<Simian::EntityComponent, GCAIMajen> factoryPlant_;
    private:
        void chaseEnter_(Simian::DelegateParameter& param);
        void chaseUpdate_(Simian::DelegateParameter& param);
        void chaseExit_(Simian::DelegateParameter& param);

        void idleEnter_(Simian::DelegateParameter& param);
        void idleUpdate_(Simian::DelegateParameter& param);

        void meleeEnter_(Simian::DelegateParameter& param);
        void meleeUpdate_(Simian::DelegateParameter& param);

        void swingEnter_(Simian::DelegateParameter& param);
        void swingUpdate_(Simian::DelegateParameter& param);
        void swingExit_(Simian::DelegateParameter& param);

        void chooseUpdate_(Simian::DelegateParameter& param);

        void triangleEnter_(Simian::DelegateParameter& param);
        void triangleUpdate_(Simian::DelegateParameter& param);

        void spinStartEnter_(Simian::DelegateParameter& param);

        void spinEnter_(Simian::DelegateParameter& param);
        void spinUpdate_(Simian::DelegateParameter& param);

        void spinEndEnter_(Simian::DelegateParameter& param);
        void spinEndExit_(Simian::DelegateParameter& param);

        void slamStartEnter_(Simian::DelegateParameter& param);

        void slamEnter_(Simian::DelegateParameter& param);
        void slamUpdate_(Simian::DelegateParameter& param);

        void cloneEnter_(Simian::DelegateParameter& param);
        void cloneUpdate_(Simian::DelegateParameter& param);

        void jumpBackEnter_(Simian::DelegateParameter& param);
        void jumpBackUpdate_(Simian::DelegateParameter& param);

        void shootArcEnter_(Simian::DelegateParameter& param);
        void shootArcUpdate_(Simian::DelegateParameter& param);

        void arcCooldownEnter_(Simian::DelegateParameter& param);
        void arcCooldownUpdate_(Simian::DelegateParameter& param);

        void dashEnter_(Simian::DelegateParameter& param);
        void dashUpdate_(Simian::DelegateParameter& param);

        void stunEnter_(Simian::DelegateParameter& param);
        void stunUpdate_(Simian::DelegateParameter& param);

        void stunStartEnter_(Simian::DelegateParameter& param);
        void stunEndEnter_(Simian::DelegateParameter& param);

        void deathEnter_(Simian::DelegateParameter& param);
        void deathUpdate_(Simian::DelegateParameter& param);

        void afterPlayerDeathEnter_(Simian::DelegateParameter& param);
        void afterPlayerDeathUpdate_(Simian::DelegateParameter& param);

        template<Simian::f32 PhaseVars::*timer, Simian::s32 nextState>
        void castUpdate_( Simian::DelegateParameter& param )
        {
            Simian::FiniteStateUpdateParameter* p;
            param.As(p);

            timer_ += p->Dt;

            if (timer_ > phaseVars_[currentPhase_].*timer)
                fsm_.ChangeState(nextState);
        }

        // Damage callback
        void onTakeDamage_(Simian::DelegateParameter& param);

        void readPhaseVars_(Simian::u32 index, const Simian::FileObjectSubNode* phase);
        void writePhaseVars_(Simian::u32 index, Simian::FileObjectSubNode* phase);

        bool reachedTile_(Simian::f32 dt);
        void updateNextTile_();

        void updateSpellTimer_(Simian::DelegateParameter& param);
        void postPhysics_(Simian::DelegateParameter& param);
        void overRideStateUpdate_( Simian::f32 dt );

        void moveTowardsPlayer_( Simian::f32 dt );

        void createBullet_(const Simian::Vector3& position, const Simian::Vector3& direction);
    public:
        GCAIMajen();

        void OnSharedLoad();
        void OnInit();
        void OnAwake();

        void Serialize( Simian::FileObjectSubNode& data );
        void Deserialize( const Simian::FileObjectSubNode& data );
    };
}

#endif
