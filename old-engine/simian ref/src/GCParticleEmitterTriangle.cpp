/*************************************************************************/
/*!
\file		GCParticleEmitterTriangle.cpp
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	2012/08/10
\brief
Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*************************************************************************/
#include "GCParticleEmitterTriangle.h"
#include "ParticleEmitterTriangle.h"
#include "ComponentTypes.h"

#include "Simian\EngineComponents.h"
#include "Simian\CParticleSystem.h"
#include "Simian\ParticleSystem.h"

namespace Descension
{
    Simian::FactoryPlant<Simian::EntityComponent, GCParticleEmitterTriangle> GCParticleEmitterTriangle::factoryPlant_(
        &Simian::GameFactory::Instance().ComponentFactory(), GC_PARTICLEEMITTERTRIANGLE);

    GCParticleEmitterTriangle::GCParticleEmitterTriangle()
        : emitter_(0)
    {

    }
    
    //--------------------------------------------------------------------------

    void GCParticleEmitterTriangle::Vertices( const Simian::Vector3* val )
    {
        vertices_[0] = val[0];
        vertices_[1] = val[1];
        vertices_[2] = val[2];

        if (emitter_)
            emitter_->Vertices(vertices_);
    }

    //--------------------------------------------------------------------------

    void GCParticleEmitterTriangle::OnAwake()
    {
        // create emitter
        ComponentByType(Simian::C_PARTICLESYSTEM, particles_);
        emitter_ = particles_->ParticleSystem()->AddEmitter<ParticleEmitterTriangle>();

        emitter_->Vertices(vertices_);
    }

    void GCParticleEmitterTriangle::OnDeinit()
    {
    }

    void GCParticleEmitterTriangle::Serialize( Simian::FileObjectSubNode& data )
    {
        Simian::FileArraySubNode* vertices = data.AddArray("Vertices");
        for (Simian::u32 i = 0; i < 3; ++i)
        {
            Simian::FileObjectSubNode* vertex = vertices->AddObject("Vertex");
            vertex->AddData("X", vertices_[i].X());
            vertex->AddData("Y", vertices_[i].Y());
            vertex->AddData("Z", vertices_[i].Z());
        }
    }

    void GCParticleEmitterTriangle::Deserialize( const Simian::FileObjectSubNode& data )
    {
        const Simian::FileArraySubNode* vertices = data.Array("Vertices");
        if (vertices)
        {
            for (Simian::u32 i = 0; i < vertices->Size(); ++i)
            {
                const Simian::FileObjectSubNode* vertex = vertices->Object(i);
                float x, y, z;
                vertex->Data("X", x, vertices_[i].X());
                vertex->Data("Y", y, vertices_[i].Y());
                vertex->Data("Z", z, vertices_[i].Z());
                vertices_[i] = Simian::Vector3(x, y, z);
            }

            if (emitter_)
                emitter_->Vertices(vertices_);
        }
    }
}
