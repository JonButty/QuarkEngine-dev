/*************************************************************************/
/*!
\file		GCParticleEmitterTriangle.h
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	2012/08/10
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*************************************************************************/
#ifndef DESCENSION_GCPARTICLEEMITTERTRIANGLE_H_
#define DESCENSION_GCPARTICLEEMITTERTRIANGLE_H_

#include "Simian/Vector3.h"
#include "Simian/EntityComponent.h"

namespace Simian
{
    class CTransform;
    class CParticleSystem;
}

namespace Descension
{
    class ParticleEmitterTriangle;

    class GCParticleEmitterTriangle: public Simian::EntityComponent
    {
    private:
        Simian::CParticleSystem* particles_;
        ParticleEmitterTriangle* emitter_;
        Simian::Vector3 vertices_[3];

        static Simian::FactoryPlant<Simian::EntityComponent, GCParticleEmitterTriangle> factoryPlant_;
    public:
        void Vertices(const Simian::Vector3* val);
    public:
        GCParticleEmitterTriangle();

        void OnAwake();
        void OnDeinit();

        void Serialize( Simian::FileObjectSubNode& data );
        void Deserialize( const Simian::FileObjectSubNode& data );
    };
}

#endif
