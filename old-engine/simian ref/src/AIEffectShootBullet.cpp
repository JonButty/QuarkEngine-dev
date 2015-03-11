/******************************************************************************/
/*!
\file		AIEffectShootBullet.cpp
\author 	Jonathan Butt, j.butt, 250000211
\par    	email: j.butt\@digipen.edu
\date   	2012/11/01
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/******************************************************************************/
#include "Simian/EngineComponents.h"
#include "Simian/Delegate.h"
#include "Simian/CTransform.h"
#include "Simian/CModel.h"
#include "Simian/AnimationController.h"
#include "Simian/Math.h"
#include "Simian/EntityComponent.h"
#include "Simian/Scene.h"
#include "Simian/CSoundEmitter.h"

#include "AIEffectShootBullet.h"

#include "GCAttributes.h"
#include "GCPlayerLogic.h"

#include "EntityTypes.h"
#include "GCEnemyBulletLogic.h"
#include "ComponentTypes.h"
#include "Simian/LogManager.h"
#include "Simian/Angle.h"

namespace Descension
{
    Simian::FactoryPlant<AIEffectBase, AIEffectShootBullet> AIEffectShootBullet::factoryPlant_(
        AIEffectBase::createFactoryPlant_<AIEffectShootBullet>(EFF_E_SHOOTBULLET));

    AIEffectShootBullet::AIEffectShootBullet()
        :   AIEffectBase(EFF_E_SHOOTBULLET),
            sounds_(0),
            ownerTrans_(0),
            targetTrans_(0),
            model_(0),
            targetAttribs_(0),
            chanceHit_(false),
            stateTrigger_(0),
            attackRadius_(0),
            attackRadiusSqr_(0),
            chargeTime_(0),
            shootTime_(0),
            tryAgainTime_(1.0f),
            timer_(0),
            tryAgainTimer_(0),
            chance_(0),
            bulletStartHeight_(0),
            shootAngle_(0),
            vecToPlayer_(Simian::Vector3::Zero),
            fsm_(SS_CHARGE,SS_TOTAL)
    {
    }

    AIEffectShootBullet::~AIEffectShootBullet()
    {

    }

    bool AIEffectShootBullet::Update( Simian::f32 dt )
    {
        if(!Enable()|| GCPlayerLogic::Instance()->Attributes()->Health() < Simian::Math::EPSILON)
            return false;

        vecToPlayer_ = targetTrans_->World().Position() - ownerTrans_->World().Position();
        Simian::f32 squareDist = vecToPlayer_.LengthSquared();

        if((Target()->CurrentState() == stateTrigger_) && (squareDist <= attackRadiusSqr_))
        {
            if(!chanceHit_ && tryAgainTimer_ > tryAgainTime_)
                chanceHit_ = (chance_ >= Simian::Math::Random(0,100));
            
            if(chanceHit_)
            {
                tryAgainTimer_ = 0.0f;
                Target()->MovementSpeed(0);
                fsm_.Update(dt);
                return true;
            }
            tryAgainTimer_ += dt;
        }
        return false;
    }

    //--------------------------------------------------------------------------

    void AIEffectShootBullet::onAwake_()
    {
    }

    void AIEffectShootBullet::onInit_( GCAttributes* source )
    {
        if(source)
        {
            source->ComponentByType(Simian::C_TRANSFORM,targetTrans_);
            targetAttribs_ = source;
        }
        // Default
        else
        {
            GCPlayerLogic::Instance()->ParentEntity()->ParentEntity()->ComponentByType(Simian::C_TRANSFORM,targetTrans_);
            targetAttribs_ = GCPlayerLogic::Instance()->Attributes();
        }

        Target()->ComponentByType(Simian::C_TRANSFORM,ownerTrans_);
        Target()->ComponentByType(Simian::C_MODEL,model_);
        Target()->ComponentByType(Simian::C_SOUNDEMITTER, sounds_);

        if(!ownerTrans_ || !model_ || !sounds_)
        {
            Enable(false);
        }
        fsm_[SS_CHARGE].OnEnter = Simian::Delegate::CreateDelegate<AIEffectShootBullet, &AIEffectShootBullet::chargeOnEnter_>(this);
        fsm_[SS_CHARGE].OnUpdate = Simian::Delegate::CreateDelegate<AIEffectShootBullet, &AIEffectShootBullet::chargeOnUpdate_>(this);
        fsm_[SS_SHOOT].OnEnter = Simian::Delegate::CreateDelegate<AIEffectShootBullet, &AIEffectShootBullet::shootOnEnter_>(this);
        fsm_[SS_SHOOT].OnUpdate = Simian::Delegate::CreateDelegate<AIEffectShootBullet, &AIEffectShootBullet::shootOnUpdate_>(this);
    }

    void AIEffectShootBullet::onUnload_()
    {

    }

    void AIEffectShootBullet::serialize_( Simian::FileObjectSubNode& node )
    {
        node.AddData("AttackRadius",attackRadius_);
        node.AddData("ChargeTime",chargeTime_);
        node.AddData("Chance",chance_);
        node.AddData("StateTrigger",stateTrigger_);
        node.AddData("BulletStartHeight",bulletStartHeight_);
        node.AddData("ShootTime",shootTime_);
    }

    void AIEffectShootBullet::deserialize_( const Simian::FileObjectSubNode& node )
    {
        const Simian::FileDataSubNode* data = node.Data("AttackRadius");
        attackRadius_ = data? data->AsF32() : attackRadius_;
        attackRadiusSqr_ = attackRadius_ * attackRadius_;

        data = node.Data("ChargeTime");
        chargeTime_ = data? data->AsF32() : chargeTime_;

        data = node.Data("Chance");
        chance_ = data? data->AsF32() : chance_;

        data = node.Data("StateTrigger");
        stateTrigger_ = data? data->AsU32() : stateTrigger_;

        data = node.Data("BulletStartHeight");
        bulletStartHeight_ = data? data->AsF32() : bulletStartHeight_;

        data = node.Data("ShootTime");
        shootTime_ = data? data->AsF32() : shootTime_;
    }

    void AIEffectShootBullet::chargeOnEnter_( Simian::DelegateParameter&  )
    {
        timer_ = 0.0f;
        model_->Controller().PlayAnimation("Attack 1 Raise");
    }

    void AIEffectShootBullet::chargeOnUpdate_( Simian::DelegateParameter& param )
    {
        timer_ += param.As<Simian::EntityUpdateParameter>().Dt;

        if(timer_ > chargeTime_)
            fsm_.ChangeState(SS_SHOOT);
    }

    void AIEffectShootBullet::shootOnEnter_( Simian::DelegateParameter&  )
    {
        model_->Controller().PlayAnimation("Attack 1 Down");

        shootAngle_ = vectorToDegreeFloat_(vecToPlayer_);
        ownerTrans_->Rotation(Simian::Vector3(0, 1, 0),Simian::Degree(shootAngle_ + 90.0f));
        Target()->Direction(vecToPlayer_.Normalized());
        Simian::Entity* bullet = Target()->ParentEntity()->ParentScene()->CreateEntity(E_ENEMYBULLET);
        Target()->ParentEntity()->ParentScene()->AddEntity(bullet);
        
        // Pass bullet attribs
        GCEnemyBulletLogic* enemyBullet_;
        bullet->ComponentByType(GC_ENEMYBULLETLOGIC, enemyBullet_);
        enemyBullet_->OwnerAttribs(targetAttribs_);
        
        // Set bullet transform
        Simian::CTransform* bulletTransform_;
        bullet->ComponentByType(Simian::C_TRANSFORM, bulletTransform_);
        bulletTransform_->Position(Simian::Vector3(ownerTrans_->World().Position().X(),
                                    ownerTrans_->World().Position().Y() + bulletStartHeight_,
                                    ownerTrans_->World().Position().Z()));

        
        // Set its velocity
        enemyBullet_->MovementVec(Simian::Vector3(std::cos(Simian::Degree(Simian::Math::TAU - shootAngle_ - 5.0f).Radians()),
            0, std::sin(Simian::Degree(Simian::Math::TAU - shootAngle_ - 5.0f).Radians())));

        // Play sound
        //if (sounds_)
       //     sounds_->Play(S_SHOOT);
        timer_ = 0.0f;
    }

    void AIEffectShootBullet::shootOnUpdate_( Simian::DelegateParameter& param )
    {
        timer_ += param.As<Simian::EntityUpdateParameter>().Dt;

        if(timer_ >= shootTime_)
        {
            chanceHit_ = false;
            fsm_.ChangeState(SS_CHARGE);
        }
    }

    Simian::f32 AIEffectShootBullet::vectorToDegreeFloat_(const Simian::Vector3& vec)
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
}
