/************************************************************************/
/*!
\file		CParticleEmitterSphere.cpp
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	August 10, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/************************************************************************/
#include "Simian/CParticleEmitterSphere.h"
#include "Simian/ParticleSystem.h"
#include "Simian/EngineComponents.h"
#include "Simian/ParticleEmitterSphere.h"
#include "Simian/CParticleSystem.h"

namespace Simian
{
    FactoryPlant<EntityComponent, CParticleEmitterSphere> CParticleEmitterSphere::factoryPlant_(
        &GameFactory::Instance().ComponentFactory(), C_PARTICLEEMITTERSPHERE);

    CParticleEmitterSphere::CParticleEmitterSphere()
        : emitter_(0),
          particleSystem_(0),
          radius_(0),
          randomRadius_(0)
    {
    }

    //--------------------------------------------------------------------------

    ParticleEmitterSphere* CParticleEmitterSphere::Emitter() const
    {
        return emitter_;
    }

    //--------------------------------------------------------------------------

    void CParticleEmitterSphere::OnAwake()
    {
        ComponentByType(C_PARTICLESYSTEM, particleSystem_);
        if (!particleSystem_)
            return;

        // add the emitter
        emitter_ = particleSystem_->ParticleSystem()->AddEmitter<ParticleEmitterSphere>();
        emitter_->Radius(radius_);
        emitter_->RandomRadius(randomRadius_);
    }

    //--------------------------------------------------------------------------

    void CParticleEmitterSphere::Serialize( FileObjectSubNode& data )
    {
        data.AddData("Radius", emitter_ ? emitter_->Radius() : radius_);
        data.AddData("RandomRadius", emitter_ ? emitter_->RandomRadius() : randomRadius_);
    }

    void CParticleEmitterSphere::Deserialize( const FileObjectSubNode& data )
    {
        data.Data("Radius", radius_, radius_);
        data.Data("RandomRadius", randomRadius_, randomRadius_);

        if (!emitter_)
            return;

        emitter_->Radius(radius_);
        emitter_->RandomRadius(randomRadius_);
    }
}
