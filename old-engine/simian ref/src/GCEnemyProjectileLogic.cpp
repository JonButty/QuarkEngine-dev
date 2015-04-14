/******************************************************************************/
/*!
\file		GCEnemyProjectileLogic.cpp
\author 	Jonathan Butt, j.butt, 250000211
\par    	email: j.butt\@digipen.edu
\date   	2012/08/10
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/******************************************************************************/
#include "Simian/CTransform.h"
#include "Simian/Math.h"
#include "Simian/EnginePhases.h"
#include "Simian/Delegate.h"
#include "Simian/CParticleSystem.h"
#include "Simian/ParticleSystem.h"
#include "Simian/EntityComponent.h"
#include "Simian/Scene.h"
#include "Simian/CSoundEmitter.h"
#include "Simian/CModelColorOperation.h"

#include "CombatCommandSystem.h"
#include "CombatCommandDealDamage.h"

#include "GCEnemyProjectileLogic.h"
#include "GCExplosionLogic.h"
#include "GCEditor.h"

#include "ComponentTypes.h"
#include "EntityTypes.h"
#include "Simian/CModel.h"
#include "Simian/AnimationController.h"
#include "Simian/LogManager.h"
#include "GCPlayerLogic.h"

namespace Descension
{
    static const float DEFAULT_GRAVITY = 9.8f;
    static const float FADE_SPEED = 1.25f;
    static const float DROP_SPEED = 1.5f;

    Simian::FactoryPlant<Simian::EntityComponent, GCEnemyProjectileLogic> GCEnemyProjectileLogic::factoryPlant_(
        &Simian::GameFactory::Instance().ComponentFactory(), GC_ENEMYPROJECTILELOGIC);

    GCEnemyProjectileLogic::GCEnemyProjectileLogic()
        :   enable_(true),
            fadeAfterDestroy_(false),
            targetTrans_(0),
            sounds_(0),
            modelColor_(0),
            currPos_(0),
            damage_(0),
            explosionTypeU_(-1),
            targetAttrib_(0),
            damageRadius_(0),
            horizSpeed_(0),
            vertInitSpeed_(0),
            timer_(0),
            flightTime_(0),
            explosionTime_(0),
            gravity_(DEFAULT_GRAVITY),
            halfGravity_(DEFAULT_GRAVITY/2),
            startHeight_(0),
            explosionType_(""),
            explosionAnimation_(""),
            dir_(Simian::Vector3::Zero),
            destPos_(Simian::Vector3::Zero),
            fsm_(FBS_BIRTH,FBS_TOTAL)
    {
    }

    void GCEnemyProjectileLogic::OnSharedLoad()
    {
        RegisterCallback(Simian::P_UPDATE, Simian::Delegate::CreateDelegate<GCEnemyProjectileLogic, &GCEnemyProjectileLogic::update_>(this));
    }

    void GCEnemyProjectileLogic::OnAwake()
    {
        fsm_[FBS_BIRTH].OnEnter = Simian::Delegate::CreateDelegate<GCEnemyProjectileLogic, &GCEnemyProjectileLogic::birthOnEnter_>(this);
        fsm_[FBS_BIRTH].OnUpdate = Simian::Delegate::CreateDelegate<GCEnemyProjectileLogic, &GCEnemyProjectileLogic::birthOnUpdate_>(this);
        fsm_[FBS_FLIGHT].OnEnter = Simian::Delegate::CreateDelegate<GCEnemyProjectileLogic, &GCEnemyProjectileLogic::flightOnEnter_>(this);
        fsm_[FBS_FLIGHT].OnUpdate = Simian::Delegate::CreateDelegate<GCEnemyProjectileLogic, &GCEnemyProjectileLogic::flightOnUpdate_>(this);
        fsm_[FBS_DEATH].OnEnter = Simian::Delegate::CreateDelegate<GCEnemyProjectileLogic, &GCEnemyProjectileLogic::deathOnEnter_>(this);
        fsm_[FBS_DEATH].OnUpdate = Simian::Delegate::CreateDelegate<GCEnemyProjectileLogic, &GCEnemyProjectileLogic::deathOnUpdate_>(this);
    }

    void GCEnemyProjectileLogic::OnInit()
    {
#ifdef _DESCENDED
        if (GCEditor::EditorMode())
            return;
#endif  
        ComponentByType(Simian::C_SOUNDEMITTER, sounds_);
        ComponentByType(Simian::C_MODELCOLOROPERATION, modelColor_);
        if (modelColor_)
            modelColor_->Operation(modelColor_->CO_MODULATE);
    }

    void GCEnemyProjectileLogic::Serialize( Simian::FileObjectSubNode& node )
    {
        node.AddData("Damage", damage_);
        node.AddData("DamageRadius", damageRadius_);
        node.AddData("Speed", horizSpeed_);
        node.AddData("Gravity",gravity_);
        node.AddData("ExplosionTime",gravity_);
        node.AddData("ExplosionType",explosionType_);
        node.AddData("ExplosionAnimation",explosionAnimation_);
        node.AddData("FadeAfterDestroy",fadeAfterDestroy_);
    }

    void GCEnemyProjectileLogic::Deserialize( const Simian::FileObjectSubNode& node )
    {
        node.Data("Damage",damage_,damage_);

        node.Data("DamageRadius",damageRadius_,damageRadius_);

        node.Data("Speed",horizSpeed_,horizSpeed_);

        node.Data("Gravity",gravity_,gravity_);

        node.Data("ExplosionTime",explosionTime_,explosionTime_);

        node.Data("ExplosionType",explosionType_,explosionType_);

        node.Data("ExplosionAnimation",explosionAnimation_,explosionAnimation_);

        node.Data("FadeAfterDestroy",fadeAfterDestroy_,fadeAfterDestroy_);

        if(explosionType_.size() > 0)
            explosionTypeU_ = Simian::GameFactory::Instance().EntityTable().Value(explosionType_);
    }

    //--------------------------------------------------------------------------

    bool GCEnemyProjectileLogic::Enable() const
    {
        return enable_;
    }

    void GCEnemyProjectileLogic::Enable( bool val )
    {
        enable_ = val;
    }

    void GCEnemyProjectileLogic::TargetTrans( Simian::CTransform* val )
    {
        targetTrans_ = val;
        destPos_ = targetTrans_->World().Position();
    }

    Simian::CTransform* GCEnemyProjectileLogic::TargetTrans()
    {
        return targetTrans_;    
    }

    GCAttributes* GCEnemyProjectileLogic::TargetAttrib() const
    {
        return targetAttrib_;
    }

    void GCEnemyProjectileLogic::TargetAttrib( GCAttributes* val )
    {
        targetAttrib_ = val;
    }

    Simian::u32 GCEnemyProjectileLogic::Damage() const
    {
        return damage_;
    }

    void GCEnemyProjectileLogic::Damage( Simian::u32 val )
    {
        damage_ = val;
    }

    void GCEnemyProjectileLogic::ExplosionType( const std::string& val )
    {
        explosionType_ = val;
    }

    Simian::f32 GCEnemyProjectileLogic::DamageRadius() const
    {
        return damageRadius_;
    }

    void GCEnemyProjectileLogic::DamageRadius( Simian::f32 val )
    {
        damageRadius_ = val;
    }

    Simian::f32 GCEnemyProjectileLogic::HorizSpeed() const
    {
        return horizSpeed_;
    }

    void GCEnemyProjectileLogic::HorizSpeed( Simian::f32 val )
    {
        horizSpeed_ = val;
    }

    Simian::f32 GCEnemyProjectileLogic::Gravity() const
    {
        return gravity_;
    }

    void GCEnemyProjectileLogic::Gravity( Simian::f32 val )
    {
        gravity_ = val;
        halfGravity_ = gravity_/2;
    }

    //-Private------------------------------------------------------------------

    void GCEnemyProjectileLogic::update_( Simian::DelegateParameter& param )
    {
#ifdef _DESCENDED
        param;
        if (GCEditor::EditorMode())
            return;
#endif
        if( !enable_ )
            return;
        if(GCPlayerLogic::Instance()->PlayerControlOverride())
        {
            ParentScene()->RemoveEntity(ParentEntity());
            return;
        }
        Simian::f32 dt = param.As<Simian::EntityUpdateParameter>().Dt;
        fsm_.Update(dt);
    }

    void GCEnemyProjectileLogic::birthOnEnter_( Simian::DelegateParameter&  )
    {
        ParentEntity()->ComponentByType(Simian::C_TRANSFORM,currPos_);
        if( !currPos_  )
            enable_ = false;

        if( gravity_ < Simian::Math::EPSILON )
            gravity_ = DEFAULT_GRAVITY;

        timer_ = 0.0f;

        startHeight_ = currPos_->Position().Y();
        dir_ = destPos_ - currPos_->Position();
        dir_.Y(0.0f);
        Simian::f32 sqrLen = dir_.LengthSquared();
        
        // Target is too close
        if( sqrLen < Simian::Math::EPSILON )
        {
            enable_ = false;
            return;
        }

        Simian::f32 distance  = std::abs(dir_.Length());
        flightTime_ = distance / horizSpeed_;
        Simian::f32 halfTime = flightTime_ / 2;

        vertInitSpeed_ = gravity_ * halfTime;
        dir_.Normalize();

        if (sounds_)
            sounds_->Play(0);
    }

    void GCEnemyProjectileLogic::birthOnUpdate_( Simian::DelegateParameter&  )
    {
        fsm_.ChangeState(FBS_FLIGHT);
    }

    void GCEnemyProjectileLogic::flightOnEnter_( Simian::DelegateParameter&  )
    {
    }

    void GCEnemyProjectileLogic::flightOnUpdate_( Simian::DelegateParameter& param )
    {
        Simian::f32 dt = param.As<Simian::EntityUpdateParameter>().Dt;

        timer_ += dt;

        Simian::f32 scaledSpeed = horizSpeed_ * dt;

        Simian::Vector3 curr(currPos_->World().Position());
        curr.X(curr.X() + dir_.X() * scaledSpeed);
        curr.Y(startHeight_ + (vertInitSpeed_  - (halfGravity_ * timer_)) * timer_);
        curr.Z(curr.Z() + dir_.Z() * scaledSpeed);
        currPos_->Position(curr);

        if( timer_ > flightTime_ )
            fsm_.ChangeState(FBS_DEATH);
    }

    void GCEnemyProjectileLogic::deathOnEnter_( Simian::DelegateParameter&  )
    {
        // Create Explosion
        if (sounds_)
            sounds_->Play(1);

        if(explosionAnimation_.size() > 0)
        {
            Simian::CModel* model;
            ParentEntity()->ComponentByType(Simian::C_MODEL,model);
            model->Controller().PlayAnimation(explosionAnimation_,false);
        }

        Simian::Entity* projectileExplosion = ParentEntity()->ParentScene()->CreateEntity(explosionTypeU_);
        if( !projectileExplosion )
        {
            Enable(false);
            return;
        }
        
        Simian::CTransform* trans = 0;

        projectileExplosion->ComponentByType(Simian::C_TRANSFORM, trans);
        if( !trans )
        {
            Enable(false);
            return;
        }

        trans->Position(destPos_);
        targetTrans_->WorldPosition().Y(0);
        Simian::f32 distance = (targetTrans_->World().Position() - destPos_).LengthSquared();
        
        // Check if player is within AOE
        if(targetAttrib_ && distance < damageRadius_ * damageRadius_ )
        {
            CombatCommandSystem::Instance().AddCommand(NULL, targetAttrib_,
                CombatCommandDealDamage::Create(damage_));
        }
        ParentEntity()->ParentScene()->AddEntity(projectileExplosion);
        timer_ = 0.0f;
    }

    void GCEnemyProjectileLogic::deathOnUpdate_( Simian::DelegateParameter& param )
    {
        timer_ += param.As<Simian::EntityUpdateParameter>().Dt;

        if (fadeAfterDestroy_ && currPos_ && currPos_->Position().Y() > 0.0f)
        {
            currPos_->Position(Simian::Vector3(currPos_->Position().X(),
                    currPos_->Position().Y()-(DROP_SPEED*param.As<Simian::EntityUpdateParameter>().Dt),
                    currPos_->Position().Z()));
        }

        if(timer_ > explosionTime_)
        {
            if (!fadeAfterDestroy_)
            {
                enable_ = false;
                ParentEntity()->ParentScene()->RemoveEntity(ParentEntity());
            }
            else
            {
                if (modelColor_)
                {
                    modelColor_->Color(Simian::Color(
                        modelColor_->Color().R(),
                        modelColor_->Color().G(),
                        modelColor_->Color().B(),
                        modelColor_->Color().A()-(FADE_SPEED*param.As<Simian::EntityUpdateParameter>().Dt)));
                    if (modelColor_->Color().A() <= 0.0f)
                    {
                        modelColor_->Color(Simian::Color(
                            modelColor_->Color().R(),
                            modelColor_->Color().G(),
                            modelColor_->Color().B(), 0.0f));
                        enable_ = false;
                        ParentEntity()->ParentScene()->RemoveEntity(ParentEntity());
                    }
                }
            }
        }
    }
}
