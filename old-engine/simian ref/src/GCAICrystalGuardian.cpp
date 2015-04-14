/******************************************************************************/
/*!
\file		GCAICrystalGuardian.cpp
\author 	Jonathan Butt, j.butt, 250000211
\par    	email: j.butt\@digipen.edu
\date   	2012/12/07
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/******************************************************************************/
#include "Simian/Math.h"
#include "Simian/Entity.h"
#include "Simian/EngineComponents.h"
#include "Simian/EntityComponent.h"
#include "Simian/Scene.h"
#include "Simian/EnginePhases.h"
#include "Simian/LogManager.h"
#include "Simian/Interpolation.h"
#include "Simian/FileArraySubNode.h"
#include "Simian/FileObjectSubNode.h"
#include "Simian/CSoundEmitter.h"

#include "GCAttributes.h"
#include "GCAICrystalGuardian.h"

#include "EntityTypes.h"
#include "ComponentTypes.h"

#include "GCEditor.h"
#include "GCPlayerLogic.h"
#include "CombatCommandSystem.h"
#include "CombatCommandDealDamage.h"
#include "GCAICrystalBossFist.h"
#include "GCMajenTriangle.h"
#include "GCFlamingSnakeEffect.h"
#include "GCDescensionCamera.h"
#include "GCRockDebris.h"
#include "GCEventScript.h"
#include "GCPhysicsController.h"
#include "GCSpawnSequence.h"
#include "GCCrystalSwap.h"
#include "Simian/Delegate.h"

namespace Descension
{
    const Simian::u32 RECOVER_SPAWN_AMOUNT = 1;
    const Simian::f32 RECOVER_SPAWN_RADIUS = 2.0f;
    const Simian::u32 TOTAL_GROWL_SOUNDS = 3;
    const Simian::u32 TOTAL_GETHIT_SOUNDS = 4;
    const Simian::u32 TOTAL_HURTGROWL_SOUNDS = 2;

    const std::string GCAICrystalGuardian::STUN_FLINCH_ANIM_TABLE[2]
    = {
        "Stun Flinch 1",
        "Stun Flinch 2"
      };

    const std::string GCAICrystalGuardian::ANIMATION_TABLE[CG_TOTAL] 
    = { 
        "Birth",                    //CG_BIRTH
        "Idle",                     //CG_IDLE
        "",                         //CG_CHOOSESKILL

        "Stun Enter",               //CG_STUNENTER
        "Stun Loop",                //CG_STUN
        "Stun End",                 //CG_STUNEXIT

        "Ground Pound Left Raise",  //CG_POUNDLCHARGE
        "Ground Pound Left Down",   //CG_POUNDL
        "Ground Pound Left Back",   //CG_POUNDL_CD 
        "Ground Pound Right Raise", //CG_POUNDRCHARGE
        "Ground Pound Right Down",  //CG_POUNDR
        "Ground Pound Right Back",  //CG_POUNDR_CD

        "Punch Left Raise",         //CG_PUNCHLCHARGE
        "Punch Left Down",          //CG_PUNCHL
        "Idle",                     //CG_PUNCHL_CD
        "Punch Right Raise",        //CG_PUNCHRCHARGE
        "Punch Right Down",         //CG_PUNCHR
        "Idle",                     //CG_PUNCHR_CD

        "Fire Lightning Beam Raise",//CG_BEAMCHARGE
        "Fire Lightning Beam",      //CG_BEAM
        "Fire Lightning Beam Down", //CG_BEAMDOWN
        "Idle",                     //CG_BEAM_CD

        "Cast Sky Lightning Raise", //CG_LIGHTNINGCHARGE
        "Cast Sky Lightning Loop",  //CG_LIGHTNING
        "Cast Sky Lightning Down",  //CG_LIGHTNING_CD

        "Slam Center Raise",        //CG_FIRESLAMCHARGE
        "Slam Center Down",         //CG_FIRESLAM
        "",                         //CG_FIRESLAM_CD

        "Cast Fire Triangles",      //CG_TRIANGLESCHARGE
        "",                         //CG_TRIANGLES
        "Idle",                     //CG_TRIANGLES_CD

        "Roar",                     //CG_ROCKFALL
        "Idle",                     //CG_ROCKFALL_CD

        "Die"                       //CG_DEATH
    };

    static const std::string BOSS_FLINCH_1 = "Stun Flinch 1";
    static const std::string BOSS_FLINCH_2 = "Stun Flinch 2";
    static const Simian::u32 TOTAL_PHASES = 3;
    static const Simian::u32 NUM_ATTACKS_P1 = 2;
    static const Simian::u32 NUM_LIGHTNING_CAST = 3;
    static const Simian::u32 NUM_TRIANGLES = 3;
    static const Simian::f32 LIGHTNING_TARGET_TIME = 0.5f;

    Simian::FactoryPlant<Simian::EntityComponent, GCAICrystalGuardian> GCAICrystalGuardian::factoryPlant_(
        &Simian::GameFactory::Instance().ComponentFactory(), GC_AICRYSTALGUARDIAN);
                                 
    GCAICrystalGuardian::GCAICrystalGuardian()
        :   GCAIBase(CG_BIRTH,CG_TOTAL),
            dead_(false),
            doneDamage_(false),
            attacked_(false),
            playerAttribs_(0),
            model_(0),  
            physics_(0),
            rightHand_(0),
            leftHand_(0),
            stunEventScript_(0),
            dieEventScript_(0),
            gauntletSpawnSeq_(0),
            sounds_(0),
            effTrans_(0),
            effEnt_(0),
            currAttackData_(0),
            currPhase_(1),
            lightningMaxCount_(0),
            lightningCount_(0),
            fireSnakeCount_(0),
            fireSnakeDamage_(0),
            fireTriCount_(0),
            rockFallCount_(0),
            rockFallCounter_(0),
            timer_(0),
            lightningDelay_(0),
            lightningTargetChargeTime_(0),
            lightningTargetTime_(0),
            lightningDelayTimer_(0),
            rockFallMinSpawnDelay_(0),
            rockFallMaxSpawnDelay_(0),
            rockFallSpawnTimer_(0),
            rockDebrisRadius_(0),
            bossTriggerPos_(Simian::Vector3::Zero)
    {
    }

    void GCAICrystalGuardian::OnSharedLoad()
    {
         RegisterCallback(Simian::P_UPDATE, Simian::Delegate::CreateDelegate<GCAICrystalGuardian, &GCAICrystalGuardian::update_>(this));
         GCAIBase::OnSharedLoad();
    }

    void GCAICrystalGuardian::OnInit()
    {
#ifdef _DESCENDED
        if (GCEditor::EditorMode())
            return;
#endif
        GCAIBase::OnInit();
        ParentEntity()->ComponentByType(Simian::C_MODEL,model_);
        ParentEntity()->ComponentByType(Simian::C_SOUNDEMITTER,sounds_);
        ParentEntity()->ComponentByType(GC_PHYSICSCONTROLLER,physics_);
        ParentEntity()->ComponentByType(GC_ATTRIBUTES,attributes_);
        
        Simian::Entity* leftHand = ParentScene()->CreateEntity(E_ENEMYCRYSTALGUARDIANFIST);
        ParentScene()->AddEntity(leftHand);
        model_->AttachEntityToJoint("L_Wrist", leftHand);

        Simian::Entity* rightHand = ParentScene()->CreateEntity(E_ENEMYCRYSTALGUARDIANFIST);
        ParentScene()->AddEntity(rightHand);
        model_->AttachEntityToJoint("R_Wrist", rightHand);

        leftHand->ComponentByType(GC_AICRYSTALGUARDIANFIST,leftHand_);
        rightHand->ComponentByType(GC_AICRYSTALGUARDIANFIST,rightHand_);

        ParentEntity()->ChildByName("Physical Crystal Left")->ChildByIndex(0)->ComponentByType(Simian::C_MODEL,phases_[P_PHYSICAL].leftCrystal_);
        ParentEntity()->ChildByName("Physical Crystal Right")->ChildByIndex(0)->ComponentByType(Simian::C_MODEL,phases_[P_PHYSICAL].rightCrystal_);

        ParentEntity()->ChildByName("Fire Crystal Left")->ChildByIndex(0)->ComponentByType(Simian::C_MODEL,phases_[P_FIRE].leftCrystal_);
        ParentEntity()->ChildByName("Fire Crystal Right")->ChildByIndex(0)->ComponentByType(Simian::C_MODEL,phases_[P_FIRE].rightCrystal_);
        
        ParentEntity()->ChildByName("Lightning Crystal Left")->ChildByIndex(0)->ComponentByType(Simian::C_MODEL,phases_[P_LIGHTNING].leftCrystal_);
        ParentEntity()->ChildByName("Lightning Crystal Right")->ChildByIndex(0)->ComponentByType(Simian::C_MODEL,phases_[P_LIGHTNING].rightCrystal_);

        ParentEntity()->ChildByName("All Crystal Left")->ChildByIndex(0)->ComponentByType(Simian::C_MODEL,phases_[P_ALL].leftCrystal_);
        ParentEntity()->ChildByName("All Crystal Right")->ChildByIndex(0)->ComponentByType(Simian::C_MODEL,phases_[P_ALL].rightCrystal_);
        playerAttribs_ = GCPlayerLogic::Instance()->Attributes();
        
        phases_[P_FIRE].leftCrystal_->Visible(false);
        phases_[P_FIRE].rightCrystal_->Visible(false);
        phases_[P_LIGHTNING].leftCrystal_->Visible(false);
        phases_[P_LIGHTNING].rightCrystal_->Visible(false);
        phases_[P_ALL].leftCrystal_->Visible(false);
        phases_[P_ALL].rightCrystal_->Visible(false);

        attributes_->MaxHealth(phases_[currPhase_].health_);
        attributes_->Health(phases_[currPhase_].health_);
        attributes_->DealDamageDelegates() += 
            Simian::Delegate::CreateDelegate<GCAICrystalGuardian, &GCAICrystalGuardian::onTakeDamage_>(this);
        
        effEnt_ = ParentEntity()->ParentScene()->CreateEntity(E_GHOSTLIGHTNINGBEAM);

        Simian::Entity* ent = ParentScene()->EntityByName("GameplayElements")->ChildByName("Boss_Cine_1_Trigger");
        Simian::CTransform* trans;
        ent->ComponentByType(Simian::C_TRANSFORM,trans);
        bossTriggerPos_ = trans->WorldPosition();

        if( effEnt_ )
        {
            effEnt_->ComponentByType(Simian::C_TRANSFORM, effTrans_);

            ParentScene()->AddEntity(effEnt_);
            if( effTrans_)
            {
                effTrans_->Position(-Simian::Vector3::UnitY);
                effTrans_->Rotation(Simian::Vector3(0, 1, 0), Simian::Degree(-135.0f));
                effTrans_->Scale(Simian::Vector3(2,2,2));
            }
        }

        // Getting the stun event script
        ent = ParentScene()->EntityByNameRecursive("BossStunEvent");
        if( ent )
            ent->ComponentByType(GC_EVENTSCRIPT,stunEventScript_);

        // Getting the die event script
        ent = ParentScene()->EntityByNameRecursive("BossDieEvent");
        if( ent )
            ent->ComponentByType(GC_EVENTSCRIPT,dieEventScript_);

        // Getting the spawn sequence
        ent = Attributes()->ParentScene()->EntityByNameRecursive("GauntletSpawner");
        if( ent )
            ent->ComponentByType(GC_SPAWNSEQUENCE, gauntletSpawnSeq_);
    }

    void GCAICrystalGuardian::OnDeinit()
    {
        //GCAIBase::OnDeinit();
        //if( effEnt_ )
        //    ParentScene()->RemoveEntity(effEnt_);
    }

    void GCAICrystalGuardian::OnAwake()
    {
#ifdef _DESCENDED
        if (GCEditor::EditorMode())
            return;
#endif
        GCAIBase::OnAwake();
        
        fsm_[CG_BIRTH].OnEnter = 
            Simian::Delegate::CreateDelegate<GCAICrystalGuardian, &GCAICrystalGuardian::castingEnter_<CG_BIRTH,false> >(this);
        fsm_[CG_BIRTH].OnUpdate = 
            Simian::Delegate::CreateDelegate<GCAICrystalGuardian, &GCAICrystalGuardian::castingUpdate_<CG_IDLE> >(this);
        fsm_[CG_BIRTH].OnExit = 
            Simian::Delegate::CreateDelegate<GCAICrystalGuardian, &GCAICrystalGuardian::stateOnExit_ >(this);
        
        fsm_[CG_IDLE].OnEnter = 
            Simian::Delegate::CreateDelegate<GCAICrystalGuardian, &GCAICrystalGuardian::idleOnEnter_ >(this);
        fsm_[CG_IDLE].OnUpdate = 
            Simian::Delegate::CreateDelegate<GCAICrystalGuardian, &GCAICrystalGuardian::idleOnUpdate_ >(this);

        fsm_[CG_CHOOSESKILL].OnUpdate = 
            Simian::Delegate::CreateDelegate<GCAICrystalGuardian, &GCAICrystalGuardian::chooseSkillOnUpdate_>(this);

        fsm_[CG_STUNENTER].OnEnter = 
            Simian::Delegate::CreateDelegate<GCAICrystalGuardian, &GCAICrystalGuardian::castingEnter_<CG_STUNENTER,false> >(this);
        fsm_[CG_STUNENTER].OnUpdate = 
            Simian::Delegate::CreateDelegate<GCAICrystalGuardian, &GCAICrystalGuardian::stunEnterOnUpdate_ >(this);

        fsm_[CG_STUN].OnEnter = 
            Simian::Delegate::CreateDelegate<GCAICrystalGuardian, &GCAICrystalGuardian::castingEnter_<CG_STUN,true> >(this);
        fsm_[CG_STUN].OnUpdate = 
            Simian::Delegate::CreateDelegate<GCAICrystalGuardian, &GCAICrystalGuardian::castingUpdate_<CG_STUNEXIT> >(this);

        fsm_[CG_STUNEXIT].OnEnter = 
            Simian::Delegate::CreateDelegate<GCAICrystalGuardian, &GCAICrystalGuardian::castingEnter_<CG_STUNEXIT,false> >(this);
        fsm_[CG_STUNEXIT].OnUpdate = 
            Simian::Delegate::CreateDelegate<GCAICrystalGuardian, &GCAICrystalGuardian::castingUpdate_<CG_IDLE> >(this);
        fsm_[CG_STUNEXIT].OnExit = 
            Simian::Delegate::CreateDelegate<GCAICrystalGuardian, &GCAICrystalGuardian::stunExitOnExit_ >(this);
       
        fsm_[CG_POUNDLCHARGE].OnEnter = 
            Simian::Delegate::CreateDelegate<GCAICrystalGuardian, &GCAICrystalGuardian::castingEnter_<CG_POUNDLCHARGE,false> >(this);
        fsm_[CG_POUNDLCHARGE].OnUpdate = 
            Simian::Delegate::CreateDelegate<GCAICrystalGuardian, &GCAICrystalGuardian::castingUpdate_<CG_POUNDL> >(this);
    
        fsm_[CG_POUNDL].OnEnter = 
            Simian::Delegate::CreateDelegate<GCAICrystalGuardian, &GCAICrystalGuardian::poundLOnEnter_ >(this);
        fsm_[CG_POUNDL].OnUpdate = 
            Simian::Delegate::CreateDelegate<GCAICrystalGuardian, &GCAICrystalGuardian::poundLOnUpdate_ >(this);

        fsm_[CG_POUNDL_CD].OnEnter = 
            Simian::Delegate::CreateDelegate<GCAICrystalGuardian, &GCAICrystalGuardian::castingEnter_<CG_POUNDL_CD,false> >(this);
        fsm_[CG_POUNDL_CD].OnUpdate = 
            Simian::Delegate::CreateDelegate<GCAICrystalGuardian, &GCAICrystalGuardian::poundCDOnUpdate_ >(this);

        fsm_[CG_POUNDRCHARGE].OnEnter = 
            Simian::Delegate::CreateDelegate<GCAICrystalGuardian, &GCAICrystalGuardian::castingEnter_<CG_POUNDRCHARGE,false> >(this);
        fsm_[CG_POUNDRCHARGE].OnUpdate = 
            Simian::Delegate::CreateDelegate<GCAICrystalGuardian, &GCAICrystalGuardian::castingUpdate_<CG_POUNDR> >(this);

        fsm_[CG_POUNDR].OnEnter = 
            Simian::Delegate::CreateDelegate<GCAICrystalGuardian, &GCAICrystalGuardian::poundROnEnter_ >(this);
        fsm_[CG_POUNDR].OnUpdate = 
            Simian::Delegate::CreateDelegate<GCAICrystalGuardian, &GCAICrystalGuardian::poundROnUpdate_ >(this);

        fsm_[CG_POUNDR_CD].OnEnter = 
            Simian::Delegate::CreateDelegate<GCAICrystalGuardian, &GCAICrystalGuardian::castingEnter_<CG_POUNDR_CD,false> >(this);
        fsm_[CG_POUNDR_CD].OnUpdate = 
            Simian::Delegate::CreateDelegate<GCAICrystalGuardian, &GCAICrystalGuardian::poundCDOnUpdate_ >(this);

        fsm_[CG_PUNCHLCHARGE].OnEnter = 
            Simian::Delegate::CreateDelegate<GCAICrystalGuardian, &GCAICrystalGuardian::castingEnter_<CG_PUNCHLCHARGE,false> >(this);
        fsm_[CG_PUNCHLCHARGE].OnUpdate = 
            Simian::Delegate::CreateDelegate<GCAICrystalGuardian, &GCAICrystalGuardian::castingUpdate_<CG_PUNCHL> >(this);

        fsm_[CG_PUNCHL].OnEnter = 
            Simian::Delegate::CreateDelegate<GCAICrystalGuardian, &GCAICrystalGuardian::punchLOnEnter_ >(this);
        fsm_[CG_PUNCHL].OnUpdate = 
            Simian::Delegate::CreateDelegate<GCAICrystalGuardian, &GCAICrystalGuardian::punchLOnUpdate_ >(this);

        fsm_[CG_PUNCHL_CD].OnEnter = 
            Simian::Delegate::CreateDelegate<GCAICrystalGuardian, &GCAICrystalGuardian::castingEnter_<CG_PUNCHL_CD,true> >(this);
        fsm_[CG_PUNCHL_CD].OnUpdate = 
            Simian::Delegate::CreateDelegate<GCAICrystalGuardian, &GCAICrystalGuardian::punchCDOnUpdate_ >(this);
    
        fsm_[CG_PUNCHRCHARGE].OnEnter = 
            Simian::Delegate::CreateDelegate<GCAICrystalGuardian, &GCAICrystalGuardian::castingEnter_<CG_PUNCHRCHARGE,false> >(this);
        fsm_[CG_PUNCHRCHARGE].OnUpdate = 
            Simian::Delegate::CreateDelegate<GCAICrystalGuardian, &GCAICrystalGuardian::castingUpdate_<CG_PUNCHR> >(this);

        fsm_[CG_PUNCHR].OnEnter = 
            Simian::Delegate::CreateDelegate<GCAICrystalGuardian, &GCAICrystalGuardian::punchROnEnter_ >(this);
        fsm_[CG_PUNCHR].OnUpdate = 
            Simian::Delegate::CreateDelegate<GCAICrystalGuardian, &GCAICrystalGuardian::punchROnUpdate_ >(this);

        fsm_[CG_PUNCHR_CD].OnEnter = 
            Simian::Delegate::CreateDelegate<GCAICrystalGuardian, &GCAICrystalGuardian::castingEnter_<CG_PUNCHR_CD,true> >(this);
        fsm_[CG_PUNCHR_CD].OnUpdate = 
            Simian::Delegate::CreateDelegate<GCAICrystalGuardian, &GCAICrystalGuardian::punchCDOnUpdate_ >(this);
    
        fsm_[CG_BEAMCHARGE].OnEnter = 
            Simian::Delegate::CreateDelegate<GCAICrystalGuardian, &GCAICrystalGuardian::castingEnter_<CG_BEAMCHARGE,false> >(this);
        fsm_[CG_BEAMCHARGE].OnUpdate = 
            Simian::Delegate::CreateDelegate<GCAICrystalGuardian, &GCAICrystalGuardian::castingUpdate_<CG_BEAM> >(this);

        fsm_[CG_BEAM].OnEnter = 
            Simian::Delegate::CreateDelegate<GCAICrystalGuardian, &GCAICrystalGuardian::beamOnEnter_>(this);
        fsm_[CG_BEAM].OnUpdate = 
            Simian::Delegate::CreateDelegate<GCAICrystalGuardian, &GCAICrystalGuardian::beamOnUpdate_>(this);
        fsm_[CG_BEAM].OnExit = 
            Simian::Delegate::CreateDelegate<GCAICrystalGuardian, &GCAICrystalGuardian::beamOnExit_>(this);
        
        fsm_[CG_BEAMDOWN].OnEnter = 
            Simian::Delegate::CreateDelegate<GCAICrystalGuardian, &GCAICrystalGuardian::castingEnter_<CG_BEAMDOWN,false> >(this);
        fsm_[CG_BEAMDOWN].OnUpdate = 
            Simian::Delegate::CreateDelegate<GCAICrystalGuardian, &GCAICrystalGuardian::castingUpdate_<CG_BEAM_CD> >(this);

        fsm_[CG_BEAM_CD].OnEnter = 
            Simian::Delegate::CreateDelegate<GCAICrystalGuardian, &GCAICrystalGuardian::castingEnter_<CG_BEAM_CD,true> >(this);
        fsm_[CG_BEAM_CD].OnUpdate = 
            Simian::Delegate::CreateDelegate<GCAICrystalGuardian, &GCAICrystalGuardian::castingUpdate_<CG_IDLE> >(this);

        fsm_[CG_LIGHTNINGCHARGE].OnEnter = 
            Simian::Delegate::CreateDelegate<GCAICrystalGuardian, &GCAICrystalGuardian::lightningChargeOnEnter_ >(this);
        fsm_[CG_LIGHTNINGCHARGE].OnUpdate = 
            Simian::Delegate::CreateDelegate<GCAICrystalGuardian, &GCAICrystalGuardian::lightningChargeOnUpdate_ >(this);

        fsm_[CG_LIGHTNING].OnEnter = 
            Simian::Delegate::CreateDelegate<GCAICrystalGuardian, &GCAICrystalGuardian::lightningOnEnter_ >(this);
        fsm_[CG_LIGHTNING].OnUpdate = 
            Simian::Delegate::CreateDelegate<GCAICrystalGuardian, &GCAICrystalGuardian::lightningOnUpdate_>(this);
        fsm_[CG_LIGHTNING].OnExit = 
            Simian::Delegate::CreateDelegate<GCAICrystalGuardian, &GCAICrystalGuardian::lightningOnExit_>(this);

        fsm_[CG_LIGHTNING_CD].OnEnter = 
            Simian::Delegate::CreateDelegate<GCAICrystalGuardian, &GCAICrystalGuardian::castingEnter_<CG_LIGHTNING_CD,false> >(this);
        fsm_[CG_LIGHTNING_CD].OnUpdate = 
            Simian::Delegate::CreateDelegate<GCAICrystalGuardian, &GCAICrystalGuardian::castingUpdate_<CG_IDLE> >(this);
        fsm_[CG_LIGHTNING_CD].OnExit = 
            Simian::Delegate::CreateDelegate<GCAICrystalGuardian, &GCAICrystalGuardian::stateOnExit_ >(this);

        fsm_[CG_FIRESLAMCHARGE].OnEnter = 
            Simian::Delegate::CreateDelegate<GCAICrystalGuardian, &GCAICrystalGuardian::fireSlamChargeOnEnter_ >(this);
        fsm_[CG_FIRESLAMCHARGE].OnUpdate = 
            Simian::Delegate::CreateDelegate<GCAICrystalGuardian, &GCAICrystalGuardian::castingUpdate_<CG_FIRESLAM> >(this);

        fsm_[CG_FIRESLAM].OnEnter = 
            Simian::Delegate::CreateDelegate<GCAICrystalGuardian, &GCAICrystalGuardian::fireSlamOnEnter_>(this);
        fsm_[CG_FIRESLAM].OnUpdate = 
            Simian::Delegate::CreateDelegate<GCAICrystalGuardian, &GCAICrystalGuardian::castingUpdate_<CG_FIRESLAM_CD> >(this);
        fsm_[CG_FIRESLAM].OnExit = 
            Simian::Delegate::CreateDelegate<GCAICrystalGuardian, &GCAICrystalGuardian::fireSlamOnExit_>(this);

        fsm_[CG_FIRESLAM_CD].OnEnter = 
            Simian::Delegate::CreateDelegate<GCAICrystalGuardian, &GCAICrystalGuardian::fireSlamCDOnEnter_ >(this);
        fsm_[CG_FIRESLAM_CD].OnUpdate = 
            Simian::Delegate::CreateDelegate<GCAICrystalGuardian, &GCAICrystalGuardian::castingUpdate_<CG_IDLE> >(this);
        fsm_[CG_FIRESLAM_CD].OnExit = 
            Simian::Delegate::CreateDelegate<GCAICrystalGuardian, &GCAICrystalGuardian::stateOnExit_ >(this);

        fsm_[CG_TRIANGLESCHARGE].OnEnter = 
            Simian::Delegate::CreateDelegate<GCAICrystalGuardian, &GCAICrystalGuardian::trianglesChargeOnEnter_ >(this);
        fsm_[CG_TRIANGLESCHARGE].OnUpdate = 
            Simian::Delegate::CreateDelegate<GCAICrystalGuardian, &GCAICrystalGuardian::castingUpdate_<CG_TRIANGLES> >(this);

        fsm_[CG_TRIANGLES].OnEnter = 
            Simian::Delegate::CreateDelegate<GCAICrystalGuardian, &GCAICrystalGuardian::trianglesOnEnter_ >(this);
        fsm_[CG_TRIANGLES].OnUpdate = 
            Simian::Delegate::CreateDelegate<GCAICrystalGuardian, &GCAICrystalGuardian::castingUpdate_<CG_TRIANGLES_CD> >(this);

        fsm_[CG_TRIANGLES_CD].OnEnter = 
            Simian::Delegate::CreateDelegate<GCAICrystalGuardian, &GCAICrystalGuardian::castingEnter_<CG_TRIANGLES_CD,true> >(this);
        fsm_[CG_TRIANGLES_CD].OnUpdate = 
            Simian::Delegate::CreateDelegate<GCAICrystalGuardian, &GCAICrystalGuardian::castingUpdate_<CG_IDLE> >(this);

        fsm_[CG_ROCKFALL].OnEnter = 
            Simian::Delegate::CreateDelegate<GCAICrystalGuardian, &GCAICrystalGuardian::rockFallOnEnter_ >(this);
        fsm_[CG_ROCKFALL].OnUpdate = 
            Simian::Delegate::CreateDelegate<GCAICrystalGuardian, &GCAICrystalGuardian::rockFallOnUpdate_ >(this);

        fsm_[CG_ROCKFALL_CD].OnEnter = 
            Simian::Delegate::CreateDelegate<GCAICrystalGuardian, &GCAICrystalGuardian::castingEnter_<CG_TRIANGLES_CD,true> >(this);
        fsm_[CG_ROCKFALL_CD].OnUpdate = 
            Simian::Delegate::CreateDelegate<GCAICrystalGuardian, &GCAICrystalGuardian::castingUpdate_<CG_IDLE> >(this);

        fsm_[CG_DEATH].OnEnter = 
            Simian::Delegate::CreateDelegate<GCAICrystalGuardian, &GCAICrystalGuardian::castingEnter_<CG_DEATH,false> >(this);
        fsm_[CG_DEATH].OnUpdate = 
            Simian::Delegate::CreateDelegate<GCAICrystalGuardian, &GCAICrystalGuardian::deathOnUpdate_ >(this);
     }

    void GCAICrystalGuardian::Serialize( Simian::FileObjectSubNode& data )
    {
        GCAIBase::Serialize(data);
        for (int i = 0; i < 3; ++i)
        {
            std::string name = "Phase";
            name += static_cast<char>('1' + i);
            serializePhases_(i,data.AddObject(name));
        }
        data.AddData("BirthTime", timerTable_[CG_BIRTH]);
        data.AddData("IdleTime", timerTable_[CG_IDLE]);
        data.AddData("DeathTime", timerTable_[CG_DEATH]);
        
        data.AddData("StunEnterTime",timerTable_[CG_STUNENTER]);
        data.AddData("StunTime",timerTable_[CG_STUN]);
        data.AddData("StunExitTime",timerTable_[CG_STUNEXIT]);
        
        data.AddData("PoundChargeTime", timerTable_[CG_POUNDLCHARGE]);
        data.AddData("PoundAttackTime", timerTable_[CG_POUNDL]);
        data.AddData("PoundCoolDownTime", timerTable_[CG_POUNDL_CD]);
        
        data.AddData("PunchChargeTime", timerTable_[CG_PUNCHLCHARGE]);
        data.AddData("PunchAttackTime", timerTable_[CG_PUNCHL]);
        data.AddData("PunchCoolDownTime", timerTable_[CG_PUNCHL_CD]);
        
        data.AddData("FireSlamChargeTime", timerTable_[CG_FIRESLAMCHARGE]);
        data.AddData("FireSlamAttackTime", timerTable_[CG_FIRESLAM]);
        data.AddData("FireSlamCoolDownTime", timerTable_[CG_FIRESLAM_CD]);
        data.AddData("FireSlamSnakeCount",fireSnakeCount_);
        data.AddData("FireSlamSnakeDamage",fireSnakeDamage_);
        
        data.AddData("FireTriangleChargeTime", timerTable_[CG_TRIANGLESCHARGE]);
        data.AddData("FireTriangleAttackTime", timerTable_[CG_TRIANGLES]);
        data.AddData("FireTriangleCoolDownTime", timerTable_[CG_TRIANGLES_CD]);
        data.AddData("FireTriangleCount",fireTriCount_);
        
        data.AddData("BeamChargeTime", timerTable_[CG_BEAMCHARGE]);
        data.AddData("BeamAttackTime", timerTable_[CG_BEAM]);
        data.AddData("BeamDownTime", timerTable_[CG_BEAMDOWN]);
        data.AddData("BeamCoolDownTime", timerTable_[CG_BEAM_CD]);
        
        data.AddData("LightningCount",lightningMaxCount_);
        data.AddData("LightningChargeTime", timerTable_[CG_LIGHTNINGCHARGE]);
        data.AddData("LightningAttackTime", timerTable_[CG_LIGHTNING]);
        data.AddData("LightningCoolDownTime", timerTable_[CG_LIGHTNING_CD]);
        data.AddData("LightningDelay",lightningDelay_);

        data.AddData("RockFallAttackTime",timerTable_[CG_ROCKFALL]);
        data.AddData("RockFallCooldownTime",timerTable_[CG_ROCKFALL_CD]);
        data.AddData("RockFallMinSpawnDelay",rockFallMinSpawnDelay_);
        data.AddData("RockFallMaxSpawnDelay",rockFallMaxSpawnDelay_);
        data.AddData("RockFallCount",rockFallCount_);
        data.AddData("RockDebrisRadius",rockDebrisRadius_);

        data.AddData("StartPhase",currPhase_);
    }

    void GCAICrystalGuardian::Deserialize( const Simian::FileObjectSubNode& data )
    {
        GCAIBase::Deserialize(data);

        for(Simian::u32 i = 0; i < 4; ++i)
        {
            std::string name = "Phase";
            name += static_cast<char>('1' + i);
            deserializePhases_(i, data.Object(name));
        }

        data.Data("BirthTime", timerTable_[CG_BIRTH]);
        data.Data("IdleTime", timerTable_[CG_IDLE]);
        data.Data("DeathTime", timerTable_[CG_DEATH]);

        data.Data("StunEnterTime",timerTable_[CG_STUNENTER]);
        data.Data("StunTime",timerTable_[CG_STUN]);
        data.Data("StunExitTime",timerTable_[CG_STUNEXIT]);
        
        data.Data("PoundChargeTime", timerTable_[CG_POUNDLCHARGE]);
        timerTable_[CG_POUNDRCHARGE] = timerTable_[CG_POUNDLCHARGE];
        data.Data("PoundAttackTime", timerTable_[CG_POUNDL]);
        timerTable_[CG_POUNDR] = timerTable_[CG_POUNDL];
        data.Data("PoundCoolDownTime", timerTable_[CG_POUNDL_CD]);
        timerTable_[CG_POUNDR_CD] = timerTable_[CG_POUNDL_CD];

        data.Data("PunchChargeTime", timerTable_[CG_PUNCHLCHARGE]);
        timerTable_[CG_PUNCHRCHARGE] = timerTable_[CG_PUNCHLCHARGE];
        data.Data("PunchAttackTime", timerTable_[CG_PUNCHL]);
        timerTable_[CG_PUNCHR] = timerTable_[CG_PUNCHL];
        data.Data("PunchCoolDownTime", timerTable_[CG_PUNCHL_CD]);
        timerTable_[CG_PUNCHR_CD] = timerTable_[CG_PUNCHL_CD];

        data.Data("FireSlamChargeTime", timerTable_[CG_FIRESLAMCHARGE]);
        data.Data("FireSlamAttackTime", timerTable_[CG_FIRESLAM]);
        data.Data("FireSlamCoolDownTime", timerTable_[CG_FIRESLAM_CD]);
        data.Data("FireSlamSnakeCount", fireSnakeCount_,fireSnakeCount_);
        data.Data("FireSlamSnakeDamage", fireSnakeDamage_,fireSnakeDamage_);

        data.Data("FireTriangleChargeTime", timerTable_[CG_TRIANGLESCHARGE]);
        data.Data("FireTriangleAttackTime", timerTable_[CG_TRIANGLES]);
        data.Data("FireTriangleCoolDownTime", timerTable_[CG_TRIANGLES_CD]);
        data.Data("FireTriangleCount",fireTriCount_,fireTriCount_);

        data.Data("BeamChargeTime", timerTable_[CG_BEAMCHARGE]);
        data.Data("BeamAttackTime", timerTable_[CG_BEAM]);
        data.Data("BeamDownTime", timerTable_[CG_BEAMDOWN]);
        data.Data("BeamCoolDownTime", timerTable_[CG_BEAM_CD]);

        data.Data("LightningCount",lightningMaxCount_,lightningMaxCount_);
        data.Data("LightningChargeTime", timerTable_[CG_LIGHTNINGCHARGE]);
        data.Data("LightningAttackTime", timerTable_[CG_LIGHTNING]);
        data.Data("LightningCoolDownTime", timerTable_[CG_LIGHTNING_CD]);
        data.Data("LightningDelay",lightningDelay_,lightningDelay_);

        data.Data("RockFallAttackTime",timerTable_[CG_ROCKFALL]);
        data.Data("RockFallCooldownTime",timerTable_[CG_ROCKFALL_CD]);
        data.Data("RockFallMinSpawnDelay",rockFallMinSpawnDelay_,rockFallMinSpawnDelay_);
        data.Data("RockFallMaxSpawnDelay",rockFallMaxSpawnDelay_,rockFallMaxSpawnDelay_);
        data.Data("RockFallCount",rockFallCount_,rockFallCount_);
        data.Data("RockDebrisRadius",rockDebrisRadius_,rockDebrisRadius_);

        data.Data("StartPhase",currPhase_,currPhase_);
        --currPhase_;
    }

    //-Private------------------------------------------------------------------


    void GCAICrystalGuardian::deserializePhases_( Simian::u32 index, const Simian::FileObjectSubNode* phase )
    {
        if(!phase)
            return;
        Phase& p = phases_[index];
        phase->Data("Health",p.health_);
        phase->Data("IdleTime",p.idleTime_);

        const Simian::FileArraySubNode* attacks = phase->Array("Attacks");
        for(Simian::u32 i = 0; i < attacks->Size(); ++i)
        {
            Attack a;
            const Simian::FileObjectSubNode* attack = attacks->Object(i);
            attack->Data("Id", a.id_);
            attack->Data("Chance", a.chance_);
            attack->Data("Damage", a.damage_);
            attack->Data("Radius", a.radius_);
            p.attacks_.push_back(a);
        }
    }

    void GCAICrystalGuardian::serializePhases_( Simian::u32 index, Simian::FileObjectSubNode* phase )
    {
        Phase& p = phases_[index];
        phase->AddData("Health",p.health_);
        phase->AddData("Idle",p.idleTime_);

        Simian::FileArraySubNode* attacks = phase->Array("Attacks");
        for(Simian::u32 i = 0; i < attacks->Size(); ++i)
        {
            Attack a = p.attacks_[i];
            Simian::FileObjectSubNode* attack = attacks->Object(i);
            attack->AddData("Id", a.id_);
            attack->AddData("Chance", a.chance_);
            attack->AddData("Damage", a.damage_);
            attack->AddData("Radius", a.radius_);
            attack->AddData("ChargeTime", timerTable_[a.id_]);
            attack->AddData("AttackTime", timerTable_[a.id_ + 1]);
            attack->AddData("CoolDownTime", timerTable_[a.id_ + 2]);
        }
    }

    void GCAICrystalGuardian::update_( Simian::DelegateParameter& param )
    {
#ifdef _DESCENDED
        if (GCEditor::EditorMode())
            return;
#endif
        fsm_.Update(param.As<Simian::EntityUpdateParameter>().Dt);
    }

    void GCAICrystalGuardian::onTakeDamage_( Simian::DelegateParameter& param)
    {
        GCAttributes::DealDamageParameter& p = param.As<GCAttributes::DealDamageParameter>();

        if (attributes_->Health() <= p.Damage)
        {
            // Play random hurt growl sound
            if (sounds_)
                sounds_->Play(Simian::Math::Random(S_HURT_GROWL1, S_HURT_GROWL1+TOTAL_HURTGROWL_SOUNDS));

            physics_->Enabled(true);
            fsm_.ChangeState(CG_STUNENTER);
            if(currPhase_ == TOTAL_PHASES)
            {
                dieEventScript_->OnTriggered(Simian::Delegate::NoParameter);
                dead_ = true;
                attributes_->MaxHealth(phases_[currPhase_].health_+ p.Damage);
                attributes_->Health(phases_[currPhase_].health_ + p.Damage);

                // kill all the little shits
                gauntletSpawnSeq_->KillSpawnedEnemies();
                gauntletSpawnSeq_->ForceStop();
                return;
            }

            ++currPhase_;

            // Must offset the damage that will be dealt after this
            attributes_->MaxHealth(phases_[currPhase_].health_+ p.Damage);
            attributes_->Health(phases_[currPhase_].health_ + p.Damage);
            stunEventScript_->OnTriggered(Simian::Delegate::NoParameter);
        }
        else if (fsm_.CurrentState() == CG_STUN)
        {
            if ((rand()%2) < 1)
            {
                model_->Controller().PlayAnimation(BOSS_FLINCH_1, false);
                createRecoveryShard_(true);
            }
            else
            {
                model_->Controller().PlayAnimation(BOSS_FLINCH_2, false);
                createRecoveryShard_(false);
            }
            // Play random hurt growl sound
            if (sounds_)
                sounds_->Play(Simian::Math::Random(S_HURT_GROWL1, S_HURT_GROWL1+TOTAL_HURTGROWL_SOUNDS));
        }
        else
        {
            // Play random get hit sound
            if (sounds_)
                sounds_->Play(Simian::Math::Random(S_GET_HIT1, S_GET_HIT1+TOTAL_GETHIT_SOUNDS));
        }
    }

    void GCAICrystalGuardian::idleOnEnter_( Simian::DelegateParameter&  )
    {
        timer_ = phases_[currPhase_].idleTime_;
        rightHand_->AttackOn(false);
        leftHand_->AttackOn(false);
        physics_->Enabled(false);
    }

    void GCAICrystalGuardian::idleOnUpdate_( Simian::DelegateParameter& param )
    {
        timer_ -= param.As<Simian::EntityUpdateParameter>().Dt;
            
        if(playerAttribs_->Health() > 0 && timer_ < 0)
            fsm_.ChangeState(CG_CHOOSESKILL);
    }

    void GCAICrystalGuardian::chooseSkillOnUpdate_( Simian::DelegateParameter&  )
    {
        //lhsCrystal_->SetActiveCrystal(GCCrystalSwap::CT_FIRE);
        //rhsCrystal_->SetActiveCrystal(GCCrystalSwap::CT_FIRE);
        if (GCPlayerLogic::Instance()->Attributes()->Health() <= 0)
        {
            fsm_.ChangeState(CG_IDLE);
            return;
        }
        currAttackData_ = &phases_[currPhase_].attacks_[randomAttack_(currPhase_)];
        fsm_.ChangeState(currAttackData_->id_);
        attacked_ = true;
    }

    void GCAICrystalGuardian::stunEnterOnUpdate_( Simian::DelegateParameter& param )
    {
        timer_ -= param.As<Simian::EntityUpdateParameter>().Dt;
        
        if(timer_< 0)
        {
            if(dead_)
            {
                phases_[currPhase_].leftCrystal_->Visible(false);
                phases_[currPhase_].rightCrystal_->Visible(false);
                fsm_.ChangeState(CG_DEATH);
            }
            else
            {
                phases_[currPhase_ - 1].leftCrystal_->Visible(false);
                phases_[currPhase_ - 1].rightCrystal_->Visible(false);
                phases_[currPhase_].leftCrystal_->Visible(true);
                phases_[currPhase_].rightCrystal_->Visible(true);
                fsm_.ChangeState(CG_STUN);
            }
        }
    }
	 void GCAICrystalGuardian::stunExitOnExit_( Simian::DelegateParameter& )
	{
		model_->Controller().PlayAnimation(ANIMATION_TABLE[CG_IDLE],true);

        physics_->Enabled(false);
		if (currPhase_ == TOTAL_PHASES && !dead_)
			// spawn little shits
			gauntletSpawnSeq_->ForceTrigger();
	}
    void GCAICrystalGuardian::poundLOnEnter_( Simian::DelegateParameter&  )
    {
        timer_ = timerTable_[CG_POUNDL];
        model_->Controller().PlayAnimation(ANIMATION_TABLE[CG_POUNDL],false);
        leftHand_->Radius(currAttackData_->radius_);
        leftHand_->Damage(currAttackData_->damage_);
        doneDamage_ = false;
        leftHand_->AttackOn(true);
    }

    void GCAICrystalGuardian::poundLOnUpdate_( Simian::DelegateParameter& param )
    {
        timer_ -= param.As<Simian::EntityUpdateParameter>().Dt;
        
        if(timer_ < 0)
        {
            Simian::Vector3 pos = leftHand_->Transform()->WorldPosition();
            pos.Y(0.0f);
            spawnPoundEffect_(pos);

            GCDescensionCamera::Instance()->Shake(0.25f, 0.25f);
            leftHand_->AttackOn(false);
            fsm_.ChangeState(CG_POUNDL_CD);

            // Play sounds
            if (sounds_)
                sounds_->Play(S_STOMP);
        }
    }

    void GCAICrystalGuardian::poundROnEnter_( Simian::DelegateParameter&  )
    {
        timer_ = timerTable_[CG_POUNDR];
        model_->Controller().PlayAnimation(ANIMATION_TABLE[CG_POUNDR],false);
        rightHand_->Radius(currAttackData_->radius_);
        rightHand_->Damage(currAttackData_->damage_);
        doneDamage_ = false;
        rightHand_->AttackOn(true);
    }

    void GCAICrystalGuardian::poundROnUpdate_( Simian::DelegateParameter& param )
    {
        timer_ -= param.As<Simian::EntityUpdateParameter>().Dt;
        
        if(timer_ < 0)
        {
            Simian::Vector3 pos = rightHand_->Transform()->WorldPosition();
            pos.Y(0.0f);
            spawnPoundEffect_(pos);

            GCDescensionCamera::Instance()->Shake(0.25f, 0.25f);
            rightHand_->AttackOn(false);
            fsm_.ChangeState(CG_POUNDR_CD);

            // Play sounds
            if (sounds_)
                sounds_->Play(S_STOMP);
        }
    }

    void GCAICrystalGuardian::poundCDOnUpdate_( Simian::DelegateParameter& param )
    {
        timer_ -= param.As<Simian::EntityUpdateParameter>().Dt;
        
        if(timer_ < 0)
        {
            // If already attacked
            if(attacked_)
            {
                Simian::u32 currState = fsm_.CurrentState();

                if(currState == CG_POUNDR_CD)
                    fsm_.ChangeState(CG_POUNDLCHARGE);
                else
                    fsm_.ChangeState(CG_POUNDRCHARGE);
                attacked_ = false;
            }
            else
            {
                model_->Controller().PlayAnimation(ANIMATION_TABLE[CG_IDLE]);
                fsm_.ChangeState(CG_IDLE);
            }
        }
    }

    void GCAICrystalGuardian::punchLOnEnter_( Simian::DelegateParameter&  )
    {
        timer_ = timerTable_[CG_PUNCHL];
        model_->Controller().PlayAnimation(ANIMATION_TABLE[CG_PUNCHL],false);
        leftHand_->Radius(currAttackData_->radius_);
        leftHand_->Damage(currAttackData_->damage_);
        doneDamage_ = false;
        leftHand_->AttackOn(true);

        // Play sounds
        if (sounds_)
            sounds_->Play(S_PUNCH1);
    }

    void GCAICrystalGuardian::punchLOnUpdate_( Simian::DelegateParameter& param )
    {
        timer_ -= param.As<Simian::EntityUpdateParameter>().Dt;

        if(timer_ < 0)
        {
            leftHand_->AttackOn(false);
            fsm_.ChangeState(CG_PUNCHL_CD);
        }
    }

    void GCAICrystalGuardian::punchCDOnUpdate_( Simian::DelegateParameter& param )
    {
        leftHand_->AttackOn(false);
        rightHand_->AttackOn(false);
        timer_ -= param.As<Simian::EntityUpdateParameter>().Dt;
        if(timer_ < 0)
        {
            // If already attacked
            if(attacked_)
            {
                Simian::u32 currState = fsm_.CurrentState();

                if(currState == CG_PUNCHR_CD)
                    fsm_.ChangeState(CG_PUNCHLCHARGE);
                else
                    fsm_.ChangeState(CG_PUNCHRCHARGE);
                attacked_ = false;
            }
            else
                fsm_.ChangeState(CG_IDLE);
        }
    }

    void GCAICrystalGuardian::punchROnEnter_( Simian::DelegateParameter&  )
    {
        timer_ = timerTable_[CG_PUNCHR];
        model_->Controller().PlayAnimation(ANIMATION_TABLE[CG_PUNCHR],false);
        rightHand_->Radius(currAttackData_->radius_);
        rightHand_->Damage(currAttackData_->damage_);
        doneDamage_ = false;
        rightHand_->AttackOn(true);

        // Play sounds
        if (sounds_)
            sounds_->Play(S_PUNCH2);
    }

    void GCAICrystalGuardian::punchROnUpdate_( Simian::DelegateParameter& param)
    {
        timer_ -= param.As<Simian::EntityUpdateParameter>().Dt;

        if(timer_ < 0)
        {
            rightHand_->AttackOn(false);
            fsm_.ChangeState(CG_PUNCHR_CD);
        }
    }

    void GCAICrystalGuardian::beamOnEnter_( Simian::DelegateParameter& )
    {
        timer_ = timerTable_[CG_BEAM];
        effTrans_->Position(leftHand_->Position());
        model_->Controller().PlayAnimation(ANIMATION_TABLE[CG_BEAM],false);
        doneDamage_ = false;

        // Play sounds
        if (sounds_)
            sounds_->Play(S_LIGHTNING_BEAM);
    }

    void GCAICrystalGuardian::beamOnUpdate_( Simian::DelegateParameter& param )
    {
        timer_ -= param.As<Simian::EntityUpdateParameter>().Dt;
        
        effTrans_->Position(leftHand_->Position());

        Simian::Vector3 vecToPlayer_ = GCPlayerLogic::Instance()->Transform().WorldPosition() - effTrans_->Position();
        if( !doneDamage_ )
        {
            Simian::Vector3 dir(effTrans_->Direction());
            Simian::f32 lenSqr = vecToPlayer_.LengthSquared();
            Simian::f32 proj = vecToPlayer_.Dot(dir);
            Simian::f32 dot = lenSqr - proj*proj;

            if( dot > 0 && dot < 0.05f )
            {
                CombatCommandSystem::Instance().AddCommand(attributes_, playerAttribs_,
                    CombatCommandDealDamage::Create(currAttackData_->damage_));
                doneDamage_ = true;
            }
        }
        if(timer_ < 0)
            fsm_.ChangeState(CG_BEAMDOWN);
    }

    void GCAICrystalGuardian::beamOnExit_( Simian::DelegateParameter&  )
    {
        effTrans_->Position(Simian::Vector3(-Simian::Vector3::UnitY));
    }

    void GCAICrystalGuardian::lightningChargeOnEnter_( Simian::DelegateParameter& )
    {
        model_->Controller().PlayAnimation(ANIMATION_TABLE[CG_LIGHTNINGCHARGE],false);
        timer_ = timerTable_[CG_LIGHTNINGCHARGE];
        lightningTargetChargeTime_ = timer_/2;

        Simian::Vector3 pos(GCPlayerLogic::Instance()->Transform().WorldPosition());
        pos.Y(0.07f);
        Effect lightning;
        lightning.castDecal_ = ParentScene()->CreateEntity(E_LIGHTNINGCAST);
        lightning.castDecal_->ComponentByType(Simian::C_TRANSFORM,lightning.transform_);
        lightning.transform_->Position(pos);
        lightning.transform_->Scale(Simian::Vector3::Zero);
        lightning.beginScale_ = 0.f;
        lightning.endScale_ = 1.f;
        lightning.timer_ = lightningTargetChargeTime_;
        effects_.push_back(lightning);
        ParentScene()->AddEntity(lightning.castDecal_);

        leftHand_->AttackOn(true);
        rightHand_->AttackOn(true);
        leftHand_->Damage(0);
        rightHand_->Damage(0);

        // Play sounds
        if (sounds_)
            sounds_->Play(S_GROWL1);
    }

    void GCAICrystalGuardian::lightningChargeOnUpdate_( Simian::DelegateParameter& param )
    {
        EffectIterator it = effects_.begin(); 
        Simian::CTransform* trans = it->transform_;
        Simian::f32 scaleI = it->beginScale_;
        Simian::f32 scaleE = it->endScale_;
        Simian::Vector3 pos(GCPlayerLogic::Instance()->Transform().WorldPosition());
        pos.Y(0.07f);
        trans->Position(pos);

        Simian::f32 dt = param.As<Simian::EntityUpdateParameter>().Dt;
        timer_ -= dt;
        it->timer_ -= dt;

        // If growing, flip to shrink
        if((it->timer_ < Simian::Math::EPSILON) && it->endScale_ > 0)
        {
            it->beginScale_ = 1.f;
            it->endScale_ = 0.f;
            it->timer_ = lightningTargetChargeTime_;
        }

        // Because timer decrements, we switch the end/begin scale to get a 
        // growing effect
        Simian::f32 intp = it->timer_/LIGHTNING_TARGET_TIME;
        intp = Simian::Math::Clamp(intp, 0.0f, 1.0f);
        trans->Scale(Simian::Interpolation::Interpolate
            <Simian::Vector3>(Simian::Vector3(scaleE,scaleE,scaleE),Simian::Vector3(scaleI,scaleI,scaleI),intp,Simian::Interpolation::Linear));

        if(timer_ < 0)
        {
            ParentScene()->RemoveEntity(it->castDecal_);
            effects_.pop_front();
            fsm_.ChangeState(CG_LIGHTNING);
        }
    }

    void GCAICrystalGuardian::lightningOnEnter_( Simian::DelegateParameter&  )
    {
        timer_ = timerTable_[CG_LIGHTNING];
        lightningDelayTimer_ = 0;
        lightningTargetTime_ = lightningDelay_/2.f;
        lightningCount_ = 0;
        model_->Controller().PlayAnimation(ANIMATION_TABLE[CG_LIGHTNING],true);
    }
    
    void GCAICrystalGuardian::lightningOnUpdate_( Simian::DelegateParameter& param )
    {
        Simian::f32 dt = param.As<Simian::EntityUpdateParameter>().Dt;
        timer_ -= dt;

        if(lightningCount_ <= lightningMaxCount_)
        {
            lightningDelayTimer_ -= dt;

            // Spawn target
            if(lightningDelayTimer_ < 0)
            {
                lightningDelayTimer_ = lightningDelay_;
                Simian::Vector3 pos(GCPlayerLogic::Instance()->Transform().WorldPosition());
                pos.Y(0.07f);
                Effect lightning;
                lightning.castDecal_ = ParentScene()->CreateEntity(E_LIGHTNINGCAST);
                lightning.castDecal_->ComponentByType(Simian::C_TRANSFORM,lightning.transform_);
                lightning.transform_->Position(pos);
                lightning.transform_->Scale(Simian::Vector3::Zero);
                lightning.beginScale_ = 0.f;
                lightning.endScale_ = 1.f;
                lightning.timer_ = lightningTargetTime_;
                effects_.push_back(lightning);
                ParentScene()->AddEntity(lightning.castDecal_);
            }
        }

        EffectIterator it = effects_.begin(); 
        // Loop through lightning and targets
        while( it != effects_.end())
        {
            it->timer_ -= dt;

            // Create lightning
            if((it->timer_ < 0) && lightningCount_ <= lightningMaxCount_)
            {
                if(it->endScale_ < Simian::Math::EPSILON)
                {
                    EffectIterator temp = it;
                    ++it;
                    ParentScene()->RemoveEntity(temp->castDecal_);
                    effects_.erase(temp);

                    continue;
                }
                Simian::CTransform* trans;
                it->effect_ = ParentScene()->CreateEntity(E_BOSSGHOSTLIGHTNING);
                it->effect_->ComponentByType(Simian::C_TRANSFORM,trans);
                trans->Position(it->transform_->Position());
                it->beginScale_ = 1.f;
                it->endScale_ = 0.f;
                it->timer_ = lightningTargetTime_;
                ParentScene()->AddEntity(it->effect_);
                if(damageAreaCheck_(it->transform_,currAttackData_->radius_))
                    dealDamage_();
                ++lightningCount_;
            }

            Simian::f32 scaleI = it->beginScale_;
            Simian::f32 scaleE = it->endScale_;

            // Interp scale of target
            Simian::f32 intp = it->timer_/lightningTargetTime_;
            intp = Simian::Math::Clamp(intp, 0.0f, 1.0f);
            it->transform_->Scale(Simian::Interpolation::Interpolate
                <Simian::Vector3>(Simian::Vector3(scaleE,scaleE,scaleE),Simian::Vector3(scaleI,scaleI,scaleI),intp,Simian::Interpolation::Linear));
            ++it;
        }
        
        if(timer_ < 0)
            fsm_.ChangeState(CG_LIGHTNING_CD);
    }

    void GCAICrystalGuardian::lightningOnExit_( Simian::DelegateParameter&  )
    {
        EffectIterator it = effects_.begin();
        ParentScene()->RemoveEntity(it->castDecal_);
        effects_.clear();
        leftHand_->AttackOn(false);
        rightHand_->AttackOn(false);
    }

    void GCAICrystalGuardian::fireSlamChargeOnEnter_( Simian::DelegateParameter&  )
    {
        std::string id = ANIMATION_TABLE[CG_FIRESLAMCHARGE];
        if(!id.empty())
            model_->Controller().PlayAnimation(ANIMATION_TABLE[CG_FIRESLAMCHARGE],false);
        timer_ = timerTable_[CG_FIRESLAMCHARGE];

        // Play sounds
        if (sounds_)
            sounds_->Play(Simian::Math::Random(S_GROWL1, S_GROWL1+TOTAL_GROWL_SOUNDS));
        leftHand_->Damage(0);
        rightHand_->Damage(0);
    }

     void GCAICrystalGuardian::fireSlamOnEnter_( Simian::DelegateParameter&  )
    {
        timer_ = timerTable_[CG_FIRESLAM];
        model_->Controller().PlayAnimation(ANIMATION_TABLE[CG_FIRESLAM], false);
        
        rightHand_->Radius(currAttackData_->radius_);
        rightHand_->Damage(currAttackData_->damage_);
        leftHand_->AttackOn(true);
        rightHand_->AttackOn(true);
    }

    void GCAICrystalGuardian::fireSlamOnExit_( Simian::DelegateParameter&  )
    {
        Simian::f32 angleInc = 360.0f/fireSnakeCount_;
        angleInc = Simian::Degree(angleInc).Radians();
        Simian::Vector3 rightHandPos(rightHand_->Transform()->WorldPosition());

        for (Simian::u32 i = 0; i < fireSnakeCount_; ++i)
        {
            Simian::f32 angle = angleInc * i;
            Simian::Vector3 dir(std::cos(angle), 0.0f, std::sin(angle));

            Simian::Entity* entity = ParentScene()->CreateEntity(E_FLAMINGSNAKEEFFECT);
            GCFlamingSnakeEffect* snake;
            entity->ComponentByType(GC_FLAMINGSNAKEEFFECT, snake);
            snake->DealDamage(true);
            snake->Damage(fireSnakeDamage_);
            snake->Direction(dir);
            Simian::CTransform* transform;
            entity->ComponentByType(Simian::C_TRANSFORM, transform);
            transform->WorldPosition(rightHandPos + dir);
            ParentScene()->AddEntity(entity);
        }
        GCDescensionCamera::Instance()->Shake(0.25f, 0.5f);
        // Play sounds
        if (sounds_)
            sounds_->Play(S_BIG_STOMP);
    }

    void GCAICrystalGuardian::fireSlamCDOnEnter_( Simian::DelegateParameter& )
    {
        timer_ = timerTable_[CG_FIRESLAM_CD];
    }

    void GCAICrystalGuardian::trianglesChargeOnEnter_( Simian::DelegateParameter&  )
    {
        timer_ = timerTable_[CG_TRIANGLESCHARGE];
        model_->Controller().PlayAnimation(ANIMATION_TABLE[CG_TRIANGLESCHARGE],false);
        Simian::f32 oneoverr2 = 1.0f/std::sqrt(2.0f);
        Simian::f32 radius = currAttackData_->radius_;

        for(Simian::u32 i = 0; i < fireTriCount_; ++i)
        {
            Simian::Vector3 points[3] = 
            {
                radius * Simian::Vector3(0.0f, 0.0f, 1.0f),
                radius * Simian::Vector3(oneoverr2, 0.0f, -oneoverr2),
                radius * Simian::Vector3(-oneoverr2, 0.0f, -oneoverr2)
            };

            Simian::f32 x = bossTriggerPos_.X() + Simian::Math::Random(-2.0f, 2.0f);
            Simian::f32 z = bossTriggerPos_.Z() + Simian::Math::Random(-2.0f,2.0f);
            for (Simian::u32 i = 0; i < 3; ++i)
                points[i] = Simian::Vector3(x, 0.5f, z) + points[i];

            GCMajenTriangle* triangleComp;
            effEnt_ = ParentScene()->CreateEntity(E_MAJENTRIANGLE);        
            effEnt_->ComponentByType(GC_MAJENTRIANGLE, triangleComp);
            triangleComp->CastingTime(timer_);
            triangleComp->Points(points);
            triangleComp->OwnerAttribs(attributes_);
            triangleComp->Damage(currAttackData_->damage_);
            ParentScene()->AddEntity(effEnt_);
        }

        leftHand_->AttackOn(true);
        rightHand_->AttackOn(true);
        leftHand_->Damage(0);
        rightHand_->Damage(0);

        // Play sounds
        if (sounds_)
            sounds_->Play(S_FIRE_TRIANGLE_FORM);
    }

    void GCAICrystalGuardian::trianglesOnEnter_( Simian::DelegateParameter& )
    {
        timer_ = timerTable_[CG_TRIANGLES];
        leftHand_->AttackOn(true);
        rightHand_->AttackOn(true);
        leftHand_->Damage(0);
        rightHand_->Damage(0);

        // Play sounds
        if (sounds_)
            sounds_->Play(S_FIRE_TRIANGLE_FIRE);
    }

    void GCAICrystalGuardian::rockFallOnEnter_( Simian::DelegateParameter&  )
    {
        rightHand_->AttackOn(true);
        leftHand_->AttackOn(true);
        leftHand_->Damage(0);
        rightHand_->Damage(0);
        model_->Controller().PlayAnimation(ANIMATION_TABLE[CG_ROCKFALL], false);
        timer_ = timerTable_[CG_ROCKFALL];
        rockFallCounter_ = 0;
        rockFallSpawnTimer_ = Simian::Math::Random(rockFallMinSpawnDelay_,rockFallMaxSpawnDelay_);

        // Play sounds
        if (sounds_)
            sounds_->Play(S_ROAR);

        GCDescensionCamera::Instance()->Shake(0.3f, 0.8f);
    }

    void GCAICrystalGuardian::rockFallOnUpdate_( Simian::DelegateParameter& param )
    {
        Simian::f32 dt = param.As<Simian::EntityUpdateParameter>().Dt;
        rockFallSpawnTimer_ -= dt;
        timer_ -= dt;
        
        if(timer_ < 0 && (rockFallCounter_ >= rockFallCount_))
        {
            fsm_.ChangeState(CG_ROCKFALL_CD);
            return;
        }
        if((rockFallCounter_ < rockFallCount_) && rockFallSpawnTimer_ < 0)
        {
            Simian::Entity* ent;
            ent = ParentScene()->CreateEntity(E_ROCKDEBRIS);
            ParentScene()->AddEntity(ent);
            GCRockDebris* logic;
            ent->ComponentByType(GC_ROCKDEBRIS,logic);
            Simian::f32 minX = bossTriggerPos_.X() - currAttackData_->radius_;
            Simian::f32 minZ = bossTriggerPos_.Z() - currAttackData_->radius_;
            Simian::f32 maxX = bossTriggerPos_.X() + currAttackData_->radius_;
            Simian::f32 maxZ = bossTriggerPos_.Z() + currAttackData_->radius_;
            logic->Radius(rockDebrisRadius_);
            logic->Damage(currAttackData_->damage_);
            logic->StartHeight(12.0f);
            logic->SetRandomSpawnPos(minX,minZ,maxX,maxZ);
            rockFallSpawnTimer_ = Simian::Math::Random(rockFallMinSpawnDelay_,rockFallMaxSpawnDelay_);
            ++rockFallCounter_;
        }
    }

    void GCAICrystalGuardian::deathOnUpdate_( Simian::DelegateParameter& param )
    {
        timer_ -= param.As<Simian::EntityUpdateParameter>().Dt;

        if(timer_< 0)
        {
            attributes_->MaxHealth(0);
            attributes_->Health(0);
        }
    }

    void GCAICrystalGuardian::stateOnExit_( Simian::DelegateParameter&  )
    {
        model_->Controller().PlayAnimation(ANIMATION_TABLE[CG_IDLE]);
    }

    Simian::u32 GCAICrystalGuardian::randomAttack_( Simian::u32 phase )
    {
        const AttackList& attacks = phases_[phase].attacks_;

        Simian::u32 chance = Simian::Math::Random(0,100);
        Simian::u32 maxAttacks = attacks.size();
        for(Simian::u32 i = 0; i < maxAttacks; ++i)
        {
            Simian::u32 phaseChance = attacks[i].chance_;
            if(chance < phaseChance)
                return i;
            chance -= phaseChance;
        }

        // Chances do not add up to 100%
        return 0;
    }

    bool GCAICrystalGuardian::damageAreaCheck_( const Simian::CTransform* trans, Simian::f32 radius )
    {
        Simian::f32 sqrDist = (GCPlayerLogic::Instance()->Transform().World().Position() - 
            trans->Position()).LengthSquared();

        return sqrDist < (radius * radius);
    }

    void GCAICrystalGuardian::dealDamage_()
    {
        CombatCommandSystem::Instance().AddCommand(attributes_, playerAttribs_,
            CombatCommandDealDamage::Create(currAttackData_->damage_));
    }

    void GCAICrystalGuardian::createRecoveryShard_(bool isHealth)
    {
        for (Simian::u32 i = 0; i < RECOVER_SPAWN_AMOUNT; ++i)
        {
            // spawn an entity
            Simian::Entity* entity = 0;
            if (isHealth)
                entity = ParentScene()->CreateEntity(E_HEALTHSHARD);
            else
                entity = ParentScene()->CreateEntity(E_MANASHARD);
            if (!entity)
                return; // failed to spawn

            // get the transform
            Simian::CTransform* trans;
            entity->ComponentByType(Simian::C_TRANSFORM, trans);

            if (trans)
            {
                // randomize an angle
                Simian::f32 angle = Simian::Math::Random(0.0f, Simian::Math::TAU);
                Simian::Vector3 direction(std::cos(angle), 0.0f, std::sin(angle));

                // set the position
                trans->Position(transform_->Position() + direction * 
                    RECOVER_SPAWN_RADIUS * Simian::Math::Random(0.0f, 1.0f) + 
                    Simian::Vector3(0, Simian::Math::Random(0.0f, 1.0f), 0));
                trans->Position(Simian::Vector3(trans->Position().X() - 0.5f, 
                                                trans->Position().Y(), 
                                                trans->Position().Z() - 0.5f));
            }

            // add spawned entity to scene
            ParentScene()->AddEntity(entity);
        }
    }

    void GCAICrystalGuardian::spawnPoundEffect_( const Simian::Vector3& pos )
    {
        Simian::Entity* poundEffect = ParentScene()->CreateEntity(E_POUNDEFFECT);
        Simian::CTransform* effectTransform;
        poundEffect->ComponentByType(Simian::C_TRANSFORM, effectTransform);
        effectTransform->WorldPosition(pos);
        ParentScene()->AddEntity(poundEffect);
    }
}
