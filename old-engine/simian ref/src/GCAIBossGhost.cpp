/******************************************************************************/
/*!
\file		GCAIBossGhost.cpp
\author 	Jonathan Butt, j.butt, 250000211
\par    	email: j.butt\@digipen.edu
\date   	2012/08/10
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/******************************************************************************/
#include "Simian/FiniteStateMachine.h"
#include "Simian/LogManager.h"
#include "Simian/EnginePhases.h"
#include "Simian/Math.h"
#include "Simian/AnimationController.h"
#include "Simian/CModel.h"
#include "Simian/CTransform.h"
#include "Simian/Scene.h"
#include "Simian/Interpolation.h"
#include "Simian/DebugRenderer.h"
#include "Simian/CSoundEmitter.h"
#include "Simian/CParticleSystem.h"
#include "Simian/ParticleSystem.h"

#include "GCPhysicsController.h"
#include "GCAIBossGhost.h"
#include "GCAttributes.h"
#include "GCPlayerLogic.h"
#include "GCTileMap.h"
#include "GCEditor.h"
#include "GCEnemyBulletLogic.h"
#include "GCSpawnSequence.h"
#include "GCEventScript.h"

#include "CombatCommandSystem.h"
#include "CombatCommandDealDamage.h"
#include "TileWallCollision.h"
#include "ComponentTypes.h"
#include "EntityTypes.h"
#include "GCUI.h"
#include "Simian/CBillboardRotate.h"
#include "GCFlamingSnakeEffect.h"

namespace Descension
{
    static const Simian::f32 MAX_STEER_TIME = 0.7f;
    static const Simian::f32 MAX_SHRINK_TIME = 1.7f;
    static const Simian::f32 BULLET_DIST_FROM_BOSS = 0.4f;
    static const Simian::f32 BEAM_SOUND_TIMER_MAX = 1.7f;
    static const Simian::u32 TOTAL_MELEE_GET_HIT_SOUNDS = 2;

    static const Simian::f32 DEATH_TOTAL_TIME = 4.0f;
    static const Simian::u32 DEATH_TOTAL_SNAKES = 12;

    static const Simian::u32 REMINDER_MAX_TIMES = 2;
    static const Simian::s32 REMINDER_MIN_HP = 40;

    Simian::FactoryPlant<Simian::EntityComponent, GCAIBossGhost> factoryPlant_(
        &Simian::GameFactory::Instance().ComponentFactory(), GC_AIBOSSGHOST);

    GCAIBossGhost::GCAIBossGhost()
    :   GCAIBase(BS_BIRTH,BS_TOTAL),
        tMap_(0),
        playerPhysics_(0),
        playerTransform_(0),
        sounds_(0),
        teleportParticles_(0),
        deathParticles_(0),
        reminderDodgeScript_(0),
        reminderDemystScript_(0),
        phaseTwoTP_(0),
        playerAtt_(0),
        lightningBeamTransform_(0),
        lightningBeam_(0),
        lightningCastEntityList_(0),
        phaseTwoSpawnSeq_(0),
        phaseThreeSpawnSeq_(0),
        hasAttacked_(false),
        playerHasAttacked_(false),
        enterMelee_(false),
        teleportChanceAdded_(false),
        playerDead_(false),
        phase_(BP_BIRTH),
        currentSpawner_(0),
        attackAnimationsTotal_(0),
        bulletCounter_(0),
        bulletMaxCount_(0),
        bulletMaxCountP3_(0),
        lightningCounter_(0),
        lightningMaxCount_(0),
        lightningMaxCountP3_(0),
        minHealth_(0),
        lightningDamage_(0),
        beamDamage_(0),
        beamSoundTimer_(0.0f),
        minDistToLightning_(0.0f),
        minDistToLightningSqr_(0.0f),
        lightningAttackRadius_(0.0f),
        chance_(0.0f),
        birthTime_(0.0f),
        deathTimer_(0),
        idleTime_(0.0f),
        coolDownTime_(0.0f),
        coolDownTimeP3_(0.0f),
        meleeTime_(0.0f),
        meleeTeleportTime_(0.0f),
        phaseTwoTime_(0.0f),
        lightningTime_(0.0f),
        meleeHitDelay_(0.0f),
        bulletDelay_(0.0f),
        bulletDelayP1_(0.0f),
        bulletDelayP2_(0.0f),
        bulletDelayP3_(0.0f),
        lightningDelay_(0.0f),
        lightningDelayP3_(0.0f),
        teleportDelay_(0.0f),
        birthTimer_(0.0f),
        idleTimer_(0.0f),
        coolDownTimer_(0.0f),
        meleeTimer_(0.0f),
        steerTimer_(0.0f),
        lightningEndTimer_(0.0f),
        bulletTimer_(0.0f),
        teleportTimer_(0.0f),
        lightningTimer_(0.0f),
        phaseTwoTimer_(0.0f),
        beamTimer_(0.0f),
        teleportChance_(0.0f),
        teleportChanceMax_(0.0f),
        lightningChance_(0.0f),
        beamChance_(0.0f),
        lightningChanceP2_(0.0f),
        lightningChanceP3_(0.0f),
        teleportChanceIncrease_(0.0f),
        meleeRadiusSqr_(0.0f),
        meleeDist_(0.0f),
        meleeChaseDist_(0.0f),
        meleeChaseRadiusSqr_(0.0f),
        meleeRadius_(0.0f),
        meleeChaseRadius_(0.0f),
        bulletStartHeight_(0.0f),
        minSpawnDist_(0.0f),
        minSpawnRadiusSqr_(0.0f),
        startAngle_(0.0f),
        finalAngle_(0.0f),
        spanAngle_(0.0f),
        rotateSpanTime_(0.0f),
        lightningBeamWidth_(0.0f),
        oldMin_(-1.f,-1.f),
        oldMax_(-1.f,-1.f),
        reminderTimer_(0),
        reminderTimerMax_(30.0f),
        reminderPrev_(0),
        startDir_(Simian::Vector3::Zero),
        finalDir_(Simian::Vector3::Zero),
        nextTile_(-1.0f,0.0f,-1.0f),
        vecToPlayer_(Simian::Vector3::Zero),
        phaseTwoSpawner_(""),
        attackAnimationPrefix_("Attack"),
        currentAttackAnimation_("")
    {

    }

    Simian::u32 GCAIBossGhost::Phase() const
    {
        return phase_;
    }

    void GCAIBossGhost::Phase( Simian::u32 val )
    {
        phase_ = val;
    }

    //--------------------------------------------------------------------------
    
    void GCAIBossGhost::OnSharedLoad()
    {
        RegisterCallback(Simian::P_POSTPHYSICS, Simian::Delegate::CreateDelegate<GCAIBossGhost, &GCAIBossGhost::postPhysics_>(this)); 
        RegisterCallback(Simian::P_UPDATE, Simian::Delegate::CreateDelegate<GCAIBossGhost, &GCAIBossGhost::update_>(this));
        GCAIBase::OnSharedLoad();
    }

    void GCAIBossGhost::Deserialize( const Simian::FileObjectSubNode& data )
    {
        GCAIBase::Deserialize(data);

        const Simian::FileDataSubNode* temp = data.Data("BirthTime");
        birthTime_ = temp ? temp->AsF32() : birthTime_;

        temp = data.Data("IdleTime");
        idleTime_ = temp ? temp->AsF32() : idleTime_;

        temp = data.Data("CoolDownTimeP1");
        coolDownTimeP1_ = temp ? temp->AsF32() : coolDownTimeP1_;

        temp = data.Data("CoolDownTimeP2");
        coolDownTimeP2_ = temp ? temp->AsF32() : coolDownTimeP2_;

        temp = data.Data("CoolDownTimeP3");
        coolDownTimeP3_ = temp ? temp->AsF32() : coolDownTimeP3_;

        temp = data.Data("MeleeTime");
        meleeTime_ = temp? temp->AsF32() : meleeTime_;

        temp = data.Data("MeleeTeleportTime");
        meleeTeleportTime_ = temp? temp->AsF32() : meleeTeleportTime_;

        temp = data.Data("PhaseTwoTime");
        phaseTwoTime_ = temp? temp->AsF32() : phaseTwoTime_;

        temp = data.Data("LightningTimeP1");
        lightningTimeP1_ = temp? temp->AsF32() : lightningTimeP1_;

        temp = data.Data("LightningTimeP2");
        lightningTimeP2_ = temp? temp->AsF32() : lightningTimeP2_;

        temp = data.Data("LightningTimeP3");
        lightningTimeP3_ = temp? temp->AsF32() : lightningTimeP3_;
        
        temp = data.Data("PhaseTwoRotationSpanTime");
        rotateSpanTime_ = temp? temp->AsF32() : rotateSpanTime_;
        
        //----------------------------------------------------------------------

        temp = data.Data("TeleportDelay");
        teleportDelay_ = temp? temp->AsF32() : teleportDelay_;

        temp = data.Data("MeleeHitDelay");
        meleeHitDelay_ = temp ? temp->AsF32() : meleeHitDelay_;

        temp = data.Data("BulletDelayP1");
        bulletDelayP1_ = temp? temp->AsF32() : bulletDelayP1_;

        temp = data.Data("BulletDelayP2");
        bulletDelayP2_ = temp? temp->AsF32() : bulletDelayP2_;

        temp = data.Data("BulletDelayP3");
        bulletDelayP3_ = temp? temp->AsF32() : bulletDelayP3_;

        temp = data.Data("LightningDelayP1");
        lightningDelayP1_ = temp? temp->AsF32() : lightningDelayP1_;

        temp = data.Data("LightningDelayP2");
        lightningDelayP2_ = temp? temp->AsF32() : lightningDelayP2_;

        temp = data.Data("LightningDelayP3");
        lightningDelayP3_ = temp? temp->AsF32() : lightningDelayP3_;

        //----------------------------------------------------------------------

        temp = data.Data("MeleeDist");
        meleeDist_ = temp? temp->AsF32() : meleeDist_;

        temp = data.Data("MeleeChaseDist");
        meleeChaseDist_ = temp? temp->AsF32() : meleeChaseDist_;

        temp = data.Data("LightningAttackRadius");
        lightningAttackRadius_ = temp? temp->AsF32() : lightningAttackRadius_;

        temp = data.Data("MinDistToLightning");
        minDistToLightning_ = temp? temp->AsF32() : minDistToLightning_;

        //----------------------------------------------------------------------
        
        temp = data.Data("BulletMaxCountP1");
        bulletMaxCountP1_ = temp? temp->AsU32() : bulletMaxCountP1_;

        temp = data.Data("BulletMaxCountP2");
        bulletMaxCountP2_ = temp? temp->AsU32() : bulletMaxCountP2_;

        temp = data.Data("BulletMaxCountP3");
        bulletMaxCountP3_ = temp? temp->AsU32() : bulletMaxCountP3_;
        
        temp = data.Data("LightningMaxCountP1");
        lightningMaxCountP1_ = temp? temp->AsU32() : lightningMaxCountP1_;

        temp = data.Data("LightningMaxCountP2");
        lightningMaxCountP2_ = temp? temp->AsU32() : lightningMaxCountP2_;

        temp = data.Data("LightningMaxCountP3");
        lightningMaxCountP3_ = temp? temp->AsU32() : lightningMaxCountP3_;
        
        if( lightningMaxCount_ )
            lightningList_.reserve(Simian::Math::Max(Simian::Math::Max(lightningMaxCountP1_,lightningMaxCountP2_),lightningMaxCountP3_));

        //----------------------------------------------------------------------

        temp = data.Data("LightningChanceP1");
        lightningChanceP1_ = temp ? temp->AsF32()/100.0f : lightningChanceP1_;

        temp = data.Data("LightningChanceP2");
        lightningChanceP2_ = temp ? temp->AsF32()/100.0f : lightningChanceP2_;

        temp = data.Data("LightningChanceP3");
        lightningChanceP3_ = temp ? temp->AsF32()/100.0f : lightningChanceP3_;

        temp = data.Data("BeamChance");
        beamChance_ = temp ? temp->AsF32()/100.0f : beamChance_;

        temp = data.Data("TeleportChanceMax");
        teleportChanceMax_ = temp ? temp->AsF32()/100.0f : teleportChanceMax_;
        
        temp = data.Data("TeleportChanceIncrease");
        teleportChanceIncrease_ = temp ? temp->AsF32()/100.0f : teleportChanceIncrease_;
        
        //----------------------------------------------------------------------

        temp = data.Data("AttackAnimationsTotal");
        attackAnimationsTotal_ = temp? static_cast<Simian::u32>(temp->AsF32()) : attackAnimationsTotal_;

        temp = data.Data("AttackAnimationPrefix");
        attackAnimationPrefix_ = temp? temp->AsString() : attackAnimationPrefix_;

        temp = data.Data("AnimationFrameRate");
        animationFrameRate_ = temp? static_cast<Simian::u32>(temp->AsF32()) : animationFrameRate_;

        temp = data.Data("BulletStartHeight");
        bulletStartHeight_ = temp? temp->AsF32() : bulletStartHeight_;

        temp = data.Data("MinSpawnDist");
        minSpawnDist_ = temp? temp->AsF32() : minSpawnDist_;

        //----------------------------------------------------------------------

        temp = data.Data("MinHealth");
        minHealth_ = temp? static_cast<Simian::s32>(temp->AsU32()) : minHealth_;

        temp = data.Data("PhaseTwoSpanAngle");
        spanAngle_ = temp? temp->AsF32() : spanAngle_;

        temp = data.Data("PhaseTwoSpawnerName");
        phaseTwoSpawner_ = temp? temp->AsString() : phaseTwoSpawner_;

        temp = data.Data("PhaseThreeSpawnerName");
        phaseThreeSpawner_ = temp? temp->AsString() : phaseThreeSpawner_;

        //----------------------------------------------------------------------

        temp = data.Data("LightningDamage");
        lightningDamage_ = temp? static_cast<Simian::s32>(temp->AsF32()) : lightningDamage_;

        temp = data.Data("BeamDamage");
        beamDamage_ = temp? static_cast<Simian::s32>(temp->AsF32()) : beamDamage_;


        data.Data("ReminderTimer", reminderTimer_, reminderTimer_);
        data.Data("ReminderTimerMax", reminderTimerMax_, reminderTimerMax_);
    }

    void GCAIBossGhost::Serialize( Simian::FileObjectSubNode& data )
    {
        data.AddData("BirthTime",birthTime_);
        data.AddData("IdleTime",idleTime_);
        data.AddData("CoolDownTimeP1",coolDownTimeP1_);
        data.AddData("CoolDownTimeP2",coolDownTimeP2_);
        data.AddData("CoolDownTimeP3",coolDownTimeP3_);
        data.AddData("MeleeTime",meleeTime_);
        data.AddData("PhaseTwoTime",phaseTwoTime_);
        data.AddData("LightningTimeP1",lightningTimeP1_);
        data.AddData("LightningTimeP2",lightningTimeP2_);
        data.AddData("LightningTimeP3",lightningTimeP3_);
        data.AddData("MeleeTeleportTime",meleeTeleportTime_);
        data.AddData("PhaseTwoRotationSpanTime",rotateSpanTime_);
        
        data.AddData("TeleportDelay",teleportDelay_);
        data.AddData("MeleeHitDelay",meleeHitDelay_);
        data.AddData("BulletDelayP1",bulletDelayP1_);
        data.AddData("BulletDelayP2",bulletDelayP2_);
        data.AddData("BulletDelayP3",bulletDelayP3_);
        data.AddData("LightningDelayP1",lightningDelayP1_);
        data.AddData("LightningDelayP2",lightningDelayP2_);
        data.AddData("LightningDelayP3",lightningDelayP3_);

        data.AddData("MeleeDist",meleeDist_);
        data.AddData("MeleeChaseDist",meleeChaseDist_);
        data.AddData("LightningAttackRadius",lightningAttackRadius_);
        data.AddData("MinDistToLightning",minDistToLightning_);

        data.AddData("BulletMaxCountP1",bulletMaxCountP1_);
        data.AddData("BulletMaxCountP2",bulletMaxCountP2_);
        data.AddData("BulletMaxCountP3",bulletMaxCountP3_);
        data.AddData("LightningMaxCountP1",lightningMaxCountP1_);
        data.AddData("LightningMaxCountP2",lightningMaxCountP2_);
        data.AddData("LightningMaxCountP3",lightningMaxCountP3_);

        data.AddData("LightningChanceP1",lightningChanceP1_*100.0f);
        data.AddData("LightningChanceP2",lightningChanceP2_*100.0f);
        data.AddData("LightningChanceP3",lightningChanceP3_*100.0f);
        data.AddData("BeamChance",beamChance_*100.0f);
        data.AddData("TeleportChanceIncrease",teleportChanceIncrease_*100.0f);
        data.AddData("TeleportChanceMax",teleportChanceMax_*100.0f);

        data.AddData("AttackAnimationsTotal",attackAnimationsTotal_);
        data.AddData("AttackAnimationPrefix",attackAnimationPrefix_);
        data.AddData("AnimationFrameRate",animationFrameRate_);
        data.AddData("BulletStartHeight",bulletStartHeight_);
        data.AddData("MinSpawnDist",minSpawnDist_);

        data.AddData("MinHealth",minHealth_);
        data.AddData("PhaseTwoSpawnerName",phaseTwoSpawner_);
        data.AddData("PhaseThreeSpawnerName",phaseThreeSpawner_);

        data.AddData("LightningDamage",lightningDamage_);
        data.AddData("BeamDamage",beamDamage_);
        
        data.AddData("ReminderTimer", reminderTimer_);
        data.AddData("ReminderTimerMax", reminderTimerMax_);

        GCAIBase::Serialize(data);
    }

    void GCAIBossGhost::OnAwake()
    {
        GCAIBase::OnAwake();

        fsm_[BS_BIRTH].OnEnter = Simian::Delegate::CreateDelegate<GCAIBossGhost, &GCAIBossGhost::birthOnEnter_>(this);
        fsm_[BS_BIRTH].OnUpdate = Simian::Delegate::CreateDelegate<GCAIBossGhost, &GCAIBossGhost::birthOnUpdate_>(this);
        fsm_[BS_IDLE].OnEnter = Simian::Delegate::CreateDelegate<GCAIBossGhost, &GCAIBossGhost::idleOnEnter_>(this);
        fsm_[BS_IDLE].OnUpdate = Simian::Delegate::CreateDelegate<GCAIBossGhost, &GCAIBossGhost::idleOnUpdate_>(this);
        fsm_[BS_SEEK].OnEnter = Simian::Delegate::CreateDelegate<GCAIBossGhost, &GCAIBossGhost::seekOnEnter_>(this);
        fsm_[BS_SEEK].OnUpdate = Simian::Delegate::CreateDelegate<GCAIBossGhost, &GCAIBossGhost::seekOnUpdate_>(this);
        fsm_[BS_MELEE].OnEnter = Simian::Delegate::CreateDelegate<GCAIBossGhost, &GCAIBossGhost::meleeOnEnter_>(this);
        fsm_[BS_MELEE].OnUpdate = Simian::Delegate::CreateDelegate<GCAIBossGhost, &GCAIBossGhost::meleeOnUpdate_>(this);
        fsm_[BS_TELEPORT].OnEnter = Simian::Delegate::CreateDelegate<GCAIBossGhost, &GCAIBossGhost::teleportOnEnter_>(this);
        fsm_[BS_TELEPORT].OnUpdate = Simian::Delegate::CreateDelegate<GCAIBossGhost, &GCAIBossGhost::teleportOnUpdate_>(this);
        fsm_[BS_RANGE].OnEnter = Simian::Delegate::CreateDelegate<GCAIBossGhost, &GCAIBossGhost::rangeOnEnter_>(this);
        fsm_[BS_RANGE].OnUpdate = Simian::Delegate::CreateDelegate<GCAIBossGhost, &GCAIBossGhost::rangeOnUpdate_>(this);
        fsm_[BS_LIGHTNING].OnEnter = Simian::Delegate::CreateDelegate<GCAIBossGhost, &GCAIBossGhost::lightningOnEnter_>(this);
        fsm_[BS_LIGHTNING].OnUpdate = Simian::Delegate::CreateDelegate<GCAIBossGhost, &GCAIBossGhost::lightningOnUpdate_>(this);
        fsm_[BS_LIGHTNING].OnExit = Simian::Delegate::CreateDelegate<GCAIBossGhost, &GCAIBossGhost::lightningOnExit_>(this);
        fsm_[BS_COOLDOWN].OnEnter = Simian::Delegate::CreateDelegate<GCAIBossGhost, &GCAIBossGhost::coolDownOnEnter_>(this);
        fsm_[BS_COOLDOWN].OnUpdate = Simian::Delegate::CreateDelegate<GCAIBossGhost, &GCAIBossGhost::coolDownOnUpdate_>(this);
        fsm_[BS_DEATH].OnEnter = Simian::Delegate::CreateDelegate<GCAIBossGhost, &GCAIBossGhost::deathOnEnter_>(this);
        fsm_[BS_DEATH].OnUpdate = Simian::Delegate::CreateDelegate<GCAIBossGhost, &GCAIBossGhost::deathOnUpdate_>(this);
        fsm_[BS_PHASETWO].OnEnter = Simian::Delegate::CreateDelegate<GCAIBossGhost, &GCAIBossGhost::phaseTwoOnEnter_>(this);
        fsm_[BS_PHASETWO].OnUpdate = Simian::Delegate::CreateDelegate<GCAIBossGhost, &GCAIBossGhost::phaseTwoOnUpdate_>(this);
        fsm_[BS_PHASETWO].OnExit = Simian::Delegate::CreateDelegate<GCAIBossGhost, &GCAIBossGhost::phaseTwoOnExit_>(this);
        fsm_[BS_PHASETHREE].OnEnter = Simian::Delegate::CreateDelegate<GCAIBossGhost, &GCAIBossGhost::phaseThreeOnEnter_>(this);
        fsm_[BS_PHASETHREE].OnUpdate = Simian::Delegate::CreateDelegate<GCAIBossGhost, &GCAIBossGhost::phaseThreeOnUpdate_>(this);
        fsm_[BS_FLEE].OnEnter = Simian::Delegate::CreateDelegate<GCAIBossGhost, &GCAIBossGhost::fleeOnEnter_>(this);
        fsm_[BS_FLEE].OnUpdate = Simian::Delegate::CreateDelegate<GCAIBossGhost, &GCAIBossGhost::fleeOnUpdate_>(this);
        fsm_[BS_LIGHTNING_PHASETHREE].OnEnter = Simian::Delegate::CreateDelegate<GCAIBossGhost, &GCAIBossGhost::lightningPhaseThreeOnEnter_>(this);
        fsm_[BS_LIGHTNING_PHASETHREE].OnUpdate = Simian::Delegate::CreateDelegate<GCAIBossGhost, &GCAIBossGhost::lightningPhaseThreeOnUpdate_>(this);
        fsm_[BS_LIGHTNING_PHASETHREE].OnExit = Simian::Delegate::CreateDelegate<GCAIBossGhost, &GCAIBossGhost::lightningPhaseThreeOnExit_>(this);
        fsm_[BS_BEAM_PHASETWO].OnEnter = Simian::Delegate::CreateDelegate<GCAIBossGhost, &GCAIBossGhost::beamPhaseTwoOnEnter_>(this);
        fsm_[BS_BEAM_PHASETWO].OnUpdate = Simian::Delegate::CreateDelegate<GCAIBossGhost, &GCAIBossGhost::beamPhaseTwoOnUpdate_>(this);
        fsm_[BS_BEAM_PHASETWO].OnExit = Simian::Delegate::CreateDelegate<GCAIBossGhost, &GCAIBossGhost::beamPhaseTwoOnExit_>(this);

        attributes_->DealDamageDelegates() += 
            Simian::Delegate::CreateDelegate<GCAIBossGhost, &GCAIBossGhost::takeDamage_>(this);
    }

    void GCAIBossGhost::OnInit()
    {
        GCAIBase::OnInit();
#ifdef _DESCENDED
        if (GCEditor::EditorMode())
            return;
#endif      
        
        playerAtt_ = GCPlayerLogic::Instance()->Attributes();
        GCPlayerLogic::Instance()->ParentEntity()->ParentEntity()->ComponentByType(GC_PHYSICSCONTROLLER,playerPhysics_);
        GCPlayerLogic::Instance()->ParentEntity()->ParentEntity()->ComponentByType(Simian::C_TRANSFORM,playerTransform_);   
        ComponentByType(Simian::C_SOUNDEMITTER, sounds_);

        ParentEntity()->ChildByName("TeleportParticles")->ComponentByType(Simian::C_PARTICLESYSTEM,teleportParticles_);
        ParentEntity()->ChildByName("DeathParticles")->ComponentByType(Simian::C_PARTICLESYSTEM,deathParticles_);
        
        lightningBeam_ = ParentEntity()->ParentScene()->CreateEntity(E_GHOSTLIGHTNINGBEAM);
        if( lightningBeam_ )
        {
            lightningBeam_->ComponentByType(Simian::C_TRANSFORM, lightningBeamTransform_);
            ParentScene()->AddEntity(lightningBeam_);
            if( lightningBeamTransform_ )
                lightningBeamTransform_->Position(-Simian::Vector3::UnitY);
        }

        if( playerPhysics_ )
        {
            Simian::f32 combinedRadius = playerPhysics_->Radius() + physics_->Radius();
            minSpawnRadiusSqr_ = minSpawnDist_ + combinedRadius;
            minSpawnRadiusSqr_ *= minSpawnRadiusSqr_;

            meleeRadius_ = meleeDist_ + combinedRadius;
            meleeRadiusSqr_ = meleeRadius_ * meleeRadius_;

            meleeChaseRadius_ = meleeChaseDist_ + combinedRadius;
            meleeChaseRadiusSqr_ = meleeChaseRadius_ * meleeChaseRadius_;

            minDistToLightning_ += playerPhysics_->Radius();
            minDistToLightningSqr_ = minDistToLightning_ * minDistToLightning_;

            if( lightningBeam_ )
                lightningBeamWidth_ = playerPhysics_->Radius() + 1.0f;
        }

        tMap_ = GCTileMap::TileMap(ParentScene());

        Simian::Entity* ent = Attributes()->ParentScene()->EntityByName("BossTeleportPoints");
        if( ent )
        {
            Simian::Entity* child = ent->ChildByIndex(0);
            Simian::u32 size = ent->Children().size();

            for( Simian::u32 i = 1; child && i <= size; ++i )
            {
                Simian::CTransform* trans = 0;
                child->ComponentByType(Simian::C_TRANSFORM,trans);
                if( trans )
                    spawners_.push_back(trans);

                if( i != size )
                    child = ent->ChildByIndex(i);
            }
        }
        ent = Attributes()->ParentScene()->EntityByName("BossPhase2Point");
        if( ent )
            ent->ComponentByType(Simian::C_TRANSFORM, phaseTwoTP_);

        ent = ParentScene()->EntityByNameRecursive(phaseTwoSpawner_);
        if( ent )
            ent->ComponentByType(GC_SPAWNSEQUENCE,phaseTwoSpawnSeq_);

        ent = ParentScene()->EntityByNameRecursive(phaseThreeSpawner_);
        if( ent )
            ent->ComponentByType(GC_SPAWNSEQUENCE,phaseThreeSpawnSeq_);

        // Getting the reminder dodge event script
        ent = ParentScene()->EntityByNameRecursive("BossDodgeReminder");
        if( ent )
            ent->ComponentByType(GC_EVENTSCRIPT,reminderDodgeScript_);

        // Getting the reminder demyst event script
        ent = ParentScene()->EntityByNameRecursive("BossDemystReminder");
        if( ent )
            ent->ComponentByType(GC_EVENTSCRIPT,reminderDemystScript_);
    }

    void GCAIBossGhost::OnDeinit()
    {
        GCAIBase::OnDeinit();
    }

    //-Private------------------------------------------------------------------

    void GCAIBossGhost::update_( Simian::DelegateParameter& param )
    {
#ifdef _DESCENDED
        if (GCEditor::EditorMode())
            return;
#endif

        DebugRendererDrawCircle(transform_->World().Position(),meleeRadius_,
            Simian::Color(0.0f, 1.0f, 0.0f));
        DebugRendererDrawCircle(transform_->World().Position(),meleeChaseRadius_,
            Simian::Color(0.0f, 0.0f, 1.0f));
        Simian::f32 dt = param.As<Simian::EntityUpdateParameter>().Dt;

        if( !playerDead_ && playerAtt_->Health() < Simian::Math::EPSILON )
        {
            attributes_->ForceSeek(false);
            if( fsm_.CurrentState() == BS_BEAM_PHASETWO )
                ParentScene()->RemoveEntity(lightningBeam_);
            fsm_.ChangeState(BS_IDLE);
            phase_ = BP_PHASE1;
            model_->Controller().PlayAnimation("Idle");
            playerDead_ = true;
            return;
        }
        else if((fsm_.CurrentState() != BS_DEATH) && (attributes_->Health() <= 0))
            fsm_.ChangeState(BS_DEATH);
        else if( teleportChance_ >= teleportChanceMax_ )
        {
            teleportChance_ = 0.0f;
            phaseTwoTimer_ = 0.0f;
            beamTimer_ = 0.0f;
            bulletTimer_ = 0.0f;

            // spawn little shits
            phaseTwoSpawnSeq_->ForceTrigger();
            // Play sound
            sounds_->Play(S_LAUGH);
            phase_ = BP_PHASE2;
            fsm_.ChangeState(BS_TELEPORT);
        }
        
        else if( ((phase_ == BP_PHASE1) || (phase_ == BP_PHASE2)) && attributes_->Health() < minHealth_ )
        {
            // spawn little shits
            phaseThreeSpawnSeq_->ForceTrigger();
            // Play sound
            sounds_->Play(S_LAUGH);
            fsm_.ChangeState(BS_PHASETHREE);
            setPosToRandSpawner_();
        }

        // Update phase 2 timer
        if( phase_ == BP_PHASE2 )
        {
            phaseTwoTimer_ += dt;
            if( phaseTwoTimer_ > phaseTwoTime_ )
            {
                phase_ = BP_PHASE1;
                lightningBeamTransform_->Position(-Simian::Vector3::UnitY);
                setPosToRandSpawner_();
                teleportChance_ = 0.0f;
                fsm_.ChangeState(BS_IDLE);
            }
        }

        // fix this shit
        fsm_.Update(dt);
        Simian::Vector3 vel(attributes_->Direction() * attributes_->MovementSpeed());
        physics_->Velocity(vel);

        // reminders
        if (reminderPrev_ != REMINDER_MAX_TIMES+1  && attributes_->Health() > REMINDER_MIN_HP)
        {
            reminderTimer_ += dt;
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

    void GCAIBossGhost::birthOnEnter_( Simian::DelegateParameter& )
    {
        birthTimer_ = birthTime_;
        attributes_->MovementSpeed(0);
        model_->Controller().PlayAnimation("Birth");
        GCUI::Instance()->StartBossBattle(this, GCUI::UI_BOSS0);
        attributes_->ForceSeek(false);
    }

    void GCAIBossGhost::birthOnUpdate_( Simian::DelegateParameter& param )
    {
        Simian::f32 dt = param.As<Simian::EntityUpdateParameter>().Dt;
        birthTimer_ -= dt;

        if(birthTimer_ <= 0)
        {
            phase_ = BP_PHASE1;
            fsm_.ChangeState(BS_IDLE);
        }
    }

    void GCAIBossGhost::idleOnEnter_( Simian::DelegateParameter& )
    {
        idleTimer_ = idleTime_;
        model_->Controller().PlayAnimation("Idle");
        attributes_->MovementSpeed(0);
        updateNextTile_(playerTransform_->World().Position());
    }

    void GCAIBossGhost::idleOnUpdate_( Simian::DelegateParameter& param )
    {
        Simian::f32 dt = param.As<Simian::EntityUpdateParameter>().Dt;
        idleTimer_ -= dt;

        updateVecToPlayer_();

        if( attributes_->ForceSeek() )
        {
            enterMelee_ = false;
            playerHasAttacked_ = false;
            fsm_.ChangeState(BS_SEEK);
            return;
        }
        
        if(playerAtt_->Health() && idleTimer_ <= 0)
        {
            // Teleport/Melee/Sky Lightning/Range

            chance_ = Simian::Math::Random(0.0f,1.0f);
            Simian::f32 vecToPlayerLengthSqr = vecToPlayer_.LengthSquared();
            if( phase_ == BP_PHASE1 )
            {
                lightningChance_ = lightningChanceP1_;
                if( vecToPlayerLengthSqr < meleeRadiusSqr_ )
                {
                    fsm_.ChangeState(BS_MELEE);
                    return;
                }
                else if( vecToPlayerLengthSqr < meleeChaseRadiusSqr_ )
                {
                    enterMelee_ = false;
                    playerHasAttacked_ = false;
                    fsm_.ChangeState(BS_SEEK);
                    return;
                }
            }
            else
                lightningChance_ = lightningChanceP2_;
            enterMelee_ = false;
            playerHasAttacked_ = false;
            if( !attributes_->ForceSeek() )
            {
                if( chance_ < lightningChance_ )   
                    fsm_.ChangeState(BS_LIGHTNING);
                else
                    fsm_.ChangeState(BS_RANGE);
            }
        }
    }

    void GCAIBossGhost::seekOnEnter_( Simian::DelegateParameter&  )
    {
        model_->Controller().PlayAnimation("Run");
        updateNextTile_(playerTransform_->World().Position());
        attributes_->MovementSpeed(attributes_->MovementSpeedMax());
    }

    void GCAIBossGhost::seekOnUpdate_( Simian::DelegateParameter& param )
    {
        Simian::f32 dt = param.As<Simian::EntityUpdateParameter>().Dt;
        updateVecToPlayer_();

        Simian::f32 vecToPlayerLenSqr = vecToPlayer_.LengthSquared();
        
        if( !attributes_->ForceSeek() && vecToPlayerLenSqr > meleeChaseRadiusSqr_ )
        {
            fsm_.ChangeState(BS_LIGHTNING);
            return;
        }
        if( vecToPlayerLenSqr < meleeRadiusSqr_ )
        {
            fsm_.ChangeState(BS_MELEE);
            return;
        }
        
        if( reachedNextTile_(dt) )
            updateNextTile_(playerTransform_->World().Position());
        steer_(dt);
    }

    void GCAIBossGhost::meleeOnEnter_( Simian::DelegateParameter& )
    {
        attributes_->ForceSeek(false);
        hasAttacked_ = false;
        helperStartAttack_();
        attributes_->MovementSpeed(0);
        facePlayer_();

        // Play sound
        sounds_->Play(S_MELEE_MISS);

        if(phase_ != BP_PHASE2)
        {
            chance_ = Simian::Math::Random(0.0f,1.0f);
            if( chance_ < teleportChance_ )
                fsm_.ChangeState(BS_TELEPORT);
            else if ( !enterMelee_ && !teleportChanceAdded_)
            {
                teleportChance_ += teleportChanceIncrease_ ;
                teleportChanceAdded_ = true;
            }
        }
        enterMelee_ = true;
        teleportTimer_ = 0.0f;
    }

    void GCAIBossGhost::meleeOnUpdate_( Simian::DelegateParameter& param )
    {
        Simian::f32 dt = param.As<Simian::EntityUpdateParameter>().Dt;        
        meleeTimer_ += dt;

        updateVecToPlayer_();
        if( vecToPlayer_.LengthSquared() < meleeRadiusSqr_ )
            teleportTimer_ += dt;
        
        steer_(dt);

        if (meleeTimer_ > meleeHitDelay_ && !hasAttacked_)
        {
            hasAttacked_ = true;
            meleeAttackDetection_();
            facePlayer_();
        }
        else if(meleeTimer_ > meleeTime_)
        {
            chance_ = Simian::Math::Random(0.0f, 1.0f);

            helperStartAttack_();
            if( chance_ < teleportChance_ || teleportTimer_ > meleeTeleportTime_ )
                fsm_.ChangeState(BS_TELEPORT);
            else if( vecToPlayer_.LengthSquared() > meleeRadiusSqr_ )
                fsm_.ChangeState(BS_IDLE);
        }
    }

    void GCAIBossGhost::teleportOnEnter_( Simian::DelegateParameter& )
    {
        model_->Controller().PlayAnimation("Teleport",false);
        teleportTimer_ = teleportDelay_;
        attributes_->MovementSpeed(0);
        // Set Teleport Chance Added to false to allow adding again everytime player enters melee radius
        teleportChanceAdded_ = false;

        // Play sound
        sounds_->Play(S_TELEPORT);

        // Play particles
        teleportParticles_->ParticleSystem()->Enabled(true);
    }

    void GCAIBossGhost::teleportOnUpdate_( Simian::DelegateParameter& param )
    {
        Simian::f32 dt = param.As<Simian::EntityUpdateParameter>().Dt;
        teleportTimer_ -= dt;

        if( teleportTimer_ <= 0 )
        {
            // Stop particles
            teleportParticles_->ParticleSystem()->Enabled(false);

            if( phase_ == BP_PHASE2 )
            {
                // set new pos
                physics_->Enabled(false);
                transform_->Position(phaseTwoTP_->Position());
                physics_->Enabled(true);

                // set up beam
                Simian::Vector3 current(transform_->Position());
                Simian::f32 mid = vectorToDegreeFloat_(Simian::Vector3(-1.0f, 0.0f, -1.0f));
                startAngle_ = mid + spanAngle_;
                finalAngle_ = mid - spanAngle_;
                model_->Controller().PlayAnimation("Cast Raise", false);
                fsm_.ChangeState(BS_PHASETWO);
                return;
            }
            setPosToRandSpawner_();
            if( phase_ == BP_PHASE3 )
                fsm_.ChangeState(BS_PHASETHREE);
            else
                fsm_.ChangeState(BS_IDLE);
        }
    }

    void GCAIBossGhost::rangeOnEnter_( Simian::DelegateParameter& )
    {
        bulletCounter_ = 0;
        bulletTimer_ = 0.0f;
        attributes_->MovementSpeed(0);
        model_->Controller().PlayAnimation("Shoot Raise",false);
        facePlayer_();
        bulletMaxCount_ = (phase_ == BP_PHASE1)? bulletMaxCountP1_ : 
                          (phase_ == BP_PHASE2)? bulletMaxCountP2_ : bulletMaxCountP3_;
        bulletDelay_ = (phase_ == BP_PHASE1)? bulletDelayP1_ : 
                       (phase_ == BP_PHASE2)? bulletDelayP2_ : bulletDelayP3_;
        bulletMaxCount_ = (phase_ == BP_PHASE1)? bulletMaxCountP1_ : 
                          (phase_ == BP_PHASE2)? bulletMaxCountP2_ : bulletMaxCountP3_;
    }

    void GCAIBossGhost::rangeOnUpdate_( Simian::DelegateParameter& param )
    {
        Simian::f32 dt = param.As<Simian::EntityUpdateParameter>().Dt;
        bulletTimer_ += dt;
        updateVecToPlayer_();

        transform_->Rotation(Simian::Vector3(0, 1, 0), Simian::Degree(vectorToDegreeFloat_(vecToPlayer_.Normalized()) + 90.0f));
        
        if( vecToPlayer_.LengthSquared() < meleeRadiusSqr_ )
        {
            fsm_.ChangeState(BS_MELEE);
            return;
        }

        if( bulletTimer_ > bulletDelay_ )
        {
            model_->Controller().PlayAnimation("Cast Down",false);
            createBullet_(vectorToDegreeFloat_(vecToPlayer_));
            ++bulletCounter_;
            bulletTimer_ = 0.0f;
            facePlayer_();
        }
        if( bulletCounter_ >= bulletMaxCount_ )
            fsm_.ChangeState(BS_COOLDOWN);
    }

    void GCAIBossGhost::lightningOnEnter_( Simian::DelegateParameter& )
    {
        model_->Controller().PlayAnimation("Lightning", false);
        attributes_->MovementSpeed(0);
        lightningTimer_ = 0.0f;
        lightningEndTimer_ = 0.0f;
        facePlayer_();
        hasAttacked_ = false;
        lightningDelay_ = (phase_ == BP_PHASE1)? lightningDelayP1_ : lightningDelayP2_;
        lightningTime_ = (phase_ == BP_PHASE1)? lightningTimeP1_ : lightningTimeP2_;
        lightningMaxCount_ = (phase_ == BP_PHASE1)? lightningMaxCountP1_ : lightningMaxCountP2_;
        createLightning_();

        // Play sound
        sounds_->Play(S_PRE_LIGHTNING);

        std::vector<LightningEffect>::iterator it = lightningList_.begin();
        for( Simian::u32 i = 0; i < lightningMaxCount_; ++i, ++it)
        {    
            Simian::CTransform* trans = 0;
            it->castDecal_->ComponentByType(Simian::C_TRANSFORM,trans);
            trans->Scale(Simian::Vector3::Zero);
            ParentScene()->AddEntity(it->castDecal_);
        }
    }

    void GCAIBossGhost::lightningOnUpdate_( Simian::DelegateParameter& param )
    {
        Simian::f32 dt = param.As<Simian::EntityUpdateParameter>().Dt;
        lightningTimer_ += dt;

        facePlayer_();
        steer_(dt);

        // Add decals
        if( lightningTimer_ < lightningDelay_ )
        {
            ParentScene()->EntitiesByType(E_LIGHTNINGCAST, lightningCastEntityList_);
            for( Simian::u32 i = 0; i < lightningCastEntityList_.size(); ++i )
                scaleInLightning_(lightningCastEntityList_[i],lightningTimer_,lightningDelay_);
        }
        
        // Adds lightning
        // Check for hit
        else if( !hasAttacked_ && lightningTimer_ > lightningDelay_ )
        {
            Simian::Vector3 player = playerTransform_->World().Position();
            // Play sound
            sounds_->Play(S_LIGHTNING);

            std::vector<LightningEffect>::iterator it = lightningList_.begin();
            for( Simian::u32 i = 0; i < lightningMaxCount_; ++i, ++it)
            {
                ParentScene()->AddEntity(it->effect_);
                Simian::f32 dist = (it->transform_->Position() - player).LengthSquared();
                if( dist < minDistToLightning_ )
                {
                    CombatCommandSystem::Instance().AddCommand(attributes_, playerAtt_,
                        CombatCommandDealDamage::Create(lightningDamage_));
                }
            }
            hasAttacked_ = true;
        }

        // Scale down decal
        bool exitState = false;
        if( hasAttacked_ )
        {
            lightningEndTimer_ += dt;
            for( Simian::u32 i = 0; i < lightningCastEntityList_.size(); ++i )
                exitState = scaleOutLightning_(lightningCastEntityList_[i], lightningEndTimer_,MAX_SHRINK_TIME);
        }
        if( lightningTimer_ > lightningTime_ && exitState )
            fsm_.ChangeState(BS_COOLDOWN);
    }

    void GCAIBossGhost::lightningOnExit_( Simian::DelegateParameter&  )
    {
        Simian::EntityList list;
        ParentScene()->EntitiesByType(E_LIGHTNINGCAST,list);
        for( Simian::u32 i = 0; i < list.size(); ++i )
            ParentScene()->RemoveEntity(list[i]);
    }

    void GCAIBossGhost::coolDownOnEnter_( Simian::DelegateParameter& )
    {
        coolDownTimer_ = (phase_ == BP_PHASE1) ? coolDownTimeP1_ : 
                         (phase_ == BP_PHASE2) ? coolDownTimeP2_ : coolDownTimeP3_ ;
        attributes_->MovementSpeed(0);
        model_->Controller().PlayAnimation("Idle");
    }

    void GCAIBossGhost::coolDownOnUpdate_( Simian::DelegateParameter& param )
    {
        Simian::f32 dt = param.As<Simian::EntityUpdateParameter>().Dt;
        coolDownTimer_ -= dt;

        if(coolDownTimer_ <= 0)
        {
            if( phase_ == BP_PHASE3 )
                fsm_.ChangeState(BS_PHASETHREE);
            else if( phase_ == BP_PHASE2 )
            {
                SVerbose(coolDownTimeP2_);
                fsm_.ChangeState(BS_PHASETWO);
            }
            else
                fsm_.ChangeState(BS_IDLE);
        }
    }

    void GCAIBossGhost::deathOnEnter_( Simian::DelegateParameter& )
    {
        model_->Controller().PlayAnimation("Die",false);
        attributes_->MovementSpeed(0);
        GCUI::Instance()->EndBossBattle();
        physics_->Enabled(false);

        // Kill all enemies once dead
        // Stop spawn script
        phaseTwoSpawnSeq_->KillSpawnedEnemies();
        phaseThreeSpawnSeq_->KillSpawnedEnemies();
        phaseTwoSpawnSeq_->ForceStop();
        phaseThreeSpawnSeq_->ForceStop();

        // Play sounds
        sounds_->Play(S_DIE_1);
        sounds_->Play(S_DIE_2);

        // Play particles
        deathParticles_->ParticleSystem()->Enabled(true);
    }

    void GCAIBossGhost::deathOnUpdate_( Simian::DelegateParameter& param )
    {
        Simian::f32 dt = param.As<Simian::EntityUpdateParameter>().Dt;

        // Stop particles
        if (deathTimer_ > DEATH_TOTAL_TIME)
        {
            deathParticles_->ParticleSystem()->Enabled(false);
            teleportParticles_->ParticleSystem()->Enabled(false);

            // Spawn fire snakes!
            Simian::CTransform* snakeTransform_;
            GCFlamingSnakeEffect* snakeFlame_;
            for (Simian::u32 i = 0; i < DEATH_TOTAL_SNAKES; ++i)
            {
                Simian::Entity* snake = ParentEntity()->ParentScene()->CreateEntity(E_FLAMINGSNAKEEFFECT);
                ParentEntity()->ParentScene()->AddEntity(snake);
                snake->ComponentByType(Simian::C_TRANSFORM, snakeTransform_);
                snakeTransform_->Position(transform_->Position());
                snake->ComponentByType(GC_FLAMINGSNAKEEFFECT, snakeFlame_);

                Simian::f32 randomAngle = Simian::Math::Random(0.0f, 2.0f*Simian::Math::PI);
                snakeFlame_->Direction(Simian::Vector3(std::cos(randomAngle), 0, std::sin(randomAngle)));
            }

            model_->Visible(false);
            Simian::CModel* shadowModel;
            ParentEntity()->ChildByName("Shadow")->ComponentByType(Simian::C_MODEL, shadowModel);
            shadowModel->Visible(false);

            deathTimer_ = -5.0f;
        }
        else if (deathTimer_ > -1.0f)
            deathTimer_ += dt;
    }

    void GCAIBossGhost::phaseTwoOnEnter_( Simian::DelegateParameter& )
    {
    }

    void GCAIBossGhost::phaseTwoOnUpdate_( Simian::DelegateParameter& param )
    {
        Simian::f32 dt = param.As<Simian::EntityUpdateParameter>().Dt;
        steerTimer_ += dt;
        bulletTimer_ += dt;

        chance_ = Simian::Math::Random(0.0f,1.0f);
        chance_ -= beamChance_;
        if( chance_ < Simian::Math::EPSILON )
        {
            fsm_.ChangeState(BS_BEAM_PHASETWO);
            return;
        }
        chance_ -= lightningChanceP2_;
        if( chance_ < Simian::Math::EPSILON  )
            fsm_.ChangeState(BS_LIGHTNING);
        else
            fsm_.ChangeState(BS_RANGE);
    }

    void GCAIBossGhost::phaseTwoOnExit_( Simian::DelegateParameter&  )
    {
        lightningBeamTransform_->Position(-Simian::Vector3::UnitY);
    }

    void GCAIBossGhost::beamPhaseTwoOnEnter_( Simian::DelegateParameter&  )
    {
        model_->Controller().PlayAnimation("Cast Raise", false);
        beamSoundTimer_ = BEAM_SOUND_TIMER_MAX;
    }

    void GCAIBossGhost::beamPhaseTwoOnUpdate_( Simian::DelegateParameter& param )
    {
        Simian::f32 dt = param.As<Simian::EntityUpdateParameter>().Dt;
        beamTimer_ += dt;

        // used to delay damage when hit
        bulletTimer_ += dt;

        if( beamTimer_ > bulletDelayP2_ )
        {
            lightningBeamTransform_->Position(Simian::Vector3(transform_->Position().X(),1.0f,transform_->Position().Z()));
            // Play sound
            beamSoundTimer_ += dt;
            if (beamSoundTimer_ > BEAM_SOUND_TIMER_MAX)
            {
                beamSoundTimer_ = 0.0f;
                sounds_->Play(S_BEAM);
            }

            Simian::f32 angle = 0.0f;
            if( beamTimer_<= rotateSpanTime_  )
            {
                Simian::f32 intp = beamTimer_/rotateSpanTime_;
                intp = Simian::Math::Clamp(intp, 0.0f, 1.0f);
                angle = Simian::Interpolation::Interpolate
                    <Simian::f32>(startAngle_,finalAngle_,intp,Simian::Interpolation::Linear);
                transform_->Rotation(Simian::Vector3(0, 1, 0), Simian::Degree(angle + 90.0f));
                lightningBeamTransform_->Rotation(Simian::Vector3(0, 1, 0), Simian::Degree(angle + 90.0f));
            }
            else
            {
                beamTimer_ = 0.0f;
                Simian::f32 temp = startAngle_;
                startAngle_ = finalAngle_;
                finalAngle_ = temp;
                fsm_.ChangeState(BS_COOLDOWN);
            }
            updateVecToPlayer_();
            Simian::f32 dotProduct = transform_->Direction().Dot(vecToPlayer_.Normalized());

            if( (bulletTimer_ > bulletDelay_ ) && 1.0f - dotProduct < Simian::Math::EPSILON )
            {
                CombatCommandSystem::Instance().AddCommand(attributes_, playerAtt_,
                    CombatCommandDealDamage::Create(beamDamage_));
                bulletTimer_ = 0.0f;
            }
        }
    }

    void GCAIBossGhost::beamPhaseTwoOnExit_( Simian::DelegateParameter& )
    {
        lightningBeamTransform_->Position(Simian::Vector3(-Simian::Vector3::UnitY));
    }

    void GCAIBossGhost::phaseThreeOnEnter_( Simian::DelegateParameter&  )
    {
        phase_ = BP_PHASE3;
        steerTimer_ = 0.0f;

        facePlayer_();
    }

    void GCAIBossGhost::phaseThreeOnUpdate_( Simian::DelegateParameter&  )
    {
        chance_ = Simian::Math::Random(0.0f,1.0f);

        updateVecToPlayer_();
        Simian::f32 sqrDist = vecToPlayer_.LengthSquared();

        if( sqrDist < meleeRadiusSqr_ )
            fsm_.ChangeState(BS_TELEPORT);
        else if( sqrDist < meleeChaseRadiusSqr_ )
            fsm_.ChangeState(BS_FLEE);
        else if( chance_ < lightningChanceP3_ )   
            fsm_.ChangeState(BS_LIGHTNING_PHASETHREE);
        else
            fsm_.ChangeState(BS_RANGE);
    }

    void GCAIBossGhost::fleeOnEnter_( Simian::DelegateParameter& )
    {
        model_->Controller().PlayAnimation("Run");
        attributes_->MovementSpeed(attributes_->MovementSpeedMax());
        steerTimer_ = 0.0f;
    }

    void GCAIBossGhost::fleeOnUpdate_( Simian::DelegateParameter& )
    {
        if( physics_->Collided() )
            fsm_.ChangeState(BS_TELEPORT);
        else
        {
            Simian::Vector3 norm( -vecToPlayer_.Normalized() );
            attributes_->Direction(norm);
            transform_->Rotation(Simian::Vector3(0, 1, 0), Simian::Degree(vectorToDegreeFloat_(norm) + 90.0f));
            fsm_.ChangeState(BS_PHASETHREE);
        }
    }

    void GCAIBossGhost::lightningPhaseThreeOnEnter_( Simian::DelegateParameter& )
    {
        model_->Controller().PlayAnimation("Lightning", false);
        attributes_->MovementSpeed(0);
        lightningTimer_ = 0.0f;
        lightningEndTimer_ = 0.0f;
        steerTimer_ = 0.0f;
        lightningCounter_ = 0;
        hasAttacked_ = false;

        // Play sound
        sounds_->Play(S_PRE_LIGHTNING);

        if( lightningList_.size() )
            lightningList_.clear();
    }

    void GCAIBossGhost::lightningPhaseThreeOnUpdate_( Simian::DelegateParameter& param )
    {
        Simian::f32 dt = param.As<Simian::EntityUpdateParameter>().Dt;
        lightningTimer_ += dt;
        lightningEndTimer_ += dt;
        facePlayer_();
        steer_(dt);

        if( lightningEndTimer_ > lightningTimeP3_ )
        {
            fsm_.ChangeState(BS_PHASETHREE);
            return;
        }
        for( Simian::u32 i = 0; i < lightningList_.size(); ++i )
        {
            LightningEffect* light = &lightningList_[i];
            light->timer_ += dt;
            if( light->timer_ < lightningDelayP3_ )
                scaleInLightning_(light->castDecal_,lightningTimer_,lightningDelayP3_);
            else if( light->timer_ > lightningDelayP3_ )
            {
                scaleOutLightning_(light->castDecal_,light->timer_ - lightningDelayP3_,MAX_SHRINK_TIME);
            
                Simian::Vector3 player = playerTransform_->World().Position();
            
                Simian::f32 dist = (light->transform_->Position() - player).LengthSquared();
                if( !light->damaged_ && dist < minDistToLightning_ )
                {
                    CombatCommandSystem::Instance().AddCommand(attributes_, playerAtt_,
                            CombatCommandDealDamage::Create(lightningDamage_));
                    light->damaged_ = true;
                }
            }
        }
        if( hasAttacked_ && lightningTimer_ > lightningDelayP3_ )
        {
            LightningEffect* light = &lightningList_[lightningCounter_];
            Simian::CTransform* effTrans = 0;
            light->effect_ = ParentScene()->CreateEntity(E_BOSSGHOSTLIGHTNING);
            light->effect_->ComponentByType(Simian::C_TRANSFORM, effTrans);
            effTrans->Position(light->transform_->Position());
            ParentScene()->AddEntity(light->effect_);
            ++lightningCounter_;
            lightningTimer_ = 0.0f;
            hasAttacked_ = false;
        }

        // Add effect
        if( !hasAttacked_ && lightningTimer_ < lightningDelayP3_ )
        {
            LightningEffect temp;
            temp.castDecal_ = ParentScene()->CreateEntity(E_LIGHTNINGCAST);
            temp.castDecal_->ComponentByType(Simian::C_TRANSFORM,temp.transform_);
            temp.timer_ = 0.0f;
            temp.damaged_ = false;
            Simian::Vector3 pos(playerTransform_->World().Position());
            temp.transform_->Position(Simian::Vector3(pos.X(),0.05f,pos.Z()));
            temp.transform_->Scale(Simian::Vector3::Zero);
            ParentScene()->AddEntity(temp.castDecal_);
            lightningList_.push_back(temp);
            steerTimer_ = 0.0f;
            hasAttacked_ = true;

            // Play sound
            sounds_->Play(S_LIGHTNING);
        }
    }

    void GCAIBossGhost::lightningPhaseThreeOnExit_( Simian::DelegateParameter&  )
    {
        Simian::EntityList list;
        ParentScene()->EntitiesByType(E_LIGHTNINGCAST,list);
        for(Simian::u32 i = 0; i < list.size(); ++i)
            ParentScene()->RemoveEntity(list[i]);
    }

    void GCAIBossGhost::overRideStateUpdate_( Simian::f32 dt )
    {
        dt;
    }

    void GCAIBossGhost::helperStartAttack_()
    {
        std::stringstream tempSstream;
        tempSstream << attackAnimationPrefix_ << " " << rand()%attackAnimationsTotal_+1;
        currentAttackAnimation_ = tempSstream.str();
        steerTimer_ = 0.0f;
        meleeTimer_ = 0.0f;
        hasAttacked_ = false;

        model_->Controller().PlayAnimation(currentAttackAnimation_ + " Raise", false);
    }

    void GCAIBossGhost::meleeAttackDetection_()
    {
        Simian::f32 attackAngle = ((attributes_->AttackAngle()/2.0f) * Simian::Math::PI) / 180.0f;
        updateVecToPlayer_();
        // Get distance vector to player
        Simian::f32 eToPLength = vecToPlayer_.Length();
        model_->Controller().PlayAnimation(currentAttackAnimation_ + " Down", false);

        // Check if distance to enemy is within attack range
        if (eToPLength*eToPLength <= meleeRadiusSqr_)
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
                sounds_->Play(S_MELEE_HIT);
            }
        }
    }

    void GCAIBossGhost::facePlayer_()
    {
        updateVecToPlayer_();
        finalDir_ = vecToPlayer_;
        startDir_ = attributes_->Direction();
    }

    void GCAIBossGhost::updateVecToPlayer_()
    {
        vecToPlayer_ = playerTransform_->World().Position() - transform_->World().Position();
    }

    Simian::f32 GCAIBossGhost::vectorToDegreeFloat_(const Simian::Vector3& vec)
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
    void GCAIBossGhost::setPosToRandSpawner_()
    {
        physics_->Enabled(false);
        
        Simian::Vector3 player = playerTransform_->World().Position();
        
        std::vector<Simian::CTransform*> candidates;
        candidates.reserve(spawners_.size());
        
        for( Simian::u32 i = 0; i < spawners_.size(); ++i )
        {
            Simian::f32 distToPlayer = (player - spawners_[i]->Position()).LengthSquared();
            if( distToPlayer > minSpawnRadiusSqr_ )
                candidates.push_back(spawners_[i]);
        }

        if( candidates.size() )
        {
            Simian::u32 rand = Simian::Math::Random(0, candidates.size());
            transform_->Position(candidates[rand]->Position());
            currentSpawner_ = rand;
        }
        physics_->Enabled(true);
    }

    void GCAIBossGhost::steer_( Simian::f32 dt )
    {
        steerTimer_ += dt;
        
        if(steerTimer_<= MAX_STEER_TIME)
        {
            Simian::f32 intp = steerTimer_/MAX_STEER_TIME;
            intp = Simian::Math::Clamp(intp, 0.0f, 1.0f);
            Attributes()->Direction(Simian::Interpolation::Interpolate
                <Simian::Vector3>(startDir_,finalDir_,intp,Simian::Interpolation::EaseSquareOut));
        }

        Simian::f32 angle;
        if(Attributes()->Direction().LengthSquared() > Simian::Math::EPSILON)
        {    
            angle = vectorToDegreeFloat_(Attributes()->Direction().Normalized());
            transform_->Rotation(Simian::Vector3(0, 1, 0), Simian::Degree(angle + 90.0f));
        }
    }

    void GCAIBossGhost::updateNextTile_( const Simian::Vector3& target )
	{
        nextTile_ = PathFinder::Instance().GeneratePath(physics_->Radius(), meleeChaseRadiusSqr_, 
            transform_->World().Position(),target);
		Simian::Vector3 dir = nextTile_ - transform_->Position();

		if(dir.LengthSquared() < Simian::Math::EPSILON)
			finalDir_ = dir;
		else
			finalDir_ = dir.Normalized();

		steerTimer_ = 0.0f;
		startDir_ = Attributes()->Direction();
	}

    bool GCAIBossGhost::reachedNextTile_( Simian::f32 dt )
    {
        Simian::f32 futureDist = attributes_->MovementSpeed() * dt;
        futureDist *= futureDist;
        Simian::Vector3 currentDist(nextTile_ - transform_->World().Position());
        Simian::f32 dist = currentDist.LengthSquared();
        return ((dist < Simian::Math::EPSILON) || (dist > futureDist));
    }

    void GCAIBossGhost::takeDamage_( Simian::DelegateParameter& param )
    {
        GCAttributes::DealDamageParameter& p = param.As<GCAttributes::DealDamageParameter>();
        if(p.Damage > attributes_->Health())
            fsm_.ChangeState(BS_DEATH);
        else if( phase_ == BP_PHASE1 )
        {
            if( !playerHasAttacked_ )
            {
                teleportChance_ += teleportChanceIncrease_;
                playerHasAttacked_ = true;
            }
        }

        // Play random sound
        if (sounds_)
            sounds_->Play(Simian::Math::Random(S_MELEE_GET_HIT_1, S_MELEE_GET_HIT_1 + TOTAL_MELEE_GET_HIT_SOUNDS));
    }

    void GCAIBossGhost::postPhysics_( Simian::DelegateParameter&  )
    {
#ifdef _DESCENDED
        if (GCEditor::EditorMode())
            return;
#endif
        if(fsm_.CurrentState() == BS_DEATH)
            PathFinder::Instance().UnsetPosition(oldMin_,oldMax_);
        else
            PathFinder::Instance().SetNewPosition(transform_->World().Position(),physics_->Radius(),oldMin_,oldMax_);
    }

    void GCAIBossGhost::createBullet_(Simian::f32 angle)
{
        // Fire bullet
        Simian::Entity* bullet = ParentEntity()->ParentScene()->CreateEntity(E_ELECTROBULLET);
        ParentEntity()->ParentScene()->AddEntity(bullet);
        // Pass bullet attribs
        GCEnemyBulletLogic* enemyBullet_;
        bullet->ComponentByType(GC_ENEMYBULLETLOGIC, enemyBullet_);
        enemyBullet_->OwnerAttribs(attributes_);
        
        updateVecToPlayer_();
        // Set bullet transform
        Simian::CTransform* bulletTransform_;
        Simian::Vector3 radius = vecToPlayer_.Normalized() * BULLET_DIST_FROM_BOSS;
        bullet->ComponentByType(Simian::C_TRANSFORM, bulletTransform_);
        bulletTransform_->Position(Simian::Vector3(transform_->World().Position().X() + radius.X(),
            transform_->World().Position().Y() + bulletStartHeight_, transform_->World().Position().Z() + radius.Z()));
        // Set its velocity
        enemyBullet_->MovementVec(Simian::Vector3(std::cos(Simian::Degree(Simian::Math::TAU - angle - 5.0f).Radians()),
            0, std::sin(Simian::Degree(Simian::Math::TAU - angle - 5.0f).Radians())));

        // Play sound
        sounds_->Play(S_SHOOT);
    }

    void GCAIBossGhost::createLightning_()
    {
        lightningList_.clear();
        Simian::f32 x = playerTransform_->World().Position().X();
        Simian::f32 y = playerTransform_->World().Position().Z();
        Simian::f32 minX = x - lightningAttackRadius_;
        Simian::f32 minY = y - lightningAttackRadius_;
        Simian::f32 maxX = x + lightningAttackRadius_;
        Simian::f32 maxY = y + lightningAttackRadius_;

        for( Simian::u32 i = 0; i < lightningMaxCount_; ++i )
        {
            LightningEffect temp;
            Simian::CTransform* trans = 0;
            temp.effect_ = ParentScene()->CreateEntity(E_BOSSGHOSTLIGHTNING);
            temp.castDecal_ = ParentScene()->CreateEntity(E_LIGHTNINGCAST);
            temp.castDecal_->ComponentByType(Simian::C_TRANSFORM,trans);
            temp.effect_->ComponentByType(Simian::C_TRANSFORM, temp.transform_);
            temp.transform_->Position(Simian::Vector3(Simian::Math::Random(minX,maxX),0.0f,Simian::Math::Random(minY,maxY)));

            Simian::Vector3 pos(temp.transform_->Position());
            trans->Position(Simian::Vector3(pos.X(),0.1f,pos.Z()));
            
            lightningList_.push_back(temp);
        }
    }

    bool GCAIBossGhost::scaleInLightning_( Simian::Entity* ent, Simian::f32 timer, Simian::f32 delay )
    {
        if( ent )
        {
            Simian::f32 intp = timer/delay;
            intp = Simian::Math::Clamp(intp, 0.0f, 1.0f);
            Simian::f32 scale = Simian::Interpolation::Interpolate
                <Simian::f32>(0.0f,minDistToLightning_,intp,Simian::Interpolation::EaseCubicOut);

            Simian::CTransform* trans = 0;
            ent->ComponentByType(Simian::C_TRANSFORM, trans);
            if(trans)
                trans->Scale(Simian::Vector3(scale,scale,scale));

            return scale == minDistToLightning_;
        }
        return false;
    }

    bool GCAIBossGhost::scaleOutLightning_( Simian::Entity* ent, Simian::f32 timer, Simian::f32 maxTime )
    {
        if( ent )
        {
            Simian::f32 intp = timer/maxTime;
            intp = Simian::Math::Clamp(intp, 0.0f, 1.0f);
            Simian::f32 scale = Simian::Interpolation::Interpolate
                <Simian::f32>(minDistToLightningSqr_,0.0f,intp,Simian::Interpolation::EaseCubicOut);

            Simian::CTransform* trans = 0;
            ent->ComponentByType(Simian::C_TRANSFORM, trans);
            if(trans)
                trans->Scale(Simian::Vector3(scale,scale,scale));

            return scale < Simian::Math::EPSILON;
        }
        return false;
    }
}
