/******************************************************************************/
/*!
\file		AISkillProjectile.cpp
\author 	Jonathan Butt, j.butt, 250000211
\par    	email: j.butt\@digipen.edu
\date   	2012/08/10
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/******************************************************************************/
#include "AISkillProjectile.h"

#include "GCEditor.h"
#include "GCAttributes.h"
#include "CombatCommandDealDamage.h"
#include "GCPhysicsController.h"
#include "GCPlayerLogic.h"
#include "GCEnemyProjectileLogic.h"

#include "ComponentTypes.h"
#include "EntityTypes.h"

#include "Simian/Scene.h"
#include "Simian/EngineComponents.h"
#include "Simian/CTransform.h"
#include "Simian/CModel.h"
#include "Simian/AnimationController.h"
#include "Simian/Entity.h"
#include "Simian/EntityComponent.h"
#include "Simian/ParticleSystem.h"
#include "Simian/CParticleSystem.h"
#include "Simian/LogManager.h"
#include "Simian/GameFactory.h"
#include "GCDurgleEggChild.h"

namespace Descension
{
    static const Simian::f32 BULLET_DIST_FROM_TARGET = 0.4f;
    
    Simian::FactoryPlant<AIEffectBase, AISkillProjectile> AISkillProjectile::factoryPlant_(
        AIEffectBase::createFactoryPlant_<AISkillProjectile>(EFF_S_PROJECTILE));

    //-Public-------------------------------------------------------------------
    AISkillProjectile::AISkillProjectile()
        :   AISkillBase(EFF_S_PROJECTILE),
            alive_(true),
            targetTrans_(0),
            trans_(0),
            destinationTrans_(0),
            model_(0),
            damage_(0),
            projectileTypeU_(-1),
            damageRadius_(0),
            horizSpeed_(0),
            gravity_(0),
            projectile_(0),
            fireBallExplosion_(0),
            projectileLogic_(0),
            timer_(0.0f),
            chargeTime_(0.0f),
            attackTime_(0.0f),
            cooldownTime_(0.0f),
            attackRadius_(0.0f),
            attackRadiusSqr_(0.0f),
            direction_(0.0f),
            startHeight_(0.0f),
            projectileType_(""),
            explosionType_(""),
            vecToPlayerCurr_(Simian::Vector3::Zero),
            vecToPlayerInst_(Simian::Vector3::Zero),
            fsm_(FB_START,FB_TOTAL)
    {
    }

    AISkillProjectile::~AISkillProjectile()
    {
    }

    bool AISkillProjectile::Update( Simian::f32 dt )
    {
        if( !Enable())
            return false;
        if(GCPlayerLogic::Instance()->PlayerControlOverride())
        {
            model_->Controller().PlayAnimation("Idle");
            fsm_.ChangeState(FB_START);
            return false;
        }
        updateVecToPlayer_();

        if( vecToPlayerCurr_.LengthSquared() < attackRadiusSqr_ )
        {
            fsm_.Update(dt);
            return alive_;
        }
        else
            fsm_.ChangeState(FB_START);
        return false;
    }

    //-Private------------------------------------------------------------------

    void AISkillProjectile::onAwake_()
    {
        
    }

    void AISkillProjectile::onInit_( GCAttributes* source )
    {
        GCPhysicsController* physics = 0, *targetPhysics = 0;
        if( source )
        {
            source->ComponentByType(Simian::C_TRANSFORM, trans_);
            source->ComponentByType(GC_PHYSICSCONTROLLER, physics);
        }
        else
        {
            GCPlayerLogic::Instance()->ParentEntity()->ParentEntity()->ComponentByType(Simian::C_TRANSFORM, trans_);
            GCPlayerLogic::Instance()->ParentEntity()->ParentEntity()->ComponentByType(GC_PHYSICSCONTROLLER, physics);
        }
        if( Target() )
        {
            Target()->ComponentByType(Simian::C_TRANSFORM,targetTrans_);

            Target()->ComponentByType(GC_PHYSICSCONTROLLER,targetPhysics);

            Target()->ComponentByType(Simian::C_MODEL,model_);
        }

        if( !trans_ || !targetTrans_ || !physics || !targetPhysics || !model_ )
            Enable(false);
        else
        {
            attackRadius_ += targetPhysics->Radius() + physics->Radius();
            attackRadiusSqr_ = attackRadius_ * attackRadius_;
        }

        fsm_[FB_START].OnUpdate = Simian::Delegate::CreateDelegate<AISkillProjectile, &AISkillProjectile::startOnUpdate_>(this);
        fsm_[FB_CHARGE].OnEnter = Simian::Delegate::CreateDelegate<AISkillProjectile, &AISkillProjectile::chargeOnEnter_>(this);
        
        fsm_[FB_CHARGE].OnUpdate = Simian::Delegate::CreateDelegate<AISkillProjectile, &AISkillProjectile::castingUpdate_<FB_ATTACK> >(this);
        
        fsm_[FB_ATTACK].OnEnter = Simian::Delegate::CreateDelegate<AISkillProjectile, &AISkillProjectile::attackOnEnter_>(this);
        fsm_[FB_ATTACK].OnUpdate = Simian::Delegate::CreateDelegate<AISkillProjectile, &AISkillProjectile::castingUpdate_<FB_COOLDOWN> >(this);
        
        fsm_[FB_COOLDOWN].OnEnter = Simian::Delegate::CreateDelegate<AISkillProjectile, &AISkillProjectile::cooldownOnEnter_>(this);
        fsm_[FB_COOLDOWN].OnUpdate = Simian::Delegate::CreateDelegate<AISkillProjectile, &AISkillProjectile::cooldownOnUpdate_>(this);
    }

    void AISkillProjectile::onUnload_()
    {

    }

    void AISkillProjectile::serialize_( Simian::FileObjectSubNode& node )
    {
        node.AddData("ChargeTime", chargeTime_);
        node.AddData("AttackTime", attackTime_);
        node.AddData("CooldownTime", cooldownTime_);
        node.AddData("AttackRadius", attackRadius_);
        node.AddData("Damage", damage_);
        node.AddData("StartHeight",startHeight_);
        node.AddData("DamageRadius", damageRadius_);
        node.AddData("Speed", horizSpeed_);
        node.AddData("Gravity",gravity_);
        node.AddData("ProjectileType",projectileType_);
        node.AddData("ExplosionType",explosionType_);
    }

    void AISkillProjectile::deserialize_( const Simian::FileObjectSubNode& node )
    {
        node.Data("ChargeTime",chargeTime_,chargeTime_);
        node.Data("AttackTime",attackTime_,attackTime_);
        node.Data("CooldownTime", cooldownTime_,cooldownTime_);
        node.Data("AttackRadius",attackRadius_,attackRadius_);
        node.Data("Damage",damage_,damage_);
        node.Data("StartHeight",startHeight_,startHeight_);
        node.Data("DamageRadius",damageRadius_,damageRadius_);
        node.Data("Speed",horizSpeed_,horizSpeed_);
        node.Data("Gravity",gravity_,gravity_);
        node.Data("ProjectileType",projectileType_,projectileType_);
        if(projectileType_.size() > 0)
            projectileTypeU_ = Simian::GameFactory::Instance().EntityTable().Value(projectileType_);
        node.Data("ExplosionType",explosionType_,explosionType_);
    }

    //--------------------------------------------------------------------------
    
    void AISkillProjectile::startOnUpdate_( Simian::DelegateParameter& )
    {
        // we dont want to reset alive_ in the same frame when the death state sets it to false
        alive_ = true;
        fsm_.ChangeState(FB_CHARGE);
    }

    void AISkillProjectile::chargeOnEnter_( Simian::DelegateParameter&  )
    {
        Simian::Entity* ent = Target()->ParentEntity()->ChildByType(E_DURGLEEGGCHILD);
        if(ent)
        {
            GCDurgleEggChild* logic;
            ent->ComponentByType(GC_DURGLEEGGCHILD,logic);
            logic->Start();
        }
        facePlayer_();
        vecToPlayerInst_ = vecToPlayerCurr_;

        timer_ = chargeTime_;

        model_->Controller().PlayAnimation("Cast Charge",false);

        if(projectileTypeU_ > 0)
            projectile_ = Target()->ParentEntity()->ParentScene()->CreateEntity(projectileTypeU_);
        
        if( !projectile_ )
        {
            Enable(false);
            return;
        }
        
        Target()->Victim()->ComponentByType(Simian::C_TRANSFORM,destinationTrans_);
        projectile_->ComponentByType(GC_ENEMYPROJECTILELOGIC,projectileLogic_);
        projectile_->ComponentByType(Simian::C_TRANSFORM, projectileTrans_);

        if( !destinationTrans_ || !projectileLogic_ || !projectileTrans_ )
        {
            Enable(false);
            return;
        }

        if(explosionType_.size() > 0)
            projectileLogic_->ExplosionType(explosionType_);
        // Set up logic
        projectileLogic_->TargetTrans(destinationTrans_);
        projectileLogic_->TargetAttrib(Target()->Victim());

        // If no values provided, use the entity's default values
        if( damage_ > Simian::Math::EPSILON )
            projectileLogic_->Damage(damage_);
        if( damageRadius_ > Simian::Math::EPSILON )
            projectileLogic_->DamageRadius(damageRadius_);
        if( horizSpeed_ > Simian::Math::EPSILON )
            projectileLogic_->HorizSpeed(horizSpeed_);
        if( gravity_ > Simian::Math::EPSILON )
            projectileLogic_->Gravity(gravity_);
    }

    void AISkillProjectile::attackOnEnter_( Simian::DelegateParameter& )
    {
        model_->Controller().PlayAnimation("Cast", false);

        Simian::Vector3 radius = vecToPlayerCurr_.Normalized() * BULLET_DIST_FROM_TARGET;

        projectileTrans_->Position(Simian::Vector3(targetTrans_->World().Position().X(),
            targetTrans_->World().Position().Y() + startHeight_, targetTrans_->World().Position().Z()));
        Target()->ParentEntity()->ParentScene()->AddEntity(projectile_);
        timer_ = attackTime_;
    }

    void AISkillProjectile::cooldownOnEnter_( Simian::DelegateParameter& )
    {
        model_->Controller().PlayAnimation("Idle");
        timer_ = cooldownTime_;
    }

    void AISkillProjectile::cooldownOnUpdate_( Simian::DelegateParameter& param )
    {
        timer_ -= param.As<Simian::EntityUpdateParameter>().Dt;
        if(timer_ < 0.0f)
        {
            fsm_.ChangeState(FB_START);
            alive_ = false;
        }
    }

    void AISkillProjectile::facePlayer_()
    {
        updateVecToPlayer_();
        Simian::f32 angle = vectorToDegreeFloat_(vecToPlayerCurr_.Normalized());
        targetTrans_->Rotation(Simian::Vector3(0, 1, 0), Simian::Degree(angle + 90.0f));
    }

    Simian::f32 AISkillProjectile::vectorToDegreeFloat_( const Simian::Vector3& vec )
    {
        // Get angle of rotation in degrees for vector
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

    void AISkillProjectile::updateVecToPlayer_()
    {
        vecToPlayerCurr_ = trans_->World().Position() - targetTrans_->World().Position();
    }
}
