/************************************************************************/
/*!
\file		CParticleEmitterSphere.h
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	August 10, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/************************************************************************/
#ifndef SIMIAN_CPARTICLEEMITTERSPHERE_H_
#define SIMIAN_CPARTICLEEMITTERSPHERE_H_

#include "SimianPlatform.h"
#include "EntityComponent.h"

namespace Simian
{
    class ParticleEmitterSphere;
    class CParticleSystem;

    class SIMIAN_EXPORT CParticleEmitterSphere: public EntityComponent
    {
    private:
        ParticleEmitterSphere* emitter_;
        CParticleSystem* particleSystem_;
        f32 radius_;
        f32 randomRadius_;

        static FactoryPlant<EntityComponent, CParticleEmitterSphere> factoryPlant_;
    public:
        ParticleEmitterSphere* Emitter() const;
    public:
        CParticleEmitterSphere();

        void OnAwake();

        void Serialize( FileObjectSubNode& data );
        void Deserialize( const FileObjectSubNode& data );
    };
}

#endif
