/*****************************************************************************/
/*!
\file		GCAIBossGhost.h
\author 	Jonathan Butt, j.butt, 250000211
\par    	email: j.butt\@digipen.edu
\date   	August 10, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*****************************************************************************/

#ifndef DESCENSION_GCAIBOSSGHOST_H_
#define DESCENSION_GCAIBOSSGHOST_H_

#include "Simian/SimianTypes.h"
#include "Simian/Delegate.h"
#include "Simian/Vector3.h"
#include "Simian/FileObjectSubNode.h"
#include "Simian/EntityComponent.h"
#include "Simian/Entity.h"

#include "PathFinder.h"
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
    class GCPhysicsController;
    class TileMap;
    class GCAttributes;
    class GCSpawnSequence;
    class GCEventScript;

    class GCAIBossGhost : public GCAIBase
    {
        enum BossStates
        {
            BS_BIRTH,
            BS_IDLE,
            BS_SEEK,
            BS_FLEE,
            BS_MELEE,
            BS_TELEPORT,
            BS_RANGE,
            BS_LIGHTNING,
            BS_COOLDOWN,
            BS_DEATH,
            BS_PHASETWO,
            BS_BEAM_PHASETWO,
            BS_PHASETHREE,
            BS_LIGHTNING_PHASETHREE,
            BS_TOTAL
        };

        enum BossPhases
        {
            BP_BIRTH,
            BP_PHASE1,
            BP_PHASE2,
            BP_PHASE3,
            BP_TOTAL
        };

        enum Sounds
        {
            S_BEAM,
            S_DIE_1,
            S_DIE_2,
            S_LAUGH,
            S_LAUGH2,
            S_LIGHTNING,
            S_MELEE_MISS,
            S_MELEE_HIT,
            S_PRE_LIGHTNING,
            S_SHOOT,
            S_SPAWN,
            S_TELEPORT,
            S_MELEE_GET_HIT_1,
            S_MELEE_GET_HIT_2
        };

        struct LightningEffect
        {
            bool damaged_;
            Simian::Entity* effect_;
            Simian::Entity* castDecal_;
            Simian::CTransform* transform_;
            Simian::f32 timer_;
        };
    private:
        GCTileMap* tMap_;
        GCPhysicsController* playerPhysics_;
        Simian::CTransform* playerTransform_;
        Simian::CTransform* phaseTwoTP_;
        Simian::CTransform* lightningBeamTransform_;
        Simian::CParticleSystem* teleportParticles_;
        Simian::CParticleSystem* deathParticles_;
        Simian::Entity* lightningBeam_;
        Simian::EntityList lightningCastEntityList_;
        GCAttributes* playerAtt_;
        Simian::CSoundEmitter* sounds_;
        GCSpawnSequence* phaseTwoSpawnSeq_, *phaseThreeSpawnSeq_;
        GCEventScript* reminderDodgeScript_;
        GCEventScript* reminderDemystScript_;
        bool hasAttacked_, playerHasAttacked_, enterMelee_, teleportChanceAdded_;
        bool playerDead_;
        Simian::u32 phase_, currentSpawner_;
        Simian::u32 attackAnimationsTotal_;
        Simian::u32 animationFrameRate_;
        Simian::u32 bulletCounter_, bulletMaxCount_, bulletMaxCountP1_, bulletMaxCountP2_, bulletMaxCountP3_;
        Simian::u32 lightningCounter_, lightningMaxCount_, lightningMaxCountP1_, lightningMaxCountP2_, lightningMaxCountP3_;
        Simian::s32 minHealth_;
        Simian::s32 lightningDamage_, beamDamage_;
        Simian::f32 minDistToLightning_, minDistToLightningSqr_;
        Simian::f32 chance_;
        Simian::f32 birthTime_, birthTimer_, deathTimer_;
        Simian::f32 idleTime_, idleTimer_;
        Simian::f32 coolDownTime_, coolDownTimeP1_, coolDownTimeP2_, coolDownTimeP3_,coolDownTimer_;
        Simian::f32 meleeTime_, meleeTeleportTime_, meleeHitDelay_, meleeTimer_;
        Simian::f32 phaseTwoTime_, phaseTwoTimer_;
        Simian::f32 lightningTime_, lightningTimeP1_, lightningTimeP2_, lightningTimeP3_,
                    lightningDelay_,lightningDelayP1_,lightningDelayP2_,lightningDelayP3_, lightningTimer_,lightningEndTimer_;
        Simian::f32 bulletDelay_, bulletDelayP1_, bulletDelayP2_, bulletDelayP3_, bulletTimer_;
        Simian::f32 teleportDelay_, teleportTimer_;
        Simian::f32 steerTimer_, beamTimer_;
        Simian::f32 teleportChance_, teleportChanceMax_, beamChance_, lightningChance_,lightningChanceP1_,lightningChanceP2_, lightningChanceP3_;
        Simian::f32 teleportChanceIncrease_;
        Simian::f32 meleeRadiusSqr_, meleeDist_;
        Simian::f32 meleeChaseDist_, meleeChaseRadiusSqr_;
        Simian::f32 meleeRadius_, meleeChaseRadius_, lightningAttackRadius_;
        Simian::f32 bulletStartHeight_;
        Simian::f32 minSpawnDist_, minSpawnRadiusSqr_;
        Simian::f32 startAngle_, finalAngle_;
        Simian::f32 spanAngle_,rotateSpanTime_;
        Simian::f32 lightningBeamWidth_;
        Simian::f32 beamSoundTimer_;
        Simian::f32 reminderTimer_;
        Simian::f32 reminderTimerMax_;
        Simian::u32 reminderPrev_;

        Simian::Vector2 oldMin_;
        Simian::Vector2 oldMax_;
        Simian::Vector3 startDir_, finalDir_;
        Simian::Vector3 nextTile_;
        Simian::Vector3 vecToPlayer_;
        std::string phaseTwoSpawner_, phaseThreeSpawner_;
        std::string attackAnimationPrefix_;
        std::string currentAttackAnimation_;
        std::vector<Simian::CTransform*> spawners_;
        std::vector<LightningEffect> lightningList_;
        static Simian::FactoryPlant<Simian::EntityComponent, GCAIBossGhost> factoryPlant_;
    public:
        Simian::u32 Phase() const;
        void Phase(Simian::u32 val);
    private:
        void update_(Simian::DelegateParameter& param);
        void birthOnEnter_(Simian::DelegateParameter& param);
        void birthOnUpdate_(Simian::DelegateParameter& param);
        void idleOnEnter_(Simian::DelegateParameter& param);
        void idleOnUpdate_(Simian::DelegateParameter& param);
        void meleeOnEnter_(Simian::DelegateParameter& param);
        void meleeOnUpdate_(Simian::DelegateParameter& param);
        void teleportOnEnter_(Simian::DelegateParameter& param);
        void teleportOnUpdate_(Simian::DelegateParameter& param);
        void seekOnEnter_(Simian::DelegateParameter& param);
        void seekOnUpdate_(Simian::DelegateParameter& param);
        void rangeOnEnter_(Simian::DelegateParameter& param);
        void rangeOnUpdate_(Simian::DelegateParameter& param);
        void lightningOnEnter_(Simian::DelegateParameter& param);
        void lightningOnUpdate_(Simian::DelegateParameter& param);
        void lightningOnExit_(Simian::DelegateParameter& param);
        void coolDownOnEnter_(Simian::DelegateParameter& param);
        void coolDownOnUpdate_(Simian::DelegateParameter& param);
        void deathOnEnter_(Simian::DelegateParameter& param);
        void deathOnUpdate_(Simian::DelegateParameter& param);
        void phaseTwoOnEnter_(Simian::DelegateParameter& param);
        void phaseTwoOnUpdate_(Simian::DelegateParameter& param);
        void phaseTwoOnExit_(Simian::DelegateParameter& param);
        void beamPhaseTwoOnEnter_( Simian::DelegateParameter& param );
        void beamPhaseTwoOnUpdate_( Simian::DelegateParameter& param );
        void beamPhaseTwoOnExit_( Simian::DelegateParameter& param );
        void phaseThreeOnEnter_(Simian::DelegateParameter& param);
        void phaseThreeOnUpdate_(Simian::DelegateParameter& param);
        void fleeOnEnter_(Simian::DelegateParameter& param);
        void fleeOnUpdate_(Simian::DelegateParameter& param);
        void lightningPhaseThreeOnEnter_( Simian::DelegateParameter& param );
        void lightningPhaseThreeOnUpdate_( Simian::DelegateParameter& param );
        void lightningPhaseThreeOnExit_( Simian::DelegateParameter& param );
        void overRideStateUpdate_( Simian::f32 dt );
        void helperStartAttack_();
        void meleeAttackDetection_();
        void facePlayer_();
        void updateVecToPlayer_();
        void setPosToRandSpawner_();
        void decideNextState_( Simian::u32 currentState );
        Simian::f32 vectorToDegreeFloat_(const Simian::Vector3& vec);
        void steer_( Simian::f32 dt );
        void updateNextTile_( const Simian::Vector3& target );
        bool reachedNextTile_( Simian::f32 dt );
        void takeDamage_( Simian::DelegateParameter& param );
        void postPhysics_(Simian::DelegateParameter& param);
        void createBullet_(Simian::f32 angle);
        void createLightning_();
        bool scaleInLightning_( Simian::Entity* ent, Simian::f32 timer, Simian::f32 delay);
        bool scaleOutLightning_( Simian::Entity* ent, Simian::f32 timer, Simian::f32 maxTime );
    public:
        GCAIBossGhost();

        void OnSharedLoad();
        void Deserialize( const Simian::FileObjectSubNode& data );
        void Serialize( Simian::FileObjectSubNode& data );
        void OnAwake();
        void OnInit();
        void OnDeinit();
    };
}
#endif
