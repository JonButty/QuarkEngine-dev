/*************************************************************************/
/*!
\file		GCParticleEffectDeactivator.cpp
\author 	Gavin Yeo, gavin.yeo, 290000111
\par    	email: gavin.yeo\@digipen.edu
\date   	2012/09/29
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*************************************************************************/
#include "GCParticleEffectDeactivator.h"
#include "ComponentTypes.h"
#include "Simian/CParticleSystem.h"
#include "Simian/ParticleSystem.h"

#include "Simian/EnginePhases.h"
#include "Simian/EngineComponents.h"
#include "Simian/CTransform.h"
#include "Simian/Math.h"
#include "Simian/Interpolation.h"
#include "Simian/Scene.h"

namespace Descension
{
    Simian::FactoryPlant<Simian::EntityComponent, GCParticleEffectDeactivator> GCParticleEffectDeactivator::factoryPlant_(
        &Simian::GameFactory::Instance().ComponentFactory(), GC_PARTICLEEFFECTDEACTIVATOR);

    GCParticleEffectDeactivator::GCParticleEffectDeactivator()
        : particles_(0),
          maxTimer_(0),
          timer_(0),
          queueForRemoval_(false)
    {
    }

    //--------------------------------------------------------------------------

    Simian::f32 GCParticleEffectDeactivator::MaxTimer() const
    {
        return maxTimer_;
    }

    void GCParticleEffectDeactivator::MaxTimer( Simian::f32 val )
    {
        maxTimer_ = val;
    }

    //--------------------------------------------------------------------------

    // update_
    void GCParticleEffectDeactivator::update_( Simian::DelegateParameter& param )
    {
        Simian::f32 dt = param.As<Simian::EntityUpdateParameter>().Dt;

        timer_ += dt;
        if (timer_ > maxTimer_)
        {
            if (queueForRemoval_)
            {
                if (ParentEntity()->ParentEntity())
                    ParentEntity()->ParentEntity()->RemoveChild(ParentEntity());
                else
                    ParentScene()->RemoveEntity(ParentEntity());
            }
            else
            {
                queueForRemoval_ = true;
                timer_ = particles_->ParticleSystem()->LifeTime() + 
                    particles_->ParticleSystem()->RandomLifeTime();
                particles_->ParticleSystem()->Enabled(false);
            }
        }
    }

    //--------------------------------------------------------------------------

    void GCParticleEffectDeactivator::OnSharedLoad()
    {
        RegisterCallback(Simian::P_UPDATE, Simian::Delegate::CreateDelegate<GCParticleEffectDeactivator, &GCParticleEffectDeactivator::update_>(this));
    }

    void GCParticleEffectDeactivator::OnAwake()
    {
        timer_ = 0.0f;
        queueForRemoval_ = false;

        ParentEntity()->ComponentByType(Simian::C_PARTICLESYSTEM,particles_);
    }

    void GCParticleEffectDeactivator::Serialize( Simian::FileObjectSubNode& data )
    {
        data.AddData("MaxTimer", maxTimer_);
    }

    void GCParticleEffectDeactivator::Deserialize(const Simian::FileObjectSubNode& data)
    {
        data.Data("MaxTimer", maxTimer_, maxTimer_);
    }
}
