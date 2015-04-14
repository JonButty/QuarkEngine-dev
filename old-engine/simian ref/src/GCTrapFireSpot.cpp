/*************************************************************************/
/*!
\file		GCTrapFireSpot.cpp
\author 	Gavin Yeo Wei Ming, gavin.yeo, 290000111
\par    	email: gavin.yeo\@digipen.edu
\date   	2012/08/10
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*************************************************************************/
#include "Simian/EngineComponents.h"
#include "Simian/EnginePhases.h"
#include "Simian/CTransform.h"
#include "Simian/Scene.h"
#include "Simian/Entity.h"
#include "Simian/Interpolation.h"
#include "Simian/CParticleSystem.h"
#include "Simian/ParticleSystem.h"
#include "Simian/CSoundEmitter.h"

#include "EntityTypes.h"
#include "ComponentTypes.h"
#include "Collision.h"
#include "CombatCommandSystem.h"
#include "CombatCommandDealDamage.h"

#include "GCTrapFireSpot.h"
#include "GCPhysicsController.h"
#include "GCAttributes.h"
#include "GCPlayerLogic.h"
#include "GCEditor.h"

namespace Descension
{
    Simian::FactoryPlant<Simian::EntityComponent, GCTrapFireSpot> GCTrapFireSpot::factoryPlant_(
        &Simian::GameFactory::Instance().ComponentFactory(), GC_TRAPFIRESPOT);

    GCTrapFireSpot::GCTrapFireSpot()
        : transform_(0),
          playerTransform_(0),
          attributes_(0),
          playerAttributes_(0),
          fireParticles1_(0),
          sounds_(0),
          radius_(0.4f),
          hitTimer_(0),
          hitDelay_(0.6f),
          soundTimerMax_(16.0f),
          soundTimer_(soundTimerMax_),
          fireDamage_(1),
          enabled_(true)
    {
    }

    //--------------------------------------------------------------------------

    Simian::CTransform& GCTrapFireSpot::Transform()
    {
        return *transform_;
    }

    bool GCTrapFireSpot::Enabled() const
    {
        return enabled_;
    }

    void GCTrapFireSpot::Enabled(bool val)
    {
        enabled_ = val;
        // Play particles
        fireParticles1_->ParticleSystem()->Enabled(enabled_);
    }

    //--------------------------------------------------------------------------

    void GCTrapFireSpot::update_( Simian::DelegateParameter& param )
    {
#ifdef _DESCENDED
        if (GCEditor::EditorMode())
            return;
#endif

        // Don't execute anything if trap is not enabled
        if (!enabled_)
            return;

        Simian::f32 dt = param.As<Simian::EntityUpdateParameter>().Dt;
        
        hitTimer_ += dt;

        Simian::Vector3 vecToTarget = transform_->World().Position() - playerTransform_->World().Position();
        Simian::f32 distance = vecToTarget.LengthSquared();
        if(hitTimer_ > hitDelay_ && distance < radius_)
        {
            CombatCommandSystem::Instance().AddCommand(attributes_, playerAttributes_,
                CombatCommandDealDamage::Create(fireDamage_));
            hitTimer_ = 0.0f;
        }

        // Play sound
        /*soundTimer_ += dt;
        if (soundTimer_ > soundTimerMax_)
        {
            soundTimer_ = 0.0f;
            sounds_->Play(0);
        }*/
    }

    //--------------------------------------------------------------------------

    void GCTrapFireSpot::OnSharedLoad()
    {
        RegisterCallback(Simian::P_UPDATE, Simian::Delegate::CreateDelegate<GCTrapFireSpot, &GCTrapFireSpot::update_>(this));
    }

    void GCTrapFireSpot::OnAwake()
    {
        ComponentByType(Simian::C_TRANSFORM, transform_);
        ComponentByType(Simian::C_SOUNDEMITTER, sounds_);
        ComponentByType(GC_ATTRIBUTES, attributes_);
    }

    void GCTrapFireSpot::OnInit()
    {
#ifdef _DESCENDED
        if (GCEditor::EditorMode())
            return;
#endif

        playerAttributes_ = GCPlayerLogic::Instance()->Attributes();
        GCPlayerLogic::Instance()->ParentEntity()->ParentEntity()->ComponentByType(Simian::C_TRANSFORM,playerTransform_);
        
        ParentEntity()->ComponentByType(Simian::C_PARTICLESYSTEM,fireParticles1_);

        fireParticles1_->ParticleSystem()->Enabled(enabled_);
    }

    void GCTrapFireSpot::Serialize( Simian::FileObjectSubNode& data )
    {
        data.AddData("Radius", radius_);
        data.AddData("HitDelay", hitDelay_);
        data.AddData("Damage", fireDamage_);
        data.AddData("Enabled", enabled_);
    }

    void GCTrapFireSpot::Deserialize( const Simian::FileObjectSubNode& data )
    {
        // get the data
        data.Data("Radius", radius_, radius_);
        radius_ *= radius_;
        data.Data("HitDelay", hitDelay_, hitDelay_);
        data.Data("Damage", fireDamage_, fireDamage_);
        data.Data("Enabled", enabled_, enabled_);
    }
}
