/*************************************************************************/
/*!
\file		GCCollectible.cpp
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	2012/08/10
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*************************************************************************/
#include "GCCollectible.h"
#include "ComponentTypes.h"
#include "GCEnemyDrop.h"
#include "GCPlayerLogic.h"
#include "CombatCommandSystem.h"
#include "CombatCommandHeal.h"

#include "Simian/EnginePhases.h"
#include "Simian/EngineComponents.h"
#include "Simian/CTransform.h"
#include "Simian/Math.h"
#include "Simian/Interpolation.h"
#include "Simian/Scene.h"
#include "Simian/CSoundEmitter.h"
#include "Simian/CParticleSystem.h"
#include "Simian/ParticleSystem.h"
#include "EntityTypes.h"

#include "GCPhysicsController.h"
#include "GCEditor.h"

namespace Descension
{
    const Simian::f32 VEL_SCALE = 0.5f;

    const Simian::f32 SPIN_VEL_MIN = 1.0f;
    const Simian::f32 SPIN_VEL_MAX = 10.0f;

    const Simian::f32 COLLECTION_TIME = 5.0f;

    const Simian::f32 COLLECTION_RADIUS = 1.0f;

    const Simian::f32 GROUND_OFFSET = 0.5f;

    Simian::FactoryPlant<Simian::EntityComponent, GCCollectible> GCCollectible::factoryPlant_(
        &Simian::GameFactory::Instance().ComponentFactory(), GC_COLLECTIBLE);

    GCCollectible::GCCollectible()
        : healthIncrement_(0),
          manaIncrement_(0),
          sounds_(0),
          yawVelocity_(0),
          pitchVelocity_(0),
          spinVelocity_(0),
          yaw_(0),
          pitch_(0),
          spin_(0),
          timer_(0),
          gravity_(true),
          collectionRadius_(COLLECTION_RADIUS),
          collectionTime_(COLLECTION_TIME),
          deathDelay_(0.0f),
          minDelay_(1.2f),
          groundOffset_(GROUND_OFFSET),
          fsm_(CS_DROPPING, CS_TOTAL)
    {
    }

    //--------------------------------------------------------------------------

    const Simian::Vector3& GCCollectible::Velocity() const
    {
        return velocity_;
    }

    void GCCollectible::Velocity( const Simian::Vector3& val )
    {
        velocity_ = val;
    }

    //--------------------------------------------------------------------------

    void GCCollectible::onUpdate_( Simian::DelegateParameter& param )
    {
#ifdef _DESCENDED
        if (GCEditor::Instance().EditorMode())
            return;
#endif

        Simian::EntityUpdateParameter* p;
        param.As(p);
        fsm_.Update(p->Dt);
    }

    void GCCollectible::droppingUpdate_( Simian::DelegateParameter& param )
    {
        Simian::FiniteStateUpdateParameter* p;
        param.As(p);

        timer_ += p->Dt;

        // integrate gravity
        if (gravity_)
        {
            velocity_ += Simian::Vector3(0, -GRAVITATIONAL_ACCELERATION, 0) * p->Dt;

            // find the new position
            Simian::Vector3 v = velocity_ * p->Dt;
            // find time of intersection
            float t = -transform_->Position().Y()/v.Y();

            // if it goes below the ground reverse the velocity
            Simian::Vector3 newPos = transform_->Position();
            if (t > 0.0f && t < 1.0f)
            {
                newPos += t * v;
                velocity_.Y(-velocity_.Y());
                velocity_ *= VEL_SCALE;
                newPos += velocity_ * p->Dt * (1.0f - t);

                // recalculate spin
                yawVelocity_ *= VEL_SCALE;
                pitchVelocity_ *= VEL_SCALE;
                spinVelocity_ *= VEL_SCALE;
            }
            else
                newPos += v;

            // shift position by velocity
            transform_->Position(newPos);

            // update velocity
            yaw_ += yawVelocity_ * p->Dt;
            pitch_ += pitchVelocity_ * p->Dt;
            spin_ += spinVelocity_ * p->Dt;
        }

        /*Simian::Matrix rot = Simian::Matrix::RotationAxisAngle(Simian::Vector3::UnitY, Simian::Radian(yaw_)) * 
            Simian::Matrix::RotationAxisAngle(Simian::Vector3::UnitX, Simian::Radian(pitch_));
        Simian::Vector3 axis(0, 1, 0);
        rot.Transform(axis);
        transform_->Rotation(axis, Simian::Radian(spin_));*/

        Simian::Vector3 dist = 
            GCPlayerLogic::Instance()->Transform().World().Position() - transform_->World().Position();
        if (timer_ > minDelay_ && dist.LengthSquared() < collectionRadius_ * collectionRadius_)
        {
            timer_ = 0.0f;
            if (!GCPlayerLogic::Instance()->Attributes()->Death())
                fsm_.ChangeState(CS_COLLECTING);
        }
    }

    void GCCollectible::collectingUpdate_( Simian::DelegateParameter& param )
    {
        Simian::FiniteStateUpdateParameter* p;
        param.As(p);
        
        timer_ += p->Dt;
        Simian::f32 intp = timer_/collectionTime_;
        intp = Simian::Math::Clamp(intp, 0.0f, 1.0f);

        Simian::Vector3 destination = GCPlayerLogic::Instance()->Transform().World().Position() +
            Simian::Vector3(0, groundOffset_, 0);

        // get vector between this and player
        Simian::Vector3 pos = Simian::Interpolation::Interpolate(transform_->World().Position(), 
            destination, intp, Simian::Interpolation::Linear);

        // set the position
        transform_->Position(pos);

        if ((transform_->World().Position() - destination).LengthSquared() < 
            GCPlayerLogic::Instance()->Physics()->Radius() * GCPlayerLogic::Instance()->Physics()->Radius())
        {
            // collect it!
            CombatCommandSystem::Instance().AddCommand(0, GCPlayerLogic::Instance()->Attributes(),
                CombatCommandHeal::Create(healthIncrement_, manaIncrement_));

            // Play sound
            if (sounds_)
                sounds_->Play(0);

            // Get healing particle effects if health or mana regen
            if (healthIncrement_ > 0 || manaIncrement_ > Simian::Math::EPSILON)
            {
                Simian::Entity* particles = 0;
                if (healthIncrement_ > 0)
                    particles = ParentEntity()->ParentScene()->CreateEntity(E_REGENHEALTHPARTICLES);
                else
                    particles = ParentEntity()->ParentScene()->CreateEntity(E_REGENMANAPARTICLES);
                if( particles )
                {
                    ParentScene()->AddEntity(particles);
                    Simian::CTransform* transferParticlestransform_;
                    Simian::CParticleSystem* transferParticles_;
                    particles->ComponentByType(Simian::C_TRANSFORM, transferParticlestransform_);
                    particles->ComponentByType(Simian::C_PARTICLESYSTEM,transferParticles_);
                    
                    transferParticlestransform_->Position(GCPlayerLogic::Instance()->Transform().World().Position());
                    transferParticles_->ParticleSystem()->Enabled(true);
                }
            }

            fsm_.ChangeState(CS_DYING);
        }
    }

    void GCCollectible::dyingUpdate_( Simian::DelegateParameter& param )
    {
        Simian::FiniteStateUpdateParameter* p;
        param.As(p);
        timer_ += p->Dt;

        if (timer_ > deathDelay_)
        {
            // queue this entity for removal
            if (ParentEntity()->ParentEntity())
                ParentEntity()->ParentEntity()->RemoveChild(ParentEntity());
            else
                ParentScene()->RemoveEntity(ParentEntity());
        }
    }

    //--------------------------------------------------------------------------

    void GCCollectible::OnSharedLoad()
    {
        RegisterCallback(Simian::P_UPDATE, Simian::Delegate::CreateDelegate<GCCollectible, &GCCollectible::onUpdate_>(this));
    }

    void GCCollectible::OnAwake()
    {
        ComponentByType(Simian::C_TRANSFORM, transform_);
        ComponentByType(Simian::C_SOUNDEMITTER, sounds_);

        yawVelocity_ = Simian::Math::Random(SPIN_VEL_MIN, SPIN_VEL_MAX);
        pitchVelocity_ = Simian::Math::Random(SPIN_VEL_MIN, SPIN_VEL_MAX);
        spinVelocity_ = Simian::Math::Random(SPIN_VEL_MIN, SPIN_VEL_MAX);

        fsm_[CS_DROPPING].OnUpdate = Simian::Delegate::CreateDelegate<GCCollectible, &GCCollectible::droppingUpdate_>(this);
        fsm_[CS_COLLECTING].OnUpdate = Simian::Delegate::CreateDelegate<GCCollectible, &GCCollectible::collectingUpdate_>(this);
        fsm_[CS_DYING].OnUpdate = Simian::Delegate::CreateDelegate<GCCollectible, &GCCollectible::dyingUpdate_>(this);
    }

    void GCCollectible::OnDeinit()
    {

    }

    void GCCollectible::Serialize( Simian::FileObjectSubNode& data )
    {
        data.AddData("HealthIncrement", healthIncrement_);
        data.AddData("ManaIncrement", manaIncrement_);
        data.AddData("CollectionRadius", collectionRadius_);
        data.AddData("CollectionTime", collectionTime_);
        data.AddData("DeathDelay", deathDelay_);
        data.AddData("Gravity", gravity_);
        data.AddData("MinDelay", minDelay_);
        data.AddData("GroundOffset", groundOffset_);
    }

    void GCCollectible::Deserialize(const Simian::FileObjectSubNode& data)
    {
        data.Data("HealthIncrement", healthIncrement_, healthIncrement_);
        data.Data("ManaIncrement", manaIncrement_, manaIncrement_);
        data.Data("CollectionRadius", collectionRadius_, collectionRadius_);
        data.Data("CollectionTime", collectionTime_, collectionTime_);
        data.Data("DeathDelay", deathDelay_, deathDelay_);
        data.Data("Gravity", gravity_, gravity_);
        data.Data("MinDelay", minDelay_, minDelay_);
        data.Data("GroundOffset", groundOffset_, groundOffset_);
    }
}
