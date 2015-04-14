/******************************************************************************/
/*!
\file		AISkillGroundSmash.cpp
\author 	Jonathan Butt, j.butt, 250000211
\par    	email: j.butt\@digipen.edu
\date   	2012/08/10
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/******************************************************************************/
#include "Simian/EngineComponents.h"
#include "Simian/Delegate.h"
#include "Simian/Scene.h"
#include "Simian/EntityComponent.h"
#include "Simian/Entity.h"
#include "Simian/AnimationController.h"
#include "Simian/CModel.h"
#include "Simian/Math.h"
#include "Simian/LogManager.h"
#include "Simian/Vector3.h"
#include "Simian/CSoundEmitter.h"

#include "ComponentTypes.h"

#include "GCPlayerLogic.h"
#include "GCAttributes.h"
#include "GCPhysicsController.h"
#include "GCBoneTrailVelocityController.h"

#include "CombatCommandSystem.h"
#include "CombatCommandDealDamage.h"
#include "GCDescensionCamera.h"

#include "AISkillGroundSmash.h"
#include "EntityTypes.h"

namespace Descension
{
    Simian::FactoryPlant<AIEffectBase, AISkillGroundSmash> AISkillGroundSmash::factoryPlant_(
        AIEffectBase::createFactoryPlant_<AISkillGroundSmash>(EFF_S_GROUNDSMASH));

    AISkillGroundSmash::AISkillGroundSmash()
        :   AISkillBase(EFF_S_GROUNDSMASH),
            targetAttribs_(0),
            model_(0),
            ownersTrans_(0),
            srcTrans_(0),
            boneTrail1_(0),
            boneTrail2_(0),
            sounds_(0),
            smash_(false),
            damage_(0),
            attackRadius_(0),
            attackRadiusSqr_(0),
            damageRadius_(0),
            damageRadiusSqr_(0),
            chargeTime_(0),
            smashTime_(0),
            coolDownTime_(0),
            slamSoundNum_(14),
            timer_(0),
            vecToTar_(Simian::Vector3::Zero),
            fsm_(SS_BIRTH,SS_TOTAL)
    {
    }

    AISkillGroundSmash::~AISkillGroundSmash()
    {
    }

    bool AISkillGroundSmash::Update( Simian::f32 dt )
    {
        if(!Enable())
            return false;
        
        updateVecToTarget_();

        if( smash_ || vecToTar_.LengthSquared() < attackRadiusSqr_ )
        {
            fsm_.Update(dt);
            if(!smash_)
                model_->Controller().PlayAnimation("Run");
        }
        
        return smash_;
    }

    //--------------------------------------------------------------------------

    void AISkillGroundSmash::onAwake_()
    {
    }

    void AISkillGroundSmash::onInit_( GCAttributes* source )
    {
        GCPhysicsController* srcPhy;
        GCPhysicsController* tarPhy;
        if(source)
        {
            source->ComponentByType(Simian::C_TRANSFORM,srcTrans_);
            source->ComponentByType(GC_PHYSICSCONTROLLER,srcPhy);
            targetAttribs_ = source;
        }
        // Default
        else
        {
            GCPlayerLogic::Instance()->ParentEntity()->ParentEntity()->ComponentByType(Simian::C_TRANSFORM,srcTrans_);
            GCPlayerLogic::Instance()->ParentEntity()->ParentEntity()->ComponentByType(GC_PHYSICSCONTROLLER,srcPhy);
            targetAttribs_ = GCPlayerLogic::Instance()->Attributes();
        }

        Target()->ParentEntity()->ComponentByType(Simian::C_MODEL,model_);
        Target()->ParentEntity()->ComponentByType(Simian::C_TRANSFORM,ownersTrans_);
        Target()->ParentEntity()->ComponentByType(Simian::C_SOUNDEMITTER,sounds_);
        Target()->ParentEntity()->ComponentByType(GC_PHYSICSCONTROLLER,tarPhy);

        if(!model_ || !ownersTrans_ || !tarPhy || !srcPhy)
        {
            Enable(false);
        }
        else
        {
            attackRadius_ += tarPhy->Radius() + srcPhy->Radius();
            attackRadiusSqr_ = attackRadius_ * attackRadius_;
        }

        fsm_[SS_BIRTH].OnEnter = Simian::Delegate::CreateDelegate<AISkillGroundSmash, &AISkillGroundSmash::birthOnEnter_>(this);
        fsm_[SS_BIRTH].OnUpdate = Simian::Delegate::CreateDelegate<AISkillGroundSmash, &AISkillGroundSmash::birthOnUpdate_>(this);
        fsm_[SS_CHARGE].OnEnter = Simian::Delegate::CreateDelegate<AISkillGroundSmash, &AISkillGroundSmash::chargeOnEnter_>(this);
        fsm_[SS_CHARGE].OnUpdate = Simian::Delegate::CreateDelegate<AISkillGroundSmash, &AISkillGroundSmash::chargeOnUpdate_>(this);
        fsm_[SS_SMASH].OnEnter = Simian::Delegate::CreateDelegate<AISkillGroundSmash, &AISkillGroundSmash::smashOnEnter_>(this);
        fsm_[SS_SMASH].OnUpdate = Simian::Delegate::CreateDelegate<AISkillGroundSmash, &AISkillGroundSmash::smashOnUpdate_>(this);
        fsm_[SS_COOLDOWN].OnEnter = Simian::Delegate::CreateDelegate<AISkillGroundSmash, &AISkillGroundSmash::coolDownOnEnter_>(this);
        fsm_[SS_COOLDOWN].OnUpdate = Simian::Delegate::CreateDelegate<AISkillGroundSmash, &AISkillGroundSmash::coolDownOnUpdate_>(this);
        
        Simian::Entity* entity = Target()->ParentEntity()->ChildByName("BoneTrail1");
        if (entity)
            entity->ComponentByType(GC_BONETRAILVELOCITYCONTROLLER, boneTrail1_);

        entity = Target()->ParentEntity()->ChildByName("BoneTrail2");
        if (entity)
            entity->ComponentByType(GC_BONETRAILVELOCITYCONTROLLER, boneTrail2_);
    }

    void AISkillGroundSmash::onUnload_()
    {

    }

    void AISkillGroundSmash::serialize_( Simian::FileObjectSubNode& node )
    {
        node.AddData("AttackRadius",attackRadius_);
        node.AddData("DamageRadius",damageRadius_);
        node.AddData("Damage",damage_);
        node.AddData("ChargeTime",chargeTime_);
        node.AddData("SlamSoundNum",slamSoundNum_);
        node.AddData("SmashTime",smashTime_);
        node.AddData("CoolDownTime",coolDownTime_);
    }

    void AISkillGroundSmash::deserialize_( const Simian::FileObjectSubNode& node )
    {
        node.Data("DamageRadius",damageRadius_,damageRadius_);
        damageRadiusSqr_ = damageRadius_*damageRadius_;

        node.Data("AttackRadius",attackRadius_,attackRadius_);
        attackRadiusSqr_ = attackRadius_*attackRadius_;

        node.Data("Damage",damage_,damage_);

        node.Data("ChargeTime",chargeTime_,chargeTime_);

        node.Data("SlamSoundNum",slamSoundNum_,slamSoundNum_);

        node.Data("SmashTime",smashTime_,smashTime_);

        node.Data("CoolDownTime",coolDownTime_,coolDownTime_);
    }

    void AISkillGroundSmash::birthOnEnter_( Simian::DelegateParameter&  )
    {
    }

    void AISkillGroundSmash::birthOnUpdate_( Simian::DelegateParameter&  )
    {
        smash_ = true;
        model_->Controller().PlayAnimation("Pre Slam", false);
        fsm_.ChangeState(SS_CHARGE);
        if (boneTrail1_)
        {
            boneTrail1_->Enabled(true);
            boneTrail2_->Enabled(true);
        }
    }

    void AISkillGroundSmash::chargeOnEnter_( Simian::DelegateParameter&  )
    {
        updateVecToTarget_();
        Simian::Vector3 dir = vecToTar_.Normalized();
        Simian::f32 angle = vectorToDegreeFloat_(dir);
        
        ownersTrans_->Rotation(Simian::Vector3(0, 1, 0), Simian::Degree(angle + 90.0f));
        Target()->Direction(dir);
        timer_ = 0.0f;
    }

    void AISkillGroundSmash::chargeOnUpdate_( Simian::DelegateParameter& param )
    {
        timer_ += param.As<Simian::EntityUpdateParameter>().Dt;

        if(timer_ >= chargeTime_)
            fsm_.ChangeState(SS_SMASH);
    }

    void AISkillGroundSmash::smashOnEnter_( Simian::DelegateParameter&  )
    {
        model_->Controller().PlayAnimation("Slam", false);
        
        timer_ = 0.0f;
    }

    void AISkillGroundSmash::smashOnUpdate_( Simian::DelegateParameter& param )
    {
        timer_ += param.As<Simian::EntityUpdateParameter>().Dt;

        if(timer_ >= smashTime_)
        {
            fsm_.ChangeState(SS_COOLDOWN);
        }
    }

    void AISkillGroundSmash::coolDownOnEnter_( Simian::DelegateParameter&  )
    {
        Simian::Entity* groundSmash = Target()->ParentScene()->CreateEntity(E_GROUNDSMASHEFFECT);

        if(groundSmash)
        {
            Simian::CTransform* effectTrans = 0;
            groundSmash->ComponentByType(Simian::C_TRANSFORM, effectTrans);
            
            if(effectTrans)
            {
                effectTrans->Position(ownersTrans_->Position());
                Target()->ParentScene()->AddEntity(groundSmash);
            }
        }
        
        if (sounds_)
            sounds_->Play(slamSoundNum_);
        // shake camera
        GCDescensionCamera::Instance()->Shake(0.3f, 0.2f);

        Simian::Entity* crater = Target()->ParentScene()->CreateEntity(E_GROUNDSMASHCRATER);
        if(crater)
        {
            Simian::CTransform* craterTrans = 0;
            crater->ComponentByType(Simian::C_TRANSFORM, craterTrans);
            if(craterTrans)
            {
                craterTrans->Position(ownersTrans_->Position());
                craterTrans->Rotation(Simian::Vector3(0,1,0),ownersTrans_->Angle());
                Target()->ParentScene()->AddEntity(crater);
            }
        }
        updateVecToTarget_();
        Simian::f32 distance = vecToTar_.LengthSquared();
        if(targetAttribs_ && distance < damageRadiusSqr_ )
        {
            CombatCommandSystem::Instance().AddCommand(NULL, targetAttribs_,
                CombatCommandDealDamage::Create(damage_));
        }
        model_->Controller().PlayAnimation("Post Slam", false);
        timer_ = 0.0f;
    }

    void AISkillGroundSmash::coolDownOnUpdate_( Simian::DelegateParameter& param )
    {
        timer_ += param.As<Simian::EntityUpdateParameter>().Dt;
        if(timer_ >= coolDownTime_)
        {
            fsm_.ChangeState(SS_BIRTH);
            smash_ = false;
            if (boneTrail1_)
            {
               boneTrail1_->Enabled(false);
               boneTrail2_->Enabled(false);
            }
        }
    }

    void AISkillGroundSmash::updateVecToTarget_()
    {
        vecToTar_ = srcTrans_->World().Position() - ownersTrans_->World().Position();
    }

    Simian::f32 AISkillGroundSmash::vectorToDegreeFloat_(const Simian::Vector3& vec)
    {
        // Get angle of rotation in degrees for vector
        if(vec.LengthSquared() > Simian::Math::EPSILON)
        {
            Simian::Vector3 tempVec = vec.Normalized();
            Simian::f32 angle = std::acos(tempVec.X());
            if (tempVec.Z() > 0)
                angle = Simian::Math::TAU - angle;
            // Wrap angle within 360
            angle = Simian::Math::WrapAngle(Simian::Radian(angle)).Degrees();

            return angle;
        }
        return 0;
    }
}
