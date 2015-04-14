/*************************************************************************/
/*!
\file		GCSpellCollectible.cpp
\author 	Gavin Yeo, gavin.yeo, 290000111
\par    	email: gavin.yeo\@digipen.edu
\date   	2012/08/10
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*************************************************************************/
#include "GCSpellCollectible.h"
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
#include "Simian\LogManager.h"
#include "EntityTypes.h"

#include "GCPhysicsController.h"
#include "GCEditor.h"

namespace Descension
{
    const Simian::f32 VEL_SCALE = 0.5f;

    const Simian::f32 SPIN_VEL_MIN = 1.0f;
    const Simian::f32 SPIN_VEL_MAX = 10.0f;

    const Simian::f32 COLLECTION_TIME = 5.0f;

    const Simian::f32 COLLECTION_RADIUS = 5.0f;

    const Simian::f32 GROUND_OFFSET = 1.5f;

    Simian::FactoryPlant<Simian::EntityComponent, GCSpellCollectible> GCSpellCollectible::factoryPlant_(
        &Simian::GameFactory::Instance().ComponentFactory(), GC_SPELLCOLLECTIBLE);

    GCSpellCollectible::GCSpellCollectible()
        : playerCineTransform_(0),
          playerCinePhysics_(0),
          healthIncrement_(0),
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
          fsm_(CS_DROPPING, CS_TOTAL),
          collectibleType_(CT_TELESLASH)
    {
    }

    //--------------------------------------------------------------------------

    const Simian::Vector3& GCSpellCollectible::Velocity() const
    {
        return velocity_;
    }

    void GCSpellCollectible::Velocity( const Simian::Vector3& val )
    {
        velocity_ = val;
    }

    //--------------------------------------------------------------------------

    void GCSpellCollectible::onUpdate_( Simian::DelegateParameter& param )
    {
#ifdef _DESCENDED
        if (GCEditor::Instance().EditorMode())
            return;
#endif

        Simian::EntityUpdateParameter* p;
        param.As(p);
        fsm_.Update(p->Dt);
    }

    void GCSpellCollectible::droppingUpdate_( Simian::DelegateParameter& param )
    {
        Simian::FiniteStateUpdateParameter* p;
        param.As(p);

        timer_ += p->Dt;

        if (!playerCineTransform_)
            return;

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

        Simian::Vector3 dist = playerCineTransform_->World().Position() - transform_->World().Position();
        if (timer_ > minDelay_ && dist.LengthSquared() < collectionRadius_ * collectionRadius_)
        {
            timer_ = 0.0f;
            fsm_.ChangeState(CS_COLLECTING);
        }
    }

    void GCSpellCollectible::collectingUpdate_( Simian::DelegateParameter& param )
    {
        Simian::FiniteStateUpdateParameter* p;
        param.As(p);
        
        timer_ += p->Dt;
        Simian::f32 intp = timer_/collectionTime_;
        intp = Simian::Math::Clamp(intp, 0.0f, 1.0f);

        if (!playerCineTransform_)
            return;

        Simian::Vector3 destination = playerCineTransform_->World().Position() +
            Simian::Vector3(0, groundOffset_, 0);

        // get vector between this and player
        Simian::Vector3 pos = Simian::Interpolation::Interpolate(transform_->World().Position(), 
            destination, intp, Simian::Interpolation::Linear);

        // set the position
        transform_->Position(pos);

        if (playerCinePhysics_ && (transform_->World().Position() - destination).LengthSquared() < 
            playerCinePhysics_->Radius() * playerCinePhysics_->Radius())
        {
            // collect it!
            // Play sound
            if (sounds_)
                sounds_->Play(0);

            // Get particle effects
            Simian::Entity* particles = 0, *particles2 = 0;
            if (collectibleType_ == CT_TELESLASH)
                particles = ParentEntity()->ParentScene()->CreateEntity(E_TELESLASH_RECEIVE_PARTICLES);
            else if (collectibleType_ == CT_FIRE)
                particles = ParentEntity()->ParentScene()->CreateEntity(E_REGENHEALTHPARTICLES);
            else if (collectibleType_ == CT_LIGHTNING)
                particles = ParentEntity()->ParentScene()->CreateEntity(E_LIGHTNING_RECEIVE_PARTICLES);
            particles2 = ParentEntity()->ParentScene()->CreateEntity(E_TELESLASH_RECEIVE_PARTICLES2);
            if( particles && particles2)
            {
                ParentScene()->AddEntity(particles);
                Simian::CTransform* transferParticlestransform_;
                Simian::CParticleSystem* transferParticles_;
                particles->ComponentByType(Simian::C_TRANSFORM, transferParticlestransform_);
                particles->ComponentByType(Simian::C_PARTICLESYSTEM,transferParticles_);
                    
                if (playerCineTransform_)
                {
                    Simian::Vector3 vec(
                        playerCineTransform_->World().Position().X(),
                        groundOffset_,
                        playerCineTransform_->World().Position().Z());
                    transferParticlestransform_->Position(vec);
                }
                transferParticles_->ParticleSystem()->Enabled(true);

                ParentScene()->AddEntity(particles2);
                particles2->ComponentByType(Simian::C_TRANSFORM, transferParticlestransform_);
                particles2->ComponentByType(Simian::C_PARTICLESYSTEM,transferParticles_);
                    
                if (playerCineTransform_)
                {
                    Simian::Vector3 vec(
                        playerCineTransform_->World().Position().X(),
                        groundOffset_,
                        playerCineTransform_->World().Position().Z());
                    transferParticlestransform_->Position(vec);
                }
                transferParticles_->ParticleSystem()->Enabled(true);
            }

            fsm_.ChangeState(CS_DYING);
        }
    }

    void GCSpellCollectible::dyingUpdate_( Simian::DelegateParameter& param )
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

    void GCSpellCollectible::OnSharedLoad()
    {
        RegisterCallback(Simian::P_UPDATE, Simian::Delegate::CreateDelegate<GCSpellCollectible, &GCSpellCollectible::onUpdate_>(this));
    }

    void GCSpellCollectible::OnAwake()
    {
        ComponentByType(Simian::C_TRANSFORM, transform_);
        ComponentByType(Simian::C_SOUNDEMITTER, sounds_);

        yawVelocity_ = Simian::Math::Random(SPIN_VEL_MIN, SPIN_VEL_MAX);
        pitchVelocity_ = Simian::Math::Random(SPIN_VEL_MIN, SPIN_VEL_MAX);
        spinVelocity_ = Simian::Math::Random(SPIN_VEL_MIN, SPIN_VEL_MAX);

        fsm_[CS_DROPPING].OnUpdate = Simian::Delegate::CreateDelegate<GCSpellCollectible, &GCSpellCollectible::droppingUpdate_>(this);
        fsm_[CS_COLLECTING].OnUpdate = Simian::Delegate::CreateDelegate<GCSpellCollectible, &GCSpellCollectible::collectingUpdate_>(this);
        fsm_[CS_DYING].OnUpdate = Simian::Delegate::CreateDelegate<GCSpellCollectible, &GCSpellCollectible::dyingUpdate_>(this);
    }

    void GCSpellCollectible::OnInit()
    {
        Simian::Entity* playerCine_ = ParentScene()->EntityByTypeRecursive(E_PLAYER_CINE);
        if (playerCine_)
        {
            playerCine_->ComponentByType(Simian::C_TRANSFORM, playerCineTransform_);
            playerCine_->ComponentByType(GC_PHYSICSCONTROLLER, playerCinePhysics_);
        }
    }

    void GCSpellCollectible::Serialize( Simian::FileObjectSubNode& data )
    {
        data.AddData("HealthIncrement", healthIncrement_);
        data.AddData("ManaIncrement", manaIncrement_);
        data.AddData("CollectionRadius", collectionRadius_);
        data.AddData("CollectionTime", collectionTime_);
        data.AddData("DeathDelay", deathDelay_);
        data.AddData("Gravity", gravity_);
        data.AddData("MinDelay", minDelay_);
        data.AddData("GroundOffset", groundOffset_);
        data.AddData("CollectibleType", collectibleType_);
    }

    void GCSpellCollectible::Deserialize(const Simian::FileObjectSubNode& data)
    {
        data.Data("HealthIncrement", healthIncrement_, healthIncrement_);
        data.Data("ManaIncrement", manaIncrement_, manaIncrement_);
        data.Data("CollectionRadius", collectionRadius_, collectionRadius_);
        data.Data("CollectionTime", collectionTime_, collectionTime_);
        data.Data("DeathDelay", deathDelay_, deathDelay_);
        data.Data("Gravity", gravity_, gravity_);
        data.Data("MinDelay", minDelay_, minDelay_);
        data.Data("GroundOffset", groundOffset_, groundOffset_);
        data.Data("CollectibleType", collectibleType_, collectibleType_);
    }
}
