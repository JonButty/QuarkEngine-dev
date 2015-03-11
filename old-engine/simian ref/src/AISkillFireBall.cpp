#include "AISkillFireBall.h"

#include "GCEditor.h"
#include "GCAttributes.h"
#include "CombatCommandDealDamage.h"
#include "GCPhysicsController.h"
#include "GCPlayerLogic.h"
#include "GCEnemyFireBallLogic.h"

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

namespace Descension
{
    static const Simian::f32 BULLET_DIST_FROM_TARGET = 0.4f;

    Simian::FactoryPlant<AIEffectBase, AISkillFireBall> AISkillFireBall::factoryPlant_(
        AIEffectBase::createFactoryPlant_<AISkillFireBall>(EFF_S_FIREBALL));

    //-Public-------------------------------------------------------------------
    AISkillFireBall::AISkillFireBall()
        :   AISkillBase(EFF_S_FIREBALL),
            alive_(true),
            targetTrans_(0),
            trans_(0),
            destinationTrans_(0),
            model_(0),
            damage_(0),
            damageRadius_(0),
            horizSpeed_(0),
            gravity_(0),
            fireBall_(0),
            fireBallExplosion_(0),
            fireBallLogic_(0),
            timer_(0.0f),
            chargeTime_(0.0f),
            explosionTime_(0.0f),
            attackRadius_(0.0f),
            attackRadiusSqr_(0.0f),
            direction_(0.0f),
            bulletStartHeight_(0.0f),
            vecToPlayerCurr_(Simian::Vector3::Zero),
            vecToPlayerInst_(Simian::Vector3::Zero),
            fsm_(FB_START,FB_TOTAL)
    {
    }

    AISkillFireBall::~AISkillFireBall()
    {
    }

    bool AISkillFireBall::Update( Simian::f32 dt )
    {
        if( !Enable() )
            return false;

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

    void AISkillFireBall::onAwake_()
    {
        
    }

    void AISkillFireBall::onInit_( GCAttributes* source )
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

        fsm_[FB_START].OnEnter = Simian::Delegate::CreateDelegate<AISkillFireBall, &AISkillFireBall::startOnEnter_>(this);
        fsm_[FB_START].OnUpdate = Simian::Delegate::CreateDelegate<AISkillFireBall, &AISkillFireBall::startOnUpdate_>(this);
        fsm_[FB_CHARGE].OnEnter = Simian::Delegate::CreateDelegate<AISkillFireBall, &AISkillFireBall::chargeOnEnter_>(this);
        fsm_[FB_CHARGE].OnUpdate = Simian::Delegate::CreateDelegate<AISkillFireBall, &AISkillFireBall::chargeOnUpdate_>(this);
        fsm_[FB_ATTACK].OnEnter = Simian::Delegate::CreateDelegate<AISkillFireBall, &AISkillFireBall::attackOnEnter_>(this);
        fsm_[FB_ATTACK].OnUpdate = Simian::Delegate::CreateDelegate<AISkillFireBall, &AISkillFireBall::attackOnUpdate_>(this);
        fsm_[FB_END].OnEnter = Simian::Delegate::CreateDelegate<AISkillFireBall, &AISkillFireBall::endOnEnter_>(this);
        fsm_[FB_END].OnUpdate = Simian::Delegate::CreateDelegate<AISkillFireBall, &AISkillFireBall::endOnUpdate_>(this);
    }

    void AISkillFireBall::onUnload_()
    {

    }

    void AISkillFireBall::serialize_( Simian::FileObjectSubNode& node )
    {
        node.AddData("ChargeTime", chargeTime_);
        node.AddData("AttackRadius", attackRadius_);
        node.AddData("Damage", damage_);
        node.AddData("BulletStartHeight",bulletStartHeight_);
        node.AddData("DamageRadius", damageRadius_);
        node.AddData("Speed", horizSpeed_);
        node.AddData("Gravity",gravity_);
    }

    void AISkillFireBall::deserialize_( const Simian::FileObjectSubNode& node )
    {
        const Simian::FileDataSubNode* data = node.Data("ChargeTime");
        chargeTime_ = data? data->AsF32() : chargeTime_;
        
        data = node.Data("AttackRadius");
        attackRadius_ = data? data->AsF32(): attackRadius_;

        data = node.Data("Damage");
        damage_ = data? data->AsU32(): damage_;

        data = node.Data("BulletStartHeight");
        bulletStartHeight_ = data? data->AsF32() : bulletStartHeight_;

        data = node.Data("DamageRadius");
        damageRadius_ = data ? data->AsF32() : damageRadius_;

        data = node.Data("Speed");
        horizSpeed_ = data ? data->AsF32() : horizSpeed_;

        data = node.Data("Gravity");
        gravity_ = data ? data->AsF32() : gravity_;
    }

    //--------------------------------------------------------------------------
    
    void AISkillFireBall::startOnEnter_( Simian::DelegateParameter&  )
    {
    }

    void AISkillFireBall::startOnUpdate_( Simian::DelegateParameter& )
    {
        // we dont want to reset alive_ in the same frame when the death state sets it to false
        alive_ = true;
        fsm_.ChangeState(FB_CHARGE);
    }

    void AISkillFireBall::chargeOnEnter_( Simian::DelegateParameter&  )
    {
        facePlayer_();
        vecToPlayerInst_ = vecToPlayerCurr_;

        timer_ = 0.0f;

        model_->Controller().PlayAnimation("Cast Charge",false);

        // create fireball
        // Set Fireball Trans
        fireBall_ = Target()->ParentEntity()->ParentScene()->CreateEntity(E_FIREBALL);
        if( !fireBall_ )
        {
            Enable(false);
            return;
        }

        Target()->Victim()->ComponentByType(Simian::C_TRANSFORM,destinationTrans_);
        fireBall_->ComponentByType(GC_ENEMYFIREBALLLOGIC,fireBallLogic_);
        fireBall_->ComponentByType(Simian::C_TRANSFORM, fireBallTrans_);

        if( !destinationTrans_ || !fireBallLogic_ || !fireBallTrans_ )
        {
            Enable(false);
            return;
        }

        // Set up logic
        fireBallLogic_->TargetTrans(destinationTrans_);
        fireBallLogic_->TargetAttrib(Target()->Victim());

        // If no values provided, use the entity's default values
        if( damage_ > Simian::Math::EPSILON )
            fireBallLogic_->Damage(damage_);
        if( damageRadius_ > Simian::Math::EPSILON )
            fireBallLogic_->DamageRadius(damageRadius_);
        if( horizSpeed_ > Simian::Math::EPSILON )
            fireBallLogic_->HorizSpeed(horizSpeed_);
        if( gravity_ > Simian::Math::EPSILON )
            fireBallLogic_->Gravity(gravity_);
    }

    void AISkillFireBall::chargeOnUpdate_( Simian::DelegateParameter& param )
    {
        Simian::f32 dt = param.As<Simian::EntityUpdateParameter>().Dt;
        timer_ += dt;

        if( timer_ > chargeTime_ )
        {
            fsm_.ChangeState(FB_ATTACK);
        }
    }

    void AISkillFireBall::attackOnEnter_( Simian::DelegateParameter& )
    {
        model_->Controller().PlayAnimation("Cast", false);

        Simian::Vector3 radius = vecToPlayerCurr_.Normalized() * BULLET_DIST_FROM_TARGET;

        fireBallTrans_->Position(Simian::Vector3(targetTrans_->World().Position().X(),
            targetTrans_->World().Position().Y() + bulletStartHeight_, targetTrans_->World().Position().Z()));
        Target()->ParentEntity()->ParentScene()->AddEntity(fireBall_);
    }

    void AISkillFireBall::attackOnUpdate_( Simian::DelegateParameter&  )
    {
        // Change state once fireball has exploded
        if( !fireBallLogic_->Enable() )
            fsm_.ChangeState(FB_END);
    }

    void AISkillFireBall::endOnEnter_( Simian::DelegateParameter& )
    {
        model_->Controller().PlayAnimation("Idle");
    }

    void AISkillFireBall::endOnUpdate_( Simian::DelegateParameter& param )
    {
        timer_ += param.As<Simian::EntityUpdateParameter>().Dt;
        fsm_.ChangeState(FB_START);
        alive_ = false;
    }

    void AISkillFireBall::facePlayer_()
    {
        updateVecToPlayer_();
        Simian::f32 angle = vectorToDegreeFloat_(vecToPlayerCurr_.Normalized());
        targetTrans_->Rotation(Simian::Vector3(0, 1, 0), Simian::Degree(angle + 90.0f));
    }

    Simian::f32 AISkillFireBall::vectorToDegreeFloat_( const Simian::Vector3& vec )
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

    void AISkillFireBall::updateVecToPlayer_()
    {
        vecToPlayerCurr_ = trans_->World().Position() - targetTrans_->World().Position();
    }
}
