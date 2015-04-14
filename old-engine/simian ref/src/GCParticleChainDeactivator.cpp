/*************************************************************************/
/*!
\file		GCParticleChainDeactivator.cpp
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	2012/08/12
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*************************************************************************/

#include "GCParticleChainDeactivator.h"
#include "ComponentTypes.h"
#include "Simian\CParticleSystem.h"
#include "Simian\ParticleSystem.h"
#include "Simian\Scene.h"
#include "Simian\EnginePhases.h"

namespace Descension
{
    Simian::FactoryPlant<Simian::EntityComponent, GCParticleChainDeactivator> GCParticleChainDeactivator::factoryPlant_(
        &Simian::GameFactory::Instance().ComponentFactory(), GC_PARTICLECHAINDEACTIVATOR);

    GCParticleChainDeactivator::GCParticleChainDeactivator()
        : duration_(0.0f),
          particlesDisabled_(false)
    {
    }

    //--------------------------------------------------------------------------

    Simian::f32 GCParticleChainDeactivator::Duration() const
    {
        return duration_;
    }

    void GCParticleChainDeactivator::Duration( Simian::f32 val )
    {
        duration_ = val;
    }

    //--------------------------------------------------------------------------

    void GCParticleChainDeactivator::update_( Simian::DelegateParameter& param )
    {
        Simian::EntityUpdateParameter* p;
        param.As(p);

        duration_ -= p->Dt;

        if (duration_ < 0.0f && !particlesDisabled_)
        {
            // scan all children and disable any particle systems
            Simian::f32 maxTimeOut = 0.0f;
            Simian::u32 size = ParentEntity()->Children().size();
            for (Simian::u32 i = 0; i < size; ++i)
            {
                Simian::Entity* entity = ParentEntity()->Children()[i];
                Simian::CParticleSystem* particles;
                entity->ComponentByType(Simian::C_PARTICLESYSTEM, particles);
                if (particles)
                {
                    particles->ParticleSystem()->Enabled(false);
                    Simian::f32 timeout = particles->ParticleSystem()->LifeTime() + 
                        particles->ParticleSystem()->RandomLifeTime();
                    maxTimeOut = timeout > maxTimeOut ? timeout : maxTimeOut;
                }
            }

            // set duration
            duration_ = maxTimeOut;

            particlesDisabled_ = true;
        }
        else if (duration_ < 0.0f && particlesDisabled_)
        {
            // remove the entity
            if (ParentEntity()->ParentEntity())
                ParentEntity()->RemoveChild(ParentEntity());
            else
                ParentScene()->RemoveEntity(ParentEntity());
        }
    }

    //--------------------------------------------------------------------------

    void GCParticleChainDeactivator::OnSharedLoad()
    {
        RegisterCallback(Simian::P_UPDATE, Simian::Delegate::CreateDelegate<GCParticleChainDeactivator, &GCParticleChainDeactivator::update_>(this));
    }

    void GCParticleChainDeactivator::Serialize( Simian::FileObjectSubNode& data )
    {
        data.AddData("Duration", duration_);
    }

    void GCParticleChainDeactivator::Deserialize( const Simian::FileObjectSubNode& data )
    {
        data.Data("Duration", duration_, duration_);
    }
}
