/*****************************************************************************/
/*!
\file		GCAshFadeEffect.cpp
\author 	Gavin Yeo Wei Ming, gavin.yeo, 290000111
\par    	email: gavin.yeo\@digipen.edu
\date   	August 10, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*****************************************************************************/

#include "GCAshFadeEffect.h"
#include "ComponentTypes.h"
#include "EntityTypes.h"

#include "Simian\EnginePhases.h"
#include "Simian\EngineComponents.h"
#include "Simian\CParticleSystem.h"
#include "Simian\ParticleSystem.h"
#include "Simian/Scene.h"

namespace Descension
{
    Simian::FactoryPlant<Simian::EntityComponent, GCAshFadeEffect> GCAshFadeEffect::factoryPlant_(
        &Simian::GameFactory::Instance().ComponentFactory(), GC_ASHFADEEFFECT);

    static const Simian::f32 DESTROY_TIMER_MAX = 3.0f;

    GCAshFadeEffect::GCAshFadeEffect()
        : enabled_(true),
          particles_(0),
          destroyTimer_(-1.0f)
    {
    }

    bool GCAshFadeEffect::Enabled() const
    {
        return enabled_;
    }

    void GCAshFadeEffect::Enabled( bool val )
    {
        enabled_ = val;
        particles_->ParticleSystem()->Enabled(enabled_);
        if (!enabled_)
            destroyTimer_ = DESTROY_TIMER_MAX;
    }

    //--------------------------------------------------------------------------

    void GCAshFadeEffect::update_( Simian::DelegateParameter& param )
    {
        Simian::f32 dt = param.As<Simian::EntityUpdateParameter>().Dt;

        if (destroyTimer_ > 0.0f)
        {
            destroyTimer_ -= dt;
            if (destroyTimer_ <= 0.0f)
            {
                if (ParentEntity()->ParentEntity())
                    ParentEntity()->ParentEntity()->RemoveChild(ParentEntity());
                else
                    ParentScene()->RemoveEntity(ParentEntity());
                destroyTimer_ = -1.0f;
            }
        }
    }

    
    //--------------------------------------------------------------------------

    void GCAshFadeEffect::OnSharedLoad()
    {
        RegisterCallback(Simian::P_UPDATE, 
            Simian::Delegate::CreateDelegate<GCAshFadeEffect, &GCAshFadeEffect::update_>(this));
    }

    void GCAshFadeEffect::OnAwake()
    {
    }
    
    void GCAshFadeEffect::OnInit()
    {
        ComponentByType(Simian::C_PARTICLESYSTEM, particles_);
        particles_->ParticleSystem()->Enabled(enabled_);
    }

    void GCAshFadeEffect::OnDeinit()
    {
    }

    void GCAshFadeEffect::Serialize( Simian::FileObjectSubNode& data )
    {
        data.AddData("Enabled", enabled_);
    }

    void GCAshFadeEffect::Deserialize( const Simian::FileObjectSubNode& data )
    {
        data.Data("Enabled", enabled_, enabled_);
    }

}
