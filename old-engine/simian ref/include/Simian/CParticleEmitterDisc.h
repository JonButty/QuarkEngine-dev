/************************************************************************/
/*!
\file		CParticleEmitterDisc.h
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	August 10, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/************************************************************************/
#ifndef SIMIAN_CPARTICLEEMITTERDISC_H_
#define SIMIAN_CPARTICLEEMITTERDISC_H_

#include "SimianPlatform.h"
#include "EntityComponent.h"

namespace Simian
{
    class ParticleEmitterDisc;
    class CParticleSystem;

    class SIMIAN_EXPORT CParticleEmitterDisc: public EntityComponent
    {
    private:
        ParticleEmitterDisc* emitter_;
        CParticleSystem* particleSystem_;
        f32 radius_;
        f32 randomRadius_;
        Vector3 normal_;
        Vector3 forward_;

        static FactoryPlant<EntityComponent, CParticleEmitterDisc> factoryPlant_;
    public:
        ParticleEmitterDisc* Emitter() const;

        Simian::f32 Radius() const;
        void Radius(Simian::f32 val);
        
        Simian::f32 RandomRadius() const;
        void RandomRadius(Simian::f32 val);
    public:
        CParticleEmitterDisc();

        void OnAwake();

        void Serialize( FileObjectSubNode& data );
        void Deserialize( const FileObjectSubNode& data );
    };
}

#endif
