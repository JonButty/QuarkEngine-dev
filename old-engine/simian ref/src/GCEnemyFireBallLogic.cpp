#include "Simian/CTransform.h"
#include "Simian/Math.h"
#include "Simian/EnginePhases.h"
#include "Simian/Delegate.h"

#include "CombatCommandSystem.h"
#include "CombatCommandDealDamage.h"
#include "GCEnemyFireBallLogic.h"

#include "ComponentTypes.h"
#include "Simian/EntityComponent.h"
#include "Simian/Scene.h"
#include "EntityTypes.h"
#include "Simian/CParticleSystem.h"
#include "Simian/ParticleSystem.h"
#include "GCEditor.h"
#include "Simian/LogManager.h"
#include "Simian/DebugRenderer.h"

namespace Descension
{
    static const float DEFAULT_GRAVITY = 9.8f;

    Simian::FactoryPlant<Simian::EntityComponent, GCEnemyFireBallLogic> GCEnemyFireBallLogic::factoryPlant_(
        &Simian::GameFactory::Instance().ComponentFactory(), GC_ENEMYFIREBALLLOGIC);

    GCEnemyFireBallLogic::GCEnemyFireBallLogic()
        :   enable_(true),
            targetTrans_(0),
            currPos_(0),
            damage_(0),
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
            dir_(Simian::Vector3::Zero),
            destPos_(Simian::Vector3::Zero),
            fsm_(FBS_BIRTH,FBS_TOTAL)
    {

    }

    void GCEnemyFireBallLogic::OnSharedLoad()
    {
        RegisterCallback(Simian::P_UPDATE, Simian::Delegate::CreateDelegate<GCEnemyFireBallLogic, &GCEnemyFireBallLogic::update_>(this));
    }

    void GCEnemyFireBallLogic::OnAwake()
    {
        fsm_[FBS_BIRTH].OnEnter = Simian::Delegate::CreateDelegate<GCEnemyFireBallLogic, &GCEnemyFireBallLogic::birthOnEnter_>(this);
        fsm_[FBS_BIRTH].OnUpdate = Simian::Delegate::CreateDelegate<GCEnemyFireBallLogic, &GCEnemyFireBallLogic::birthOnUpdate_>(this);
        fsm_[FBS_FLIGHT].OnEnter = Simian::Delegate::CreateDelegate<GCEnemyFireBallLogic, &GCEnemyFireBallLogic::flightOnEnter_>(this);
        fsm_[FBS_FLIGHT].OnUpdate = Simian::Delegate::CreateDelegate<GCEnemyFireBallLogic, &GCEnemyFireBallLogic::flightOnUpdate_>(this);
        fsm_[FBS_DEATH].OnEnter = Simian::Delegate::CreateDelegate<GCEnemyFireBallLogic, &GCEnemyFireBallLogic::deathOnEnter_>(this);
        fsm_[FBS_DEATH].OnUpdate = Simian::Delegate::CreateDelegate<GCEnemyFireBallLogic, &GCEnemyFireBallLogic::deathOnUpdate_>(this);
    }

    void GCEnemyFireBallLogic::OnInit()
    {
#ifdef _DESCENDED
        if (GCEditor::EditorMode())
            return;
#endif  
    }

    void GCEnemyFireBallLogic::Serialize( Simian::FileObjectSubNode& node )
    {
        node.AddData("Damage", damage_);
        node.AddData("DamageRadius", damageRadius_);
        node.AddData("Speed", horizSpeed_);
        node.AddData("Gravity",gravity_);
    }

    void GCEnemyFireBallLogic::Deserialize( const Simian::FileObjectSubNode& node )
    {
        const Simian::FileDataSubNode* data = node.Data("Damage");
        damage_ = data ? data->AsU32() : damage_;

        data = node.Data("DamageRadius");
        damageRadius_ = data ? data->AsF32() : damageRadius_;

        data = node.Data("Speed");
        horizSpeed_ = data ? data->AsF32() : horizSpeed_;

        data = node.Data("Gravity");
        Gravity(data ? data->AsF32() : gravity_);
    }

    //--------------------------------------------------------------------------

    bool GCEnemyFireBallLogic::Enable() const
    {
        return enable_;
    }

    void GCEnemyFireBallLogic::Enable( bool val )
    {
        enable_ = val;
    }

    void GCEnemyFireBallLogic::TargetTrans( Simian::CTransform* val )
    {
        targetTrans_ = val;
        destPos_ = targetTrans_->World().Position();
    }

    Simian::CTransform* GCEnemyFireBallLogic::TargetTrans()
    {
        return targetTrans_;    
    }

    GCAttributes* GCEnemyFireBallLogic::TargetAttrib() const
    {
        return targetAttrib_;
    }

    void GCEnemyFireBallLogic::TargetAttrib( GCAttributes* val )
    {
        targetAttrib_ = val;
    }

    Simian::u32 GCEnemyFireBallLogic::Damage() const
    {
        return damage_;
    }

    void GCEnemyFireBallLogic::Damage( Simian::u32 val )
    {
        damage_ = val;
    }

    Simian::f32 GCEnemyFireBallLogic::DamageRadius() const
    {
        return damageRadius_;
    }

    void GCEnemyFireBallLogic::DamageRadius( Simian::f32 val )
    {
        damageRadius_ = val;
    }

    Simian::f32 GCEnemyFireBallLogic::HorizSpeed() const
    {
        return horizSpeed_;
    }

    void GCEnemyFireBallLogic::HorizSpeed( Simian::f32 val )
    {
        horizSpeed_ = val;
    }

    Simian::f32 GCEnemyFireBallLogic::Gravity() const
    {
        return gravity_;
    }

    void GCEnemyFireBallLogic::Gravity( Simian::f32 val )
    {
        gravity_ = val;
        halfGravity_ = gravity_/2;
    }

    //-Private------------------------------------------------------------------

    void GCEnemyFireBallLogic::update_( Simian::DelegateParameter& param )
    {
#ifdef _DESCENDED
        param;
        if (GCEditor::EditorMode())
            return;
#endif
        if( !enable_ )
            return;
        Simian::f32 dt = param.As<Simian::EntityUpdateParameter>().Dt;
        fsm_.Update(dt);
    }

    void GCEnemyFireBallLogic::birthOnEnter_( Simian::DelegateParameter&  )
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
    }

    void GCEnemyFireBallLogic::birthOnUpdate_( Simian::DelegateParameter&  )
    {
        fsm_.ChangeState(FBS_FLIGHT);
    }

    void GCEnemyFireBallLogic::flightOnEnter_( Simian::DelegateParameter&  )
    {
    }

    void GCEnemyFireBallLogic::flightOnUpdate_( Simian::DelegateParameter& param )
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

    void GCEnemyFireBallLogic::deathOnEnter_( Simian::DelegateParameter&  )
    {
        // Create Explosion
        Simian::Entity* fireBallExplosion = ParentEntity()->ParentScene()->CreateEntity(E_FIREBALLEXPLOSION);
        if( !fireBallExplosion )
        {
            Enable(false);
            return;
        }
        
        Simian::CTransform* trans = 0;
        fireBallExplosion->ComponentByType(Simian::C_TRANSFORM, trans);

        if( !trans )
        {
            Enable(false);
            return;
        }

        trans->Position(destPos_);
        ParentEntity()->ParentScene()->AddEntity(fireBallExplosion);
        Simian::Entity* child = fireBallExplosion->ChildByName("Sparks");

        if( !child )
        {
            Enable(false);
            return;
        }

        Simian::CParticleSystem* particle = 0;
        child->ComponentByType(Simian::C_PARTICLESYSTEM,particle);
        if( !particle )
        {
            Enable(false);
            return;
        }
        explosionTime_ = particle->ParticleSystem()->LifeTime();

        Simian::f32 distance = (targetTrans_->World().Position() - destPos_).LengthSquared();
        
        // Check if player is within AOE
        if(targetAttrib_ && distance < damageRadius_ * damageRadius_ )
        {
            CombatCommandSystem::Instance().AddCommand(NULL, targetAttrib_,
                CombatCommandDealDamage::Create(damage_));
        }
        
        timer_ = 0.0f;
    }

    void GCEnemyFireBallLogic::deathOnUpdate_( Simian::DelegateParameter& param )
    {
        timer_ += param.As<Simian::EntityUpdateParameter>().Dt;
        
        if(timer_ < explosionTime_)
        {
            enable_ = false;
            ParentEntity()->ParentScene()->RemoveEntity(ParentEntity());
        }
    }
}
