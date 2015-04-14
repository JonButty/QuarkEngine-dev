/******************************************************************************/
/*!
\file		GCAICrystalGuardian.h
\author 	Jonathan Butt, j.butt, 250000211
\par    	email: j.butt\@digipen.edu
\date   	2012/08/10
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/******************************************************************************/
#ifndef DESCENSION_GCCRYSTALGUARDIAN_H_
#define DESCENSION_GCCRYSTALGUARDIAN_H_

#include "Simian\CModel.h"
#include "Simian\AnimationController.h"

#include "GCAIBase.h"

#include <list>
#include <string>
#include "Simian\LogManager.h"

namespace Simian
{
    class CModel;
    class CTransform;
    class Entity;
    class CSoundEmitter;
}

namespace Descension
{
    class GCAttributes;
    class GCAICrystalBossFist;
    class GCPhysicsController;
    class GCEventScript;
    class GCSpawnSequence;
    class GCCrystalSwap;

    class GCAICrystalGuardian: public GCAIBase
    {
    private:
        enum PhaseType
        {
            P_PHYSICAL,
            P_LIGHTNING,
            P_FIRE,
            P_ALL,
            P_TOTAL
        };
        enum State
        {
            CG_BIRTH,
            CG_IDLE,
            CG_CHOOSESKILL,

            CG_STUNENTER,
            CG_STUN,
            CG_STUNEXIT,

            CG_POUNDLCHARGE,
            CG_POUNDL,
            CG_POUNDL_CD,
            CG_POUNDRCHARGE,
            CG_POUNDR,
            CG_POUNDR_CD,

            CG_PUNCHLCHARGE,
            CG_PUNCHL,
            CG_PUNCHL_CD,
            CG_PUNCHRCHARGE,
            CG_PUNCHR,
            CG_PUNCHR_CD,

            CG_BEAMCHARGE,
            CG_BEAM,
            CG_BEAMDOWN,
            CG_BEAM_CD,

            CG_LIGHTNINGCHARGE,
            CG_LIGHTNING,
            CG_LIGHTNING_CD,

            CG_FIRESLAMCHARGE,
            CG_FIRESLAM,
            CG_FIRESLAM_CD,

            CG_TRIANGLESCHARGE,
            CG_TRIANGLES,
            CG_TRIANGLES_CD,

            CG_ROCKFALL,
            CG_ROCKFALL_CD,

            CG_DEATH,
            CG_TOTAL
        };

        enum Sounds
        {
            S_BIG_STOMP,
            S_DEATH_ROAR1,
            S_DEATH_ROAR2,
            S_GET_HIT1,
            S_GET_HIT2,
            S_GET_HIT3,
            S_GET_HIT4,
            S_GROWL1,
            S_GROWL2,
            S_GROWL3,
            S_HURT_GROWL1,
            S_HURT_GROWL2,
            S_PUNCH1,
            S_PUNCH2,
            S_ROAR,
            S_ROCK_LOOP,
            S_STOMP,
            S_PARAGON_CRYSTAL_COLLECT,
            S_ROCK_BURST1,
            S_ROCK_BURST2,
            S_LIGHTNING_BEAM,
            S_FIRE_TRIANGLE_FORM,
            S_FIRE_TRIANGLE_FIRE,
            S_FIRE_SNAKES,
            S_TOTAL
        };

        struct Attack
        {
            Simian::u32 id_;
            Simian::u32 chance_;
            Simian::u32 damage_;
            Simian::f32 radius_;
        };

        typedef std::vector<Attack> AttackList;

        struct Phase
        {
            Simian::s32 health_;
            Simian::f32 idleTime_;
            AttackList attacks_;
            Simian::CModel* leftCrystal_;
            Simian::CModel* rightCrystal_;
        };

        struct Effect
        {
            bool damaged_;
            Simian::Entity* effect_;
            Simian::Entity* castDecal_;
            Simian::CTransform* transform_;
            Simian::f32 timer_;
            Simian::f32 beginScale_;
            Simian::f32 endScale_;
        };
        typedef std::list<Effect>EffectList;
        typedef EffectList::iterator EffectIterator;
    private:
        bool dead_;
        bool doneDamage_;
        bool attacked_;
        GCAttributes* playerAttribs_;
        Simian::CModel* model_;
        GCPhysicsController* physics_;
        GCAICrystalBossFist* rightHand_;
        GCAICrystalBossFist* leftHand_;
        Simian::CTransform* effTrans_;
        Simian::Entity* effEnt_;
        GCEventScript* stunEventScript_;
        GCEventScript* dieEventScript_;
        GCSpawnSequence* gauntletSpawnSeq_;
        Simian::CSoundEmitter* sounds_;
        Simian::u32 currPhase_;
        Simian::u32 lightningMaxCount_;
        Simian::u32 lightningCount_;
        Simian::u32 fireSnakeCount_;
        Simian::u32 fireSnakeDamage_;
        Simian::u32 fireTriCount_;
        Simian::f32 rockFallCount_;
        Simian::f32 rockFallCounter_;
        Simian::f32 timer_;
        Simian::f32 timerTable_[CG_TOTAL];
        Simian::f32 lightningDelay_;
        Simian::f32 lightningTargetChargeTime_;
        Simian::f32 lightningTargetTime_;
        Simian::f32 lightningDelayTimer_;
        Simian::f32 rockFallMinSpawnDelay_;
        Simian::f32 rockFallMaxSpawnDelay_;
        Simian::f32 rockFallSpawnTimer_;
        Simian::f32 rockDebrisRadius_;
        Simian::Vector3 bossTriggerPos_;

        Phase phases_[P_TOTAL];
        Attack* currAttackData_;

        EffectList effects_;
        static const std::string ANIMATION_TABLE[CG_TOTAL];
        static const std::string STUN_FLINCH_ANIM_TABLE[2];
        
        static Simian::FactoryPlant<Simian::EntityComponent, GCAICrystalGuardian> factoryPlant_;
    private:
        void deserializePhases_(Simian::u32 index, const Simian::FileObjectSubNode* phase);
        void serializePhases_(Simian::u32 index, Simian::FileObjectSubNode* phase);

        void update_( Simian::DelegateParameter& param );
        void onTakeDamage_(Simian::DelegateParameter& param);
        
        void idleOnEnter_( Simian::DelegateParameter& param );
        void idleOnUpdate_( Simian::DelegateParameter& param );

        void chooseSkillOnUpdate_( Simian::DelegateParameter& param );
        
        void stunEnterOnUpdate_( Simian::DelegateParameter& param );

        void poundLOnEnter_( Simian::DelegateParameter& param );
        void poundLOnUpdate_( Simian::DelegateParameter& param );

        void poundROnEnter_( Simian::DelegateParameter& param );
        void poundROnUpdate_( Simian::DelegateParameter& param );

        void poundCDOnUpdate_( Simian::DelegateParameter& param );

        void punchLOnEnter_( Simian::DelegateParameter& param );
        void punchLOnUpdate_( Simian::DelegateParameter& param );

        void punchROnEnter_( Simian::DelegateParameter& param );
        void punchROnUpdate_( Simian::DelegateParameter& param );

        void punchCDOnUpdate_( Simian::DelegateParameter& param );

        void beamOnEnter_( Simian::DelegateParameter& param );
        void beamOnUpdate_( Simian::DelegateParameter& param );
        void beamOnExit_( Simian::DelegateParameter& param );

        void lightningChargeOnEnter_(Simian::DelegateParameter& param );
        void lightningChargeOnUpdate_(Simian::DelegateParameter& param );

        void lightningOnEnter_( Simian::DelegateParameter& param );
        void lightningOnUpdate_( Simian::DelegateParameter& param );
        void lightningOnExit_( Simian::DelegateParameter& param );

        void fireSlamChargeOnEnter_(Simian::DelegateParameter& param);

        void fireSlamOnEnter_(Simian::DelegateParameter& param);
        void fireSlamOnExit_(Simian::DelegateParameter& param);

        void fireSlamCDOnEnter_(Simian::DelegateParameter& param);

        void trianglesChargeOnEnter_( Simian::DelegateParameter& param );
        
        void trianglesOnEnter_( Simian::DelegateParameter& param );

        void rockFallOnEnter_(Simian::DelegateParameter& param );
        void rockFallOnUpdate_(Simian::DelegateParameter& param );

        void deathOnUpdate_(Simian::DelegateParameter& param);

        void stunExitOnExit_(Simian::DelegateParameter& param);

        void stateOnExit_( Simian::DelegateParameter& param );

        void overRideStateUpdate_(Simian::f32){}

        bool damageAreaCheck_(const Simian::CTransform* trans, Simian::f32 radius);
        void dealDamage_();
        // choose a random attack from a phase, returns the state to transition 
        // to
        Simian::u32 randomAttack_(Simian::u32 phase);
        void createRecoveryShard_(bool isHealth);

        void spawnPoundEffect_(const Simian::Vector3& pos);

        // generator function for casting enter callback
        template <Simian::u32 ID, bool loop>
        void castingEnter_(Simian::DelegateParameter& )
        {
            // play the animation in ANIMATION_TABLE based on ID
            // set timer to Duration
            std::string id = ANIMATION_TABLE[ID];
            if(!id.empty())
                model_->Controller().PlayAnimation(ANIMATION_TABLE[ID],loop);
            timer_ = timerTable_[ID];

            // Play sounds
            if (sounds_)
                sounds_->Play(Simian::Math::Random(S_GROWL1, S_GROWL1+TOTAL_GROWL_SOUNDS));
        }

        // generator function for casting update callback
        template <Simian::u32 NextState>
        void castingUpdate_(Simian::DelegateParameter& p)
        {
            timer_ -= p.As<Simian::EntityUpdateParameter>().Dt;
            if(timer_ < 0)
                fsm_.ChangeState(NextState);
        }
    public:
        // put your usual functions here.. ctor, serialize, deserialize, etc.
        GCAICrystalGuardian();

        void OnSharedLoad();
        void OnInit();
        void OnDeinit();
        void OnAwake();

        void Serialize( Simian::FileObjectSubNode& data );
        void Deserialize( const Simian::FileObjectSubNode& data );
    };
}

#endif
